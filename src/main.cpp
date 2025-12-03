#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

#include <llvm/Support/CommandLine.h>

#include "Lexer/Token.h"
#include "Lexer/Lexer.h"

#include "Parser/Parser.h"
#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementModule.h"

#include "Analyzer/TypesAnalyzer.h"

#include "Compiler/ModuleBuilder.h"
#include "Compiler/CodeGenerator.h"

#include "Logger.h"

using namespace std;

#define DEFAULT_MODULE_NAME "main"

enum class Verbosity {
    V0,
    V1,
    V2,
    V3
};

string readFile(filesystem::path filePath) {
    ifstream file(filePath, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Cannot open file " << filePath << endl;
        exit(1);
    }

    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);
    vector<char> fileBytes(fileSize);
    file.read(fileBytes.data(), fileSize);
    return string(fileBytes.data(), fileSize);
}

void versionPrinter(llvm::raw_ostream &os) {
    os << "Bits Runner Builder, version " << VERSION << "\n";
}

int main(int argc, char **argv) {
    llvm::cl::SetVersionPrinter(versionPrinter);

    // Main Options
    llvm::cl::OptionCategory mainOptions(" Main Options");

    // verbosity
    llvm::cl::opt<Verbosity> verbosity(
        "ver",
        llvm::cl::desc("Output verbosity:"),
        llvm::cl::init(Verbosity::V1),
        llvm::cl::values(
            clEnumValN(Verbosity::V0, "v0", "Errors only"),
            clEnumValN(Verbosity::V1, "v1", "v0 + Current action (default)"),
            clEnumValN(Verbosity::V2, "v2", "v1 + time statistics"),
            clEnumValN(Verbosity::V3, "v3", "v2 + detailed scan & parse logs")
        ),
        llvm::cl::cat(mainOptions)
    );

    // output kind
    llvm::cl::opt<CodeGenerator::OutputKind> outputKind(
        "gen",
        llvm::cl::desc("Generated output:"),
        llvm::cl::init(CodeGenerator::OutputKind::OBJECT),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OutputKind::OBJECT, "obj", "Generate object file (Default)"),
            clEnumValN(CodeGenerator::OutputKind::ASSEMBLY, "asm", "Generate assembly file"),
            clEnumValN(CodeGenerator::OutputKind::IR, "ir", "Generate LLVM IR")
        ),
        llvm::cl::cat(mainOptions)
    );

    // input files
    llvm::cl::list<string> inputFileNames(
        llvm::cl::Positional,
        llvm::cl::desc("<input file>"),
        llvm::cl::Optional,
        llvm::cl::cat(mainOptions)
    );

    // Target Options
    llvm::cl::OptionCategory targetOptions(" Target Options");

    // target triple
    llvm::cl::opt<string> targetTriple(
        "triple",
        llvm::cl::desc("Target triple"),
        llvm::cl::cat(targetOptions)
    );

    // architecture
    llvm::cl::opt<string> architecture(
        "arch",
        llvm::cl::desc("Target architecture"),
        llvm::cl::cat(targetOptions)
    );

    // relocation model
    llvm::cl::opt<CodeGenerator::RelocationModel> relocationModel(
        "reloc",
        llvm::cl::desc("Relocation model:"),
        llvm::cl::init(CodeGenerator::RelocationModel::PIC),
        llvm::cl::values(
            clEnumValN(CodeGenerator::RelocationModel::STATIC, "static", "Non-relocatable code"),
            clEnumValN(CodeGenerator::RelocationModel::PIC, "pic", "Fully relocatable position independent code (Default)")
        ),
        llvm::cl::cat(targetOptions)
    );

    // code model
    llvm::cl::opt<CodeGenerator::CodeModel> codeModel(
        "code",
        llvm::cl::desc("Code model:"),
        llvm::cl::init(CodeGenerator::CodeModel::SMALL),
        llvm::cl::values(
            clEnumValN(CodeGenerator::CodeModel::TINY, "tiny", "Tiny code model"),
            clEnumValN(CodeGenerator::CodeModel::SMALL, "small", "Small code model (Default)"),
            clEnumValN(CodeGenerator::CodeModel::KERNEL, "kernel", "Kernel code model"),
            clEnumValN(CodeGenerator::CodeModel::MEDIUM, "medium", "Medium code model"),
            clEnumValN(CodeGenerator::CodeModel::LARGE, "large", "Large code model")
        ),
        llvm::cl::cat(targetOptions)
    );

    // optimization level
    llvm::cl::opt<CodeGenerator::OptimizationLevel> optimizationLevel(
        "opt",
        llvm::cl::desc("Optimization level:"),
        llvm::cl::init(CodeGenerator::OptimizationLevel::O2),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OptimizationLevel::O0, "g", "No optimizations with debug symbols"),
            clEnumValN(CodeGenerator::OptimizationLevel::O0, "o0", "No optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O1, "o1", "Basic optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O2, "o2", "Some optimizations (Default)"),
            clEnumValN(CodeGenerator::OptimizationLevel::O3, "o3", "Aggressive optimizations")
        ),
        llvm::cl::cat(targetOptions)
    );

    // calling convention
    llvm::cl::opt<CodeGenerator::CallingConvention> callingConvention(
        "call",
        llvm::cl::desc("Calling convention:"),
        llvm::cl::init(CodeGenerator::CallingConvention::CDECL),
        llvm::cl::values(
            clEnumValN(CodeGenerator::CallingConvention::CDECL, "cdecl", "C Declaration - Standard C calling convention (Default)"),
            clEnumValN(CodeGenerator::CallingConvention::STDCALL, "stdcall", "Used by the Win32 API, calee clears the stack"),
            clEnumValN(CodeGenerator::CallingConvention::FASTCALL, "fastcall", "Fast Call - first 2,3 arguments passed in registers"),
            clEnumValN(CodeGenerator::CallingConvention::TAIL, "tail", "Allows for tail call optimizations")
        ),
        llvm::cl::cat(targetOptions)
    );

    // options
    llvm::cl::bits<CodeGenerator::Options> options(
        llvm::cl::desc("Additional options"),
        llvm::cl::values(
            clEnumValN(CodeGenerator::Options::FUNCTION_SECTIONS, "function-sections", "Place each function in its own section"),
            clEnumValN(CodeGenerator::Options::NO_BSS, "no-zero-initialized-in-bss", "Don't place zero initialized data in BSS")
        ),
        llvm::cl::cat(targetOptions)
    );

    llvm::cl::extrahelp("\n More Info:\n\n  Check the GitHub page at https://github.com/rafalgrodzinski/bits-runner-builder for more information on the BRC language\n");
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bits Runner Builder - LLVM based compiler for the Bits Runner Code language");

    // Check if input files have been provided
    if (inputFileNames.empty()) {
        cout << "🔥 No input files have been provided." << endl;
        cout << "For more information try brb --help or check the GitHub page at https://github.com/rafalgrodzinski/bits-runner-builder" << endl;
        exit(1);
    }

    // Read each source
    vector<string> sources;
    for (string &inputFileName : inputFileNames) {
        filesystem::path inputFilePath(inputFileName);
        string source = readFile(inputFilePath);
        sources.push_back(source);
    }

    map<string, vector<shared_ptr<Statement>>> statementsMap;
    map<string, vector<shared_ptr<Statement>>> headerStatementsMap;
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap;

    time_t totalScanTime = 0;
    time_t totalParseTime = 0;
    time_t totalAnalysisTime = 0;
    time_t totalModuleBuildTime = 0;
    time_t totalCodeGnerationTime = 0;

    time_t totalTimeStamp = clock();
    // For each source, scan it, parse it, and then fill appropriate maps (corresponding to the defined modules)
    for (int i=0; i<sources.size(); i++) {
        time_t timeStamp;
    
        // Scanning
        if (verbosity >= Verbosity::V1)
            cout << format("🔍 Scanning \"{}\"", inputFileNames[i]) << endl;

        timeStamp = clock();
        Lexer lexer(sources[i]);
        vector<shared_ptr<Token>> tokens = lexer.getTokens();
        timeStamp = clock() - timeStamp;
        totalScanTime += timeStamp;

        if (verbosity >= Verbosity::V2)
            cout << format("⏱️ Scanned \"{}\" in {:.6f} seconds", inputFileNames[i], (float)timeStamp / CLOCKS_PER_SEC) << endl << endl;
    
        if (verbosity >= Verbosity::V3) {
            Logger::print(tokens);
            cout << endl;
        }

        // Parsing
        if (verbosity >= Verbosity::V1)
            cout << format("🧸 Parsing \"{}\"", inputFileNames[i]) << endl;

        timeStamp = clock();
        Parser parser(DEFAULT_MODULE_NAME, tokens);
        shared_ptr<StatementModule> statementModule = parser.getStatementModule();

        // Append statements to existing module or create a new one
        if (statementsMap.contains(statementModule->getName())) {
            for (shared_ptr<Statement> &statement : statementModule->getStatements())
                statementsMap[statementModule->getName()].push_back(statement);
            for (shared_ptr<Statement> &headerStatement : statementModule->getHeaderStatements()) {
                if (headerStatement->getKind() == StatementKind::BLOB_DECLARATION) {
                    headerStatementsMap[statementModule->getName()].insert(
                        headerStatementsMap[statementModule->getName()].begin(),
                        headerStatement
                    );
                } else {
                    headerStatementsMap[statementModule->getName()].push_back(headerStatement);
                }
            }
            for (shared_ptr<Statement> &exportedHeaderStatement : statementModule->getExportedHeaderStatements())
                exportedHeaderStatementsMap[statementModule->getName()].push_back(exportedHeaderStatement);
        } else {
            statementsMap[statementModule->getName()] = statementModule->getStatements();
            headerStatementsMap[statementModule->getName()] = statementModule->getHeaderStatements();
            exportedHeaderStatementsMap[statementModule->getName()] = statementModule->getExportedHeaderStatements();
        }

        timeStamp = clock() - timeStamp;
        totalParseTime += timeStamp;
        if (verbosity >= Verbosity::V2)
            cout << format("⏱️ Parsed \"{}\" in {:.6f} seconds", inputFileNames[i], (float)timeStamp / CLOCKS_PER_SEC) << endl << endl;
    }

    // Analysis
    for (const auto &statementsEntry : statementsMap) {
        time_t timeStamp;
    
        string moduleName = statementsEntry.first;
        vector<shared_ptr<Statement>> statements = statementsEntry.second;
        vector<shared_ptr<Statement>> headerStatements = headerStatementsMap[moduleName];

        if (verbosity >= Verbosity::V1)
            cout << format("🔮 Analyzing module \"{}\"", moduleName) << endl;

        timeStamp = clock();
        TypesAnalyzer typesAnalyzer(statements, headerStatements, exportedHeaderStatementsMap);
        typesAnalyzer.checkModule();
        timeStamp = clock() - timeStamp;
        totalAnalysisTime += timeStamp;

        if (verbosity >= Verbosity::V2)
            cout << format("⏱️ Analyzed module \"{}\" in {:.6f} seconds", moduleName, (float)timeStamp / CLOCKS_PER_SEC) << endl << endl;

        if (verbosity >= Verbosity::V3) {
            // reconstruct concatenated module 
            shared_ptr<StatementModule> statementModule = make_shared<StatementModule>(
                moduleName,
                statements,
                headerStatementsMap[moduleName],
                vector<shared_ptr<Statement>>(),
                0,
                0
            );
            Logger::print(statementModule);
            cout << endl;
        }
    }

    // Specify code generator for deired target
    CodeGenerator codeGenerator(targetTriple, architecture, relocationModel, codeModel, optimizationLevel, callingConvention, options.getBits());

    for (const auto &statementsEntry : statementsMap) {
        time_t timeStamp;

        if (verbosity >= Verbosity::V1)
            cout << format("🐄 Building module \"{}\"", statementsEntry.first) << endl;

        // we don't want any prefix for the default module
        string moduleName = statementsEntry.first;
        vector<shared_ptr<Statement>> statements = statementsEntry.second;
        vector<shared_ptr<Statement>> headerStatements = headerStatementsMap[moduleName];

        timeStamp = clock();
        ModuleBuilder moduleBuilder(
            moduleName,
            DEFAULT_MODULE_NAME,
            codeGenerator.getIntSize(),
            codeGenerator.getPointerSize(),
            codeGenerator.getCallingConvetion(),
            statements,
            headerStatements,
            exportedHeaderStatementsMap
        );
        shared_ptr<llvm::Module> module = moduleBuilder.getModule();
        timeStamp = clock() - timeStamp;
        totalModuleBuildTime += timeStamp;

        if (verbosity >= Verbosity::V2)
            cout << format("⏱️ Built module \"{}\" in {:.6f} seconds", moduleName, (float)timeStamp / CLOCKS_PER_SEC) << endl << endl;

        // Generate native machine code
        timeStamp = clock();
        codeGenerator.generateObjectFile(module, outputKind, verbosity >= Verbosity::V1);
        timeStamp = clock() - timeStamp;
        totalCodeGnerationTime += timeStamp;

        if (verbosity >= Verbosity::V2)
            cout << format("⏱️ Generated code for \"{}\" in {:.6f} seconds", moduleName, (float)timeStamp / CLOCKS_PER_SEC) << endl << endl;
    }
    totalTimeStamp = clock() - totalTimeStamp;

    if (verbosity >= Verbosity::V2) {
        cout << "⏱️ Time taken" << endl;
        cout << format("Total: {:.6f} seconds", (float)totalTimeStamp / CLOCKS_PER_SEC) << endl;
        cout << format("Scanning: {:.6f} seconds ({:.2f}%)", (float)totalScanTime / CLOCKS_PER_SEC, (float)totalScanTime / totalTimeStamp * 100) << endl;
        cout << format("Parsing: {:.6f} seconds ({:.2f}%)", (float)totalParseTime / CLOCKS_PER_SEC, (float)totalParseTime / totalTimeStamp * 100) << endl;
        cout << format("Analysis: {:.6f} seconds ({:.2f}%)", (float)totalAnalysisTime / CLOCKS_PER_SEC, (float)totalAnalysisTime / totalTimeStamp * 100) << endl;
        cout << format("Module building: {:.6f} seconds ({:.2f}%)", (float)totalModuleBuildTime / CLOCKS_PER_SEC, (float)totalModuleBuildTime / totalTimeStamp * 100) << endl;
        cout << format("Code generation: {:.6f} seconds ({:.2f}%)", (float)totalCodeGnerationTime / CLOCKS_PER_SEC, (float)totalCodeGnerationTime / totalTimeStamp * 100) << endl;
    }

    return 0;
}