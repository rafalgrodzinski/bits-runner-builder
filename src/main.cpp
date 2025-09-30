#include <iostream>
#include <fstream>
#include <filesystem>

#include <llvm/Support/CommandLine.h>

#include "Lexer/Token.h"
#include "Lexer/Lexer.h"

#include "Parser/Parser.h"
#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementModule.h"

#include "Compiler/ModuleBuilder.h"
#include "Compiler/CodeGenerator.h"

#include "Logger.h"

using namespace std;

#define DEFAULT_MODULE_NAME "main"

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
    os << "Bits Runner Code, Version 0.1.0 (pre-alpha)\n";
}

int main(int argc, char **argv) {
    llvm::cl::SetVersionPrinter(versionPrinter);
    llvm::cl::OptionCategory mainOptions(" Main Options");
    llvm::cl::opt<bool> isVerbose(
        "v",
        llvm::cl::desc("Verbos output"),
        llvm::cl::init(false),
        llvm::cl::cat(mainOptions)
    );
    llvm::cl::opt<CodeGenerator::OutputKind> outputKind(
        llvm::cl::desc("Generated output:"),
        llvm::cl::init(CodeGenerator::OutputKind::OBJECT),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OutputKind::OBJECT, "c", "Generate object file"),
            clEnumValN(CodeGenerator::OutputKind::ASSEMBLY, "S", "Generate assembly file")
        ),
        llvm::cl::cat(mainOptions)
    );
    llvm::cl::opt<CodeGenerator::OptimizationLevel> optimizationLevel(
        llvm::cl::desc("Optimization level:"),
        llvm::cl::init(CodeGenerator::OptimizationLevel::O2),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OptimizationLevel::O0, "g", "No optimizations (debug mode)"),
            clEnumValN(CodeGenerator::OptimizationLevel::O0, "O0", "No optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O1, "O1", "Less optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O2, "O2", "Default optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O3, "O3", "Aggressive optimizations")
        ),
        llvm::cl::cat(mainOptions)
    );
    llvm::cl::list<string> inputFileNames(
        llvm::cl::Positional,
        llvm::cl::desc("<input file>"),
        llvm::cl::OneOrMore,
        llvm::cl::cat(mainOptions)
    );
    llvm::cl::OptionCategory targetOptions(" Target Options");
    llvm::cl::opt<string> targetTriple(
        "triple",
        llvm::cl::desc("Target triple"),
        llvm::cl::cat(targetOptions)
    );
    llvm::cl::opt<string> architecture(
        "arch",
        llvm::cl::desc("Target architecture"),
        llvm::cl::cat(targetOptions)
    );
    llvm::cl::bits<CodeGenerator::Options> options(
        llvm::cl::desc("Target gneration options"),
        llvm::cl::values(
            clEnumValN(CodeGenerator::Options::FUNCTION_SECTIONS, "function-sections", "Place each function in its own section")
        ),
        llvm::cl::cat(targetOptions)
    );
    llvm::cl::extrahelp("\n More Info:\n\n  Check the GitHub page at https://github.com/rafalgrodzinski/bits-runner-builder for more information in Bits Runner Builder\n");
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bits Runner Builder - LLVM based compiler for the Bits Runner Code language");

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
    // For each source, scan it, parse it, and then fill appropriate maps (corresponding to the defined modules)
    for (int i=0; i<sources.size(); i++) {
        if (isVerbose)
            cout << format("🔍 Scanning \"{}\"", inputFileNames[i]) << endl << endl;

        Lexer lexer(sources[i]);
        vector<shared_ptr<Token>> tokens = lexer.getTokens();
        if (isVerbose) {
            Logger::print(tokens);
            cout << endl;
        }

        if (isVerbose)
            cout << format("🧸 Parsing \"{}\"", inputFileNames[i]) << endl << endl;
        Parser parser(DEFAULT_MODULE_NAME, tokens);
        shared_ptr<StatementModule> statementModule = parser.getStatementModule();

        if (isVerbose) {
            Logger::print(statementModule);
            cout << endl;
        }

        // Append statements to existing module or create a new one
        if (statementsMap.contains(statementModule->getName())) {
            for (shared_ptr<Statement> &statement : statementModule->getStatements())
                statementsMap[statementModule->getName()].push_back(statement);
            for (shared_ptr<Statement> &headerStatement : statementModule->getHeaderStatements())
                headerStatementsMap[statementModule->getName()].push_back(headerStatement);
            for (shared_ptr<Statement> &exportedHeaderStatement : statementModule->getExportedHeaderStatements())
                exportedHeaderStatementsMap[statementModule->getName()].push_back(exportedHeaderStatement);
        } else {
            statementsMap[statementModule->getName()] = statementModule->getStatements();
            headerStatementsMap[statementModule->getName()] = statementModule->getHeaderStatements();
            exportedHeaderStatementsMap[statementModule->getName()] = statementModule->getExportedHeaderStatements();
        }
    }

    for (const auto &statementsEntry : statementsMap) {
        if (isVerbose)
            cout << format("🦖 Building module \"{}\"", statementsEntry.first) << endl << endl;

        // we don't want any prefix for the default module
        string moduleName = statementsEntry.first;
        vector<shared_ptr<Statement>> statements = statementsEntry.second;
        vector<shared_ptr<Statement>> headerStatements = headerStatementsMap[moduleName];

        ModuleBuilder moduleBuilder(moduleName, DEFAULT_MODULE_NAME, statements, headerStatements, exportedHeaderStatementsMap);
        shared_ptr<llvm::Module> module = moduleBuilder.getModule();

        if (isVerbose) {
            module->print(llvm::outs(), nullptr);
            cout << endl;
        }

        CodeGenerator codeGenerator(module);
        codeGenerator.generateObjectFile(outputKind, optimizationLevel, targetTriple, architecture, isVerbose, options.getBits());
    }

    return 0;
}