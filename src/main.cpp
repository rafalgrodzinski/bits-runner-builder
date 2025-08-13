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
    os << "Bits Runner Code, Version 1.0.0 (pre-alpha)\n";
}

int main(int argc, char **argv) {
    llvm::cl::SetVersionPrinter(versionPrinter);
    llvm::cl::extrahelp("\nADDITIONAL HELP:\n\n  This is the extra help\n");
    llvm::cl::opt<bool> isVerbose("v", llvm::cl::desc("Verbos output"), llvm::cl::init(false));
    llvm::cl::opt<CodeGenerator::OutputKind> outputKind(
        llvm::cl::desc("Generated output:"),
        llvm::cl::init(CodeGenerator::OutputKind::OBJECT),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OutputKind::OBJECT, "c", "Generate object file"),
            clEnumValN(CodeGenerator::OutputKind::ASSEMBLY, "S", "Generate assembly file")
        )
    );
    llvm::cl::opt<CodeGenerator::OptimizationLevel> optimizationLevel(
        llvm::cl::desc("Optimization level:"),
        llvm::cl::init(CodeGenerator::OptimizationLevel::O2),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OptimizationLevel::O0, "O0", "No optimizations (debug)"),
            clEnumValN(CodeGenerator::OptimizationLevel::O1, "O1", "Less optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O2, "O2", "Default optimizations"),
            clEnumValN(CodeGenerator::OptimizationLevel::O3, "O3", "Aggressive optimizations")
        )
    );
    llvm::cl::list<string> inputFileNames(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::OneOrMore);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bits Runner Builder - LLVM based compiler for the Bits Runner Code language");

    // Read each source
    vector<string> sources;
    for (string &inputFileName : inputFileNames) {
        filesystem::path inputFilePath(inputFileName);
        string source = readFile(inputFilePath);
        sources.push_back(source);
    }

    map<string, vector<shared_ptr<Statement>>> statementsMap;
    // For each source, scan it, parse it, and then fill appropriate maps (corresponding to the defined modules)
    for (int i=0; i<sources.size(); i++) {
        if (isVerbose)
            cout << format("🔍 Scanning {}", inputFileNames[i]) << endl << endl;

        Lexer lexer(sources[i]);
        vector<shared_ptr<Token>> tokens = lexer.getTokens();
        if (isVerbose) {
            Logger::print(tokens);
            cout << endl;
        }

        if (isVerbose)
            cout << format("🧸 Parsing {}", inputFileNames[i]) << endl << endl;
        Parser parser(tokens);
        shared_ptr<StatementModule> statementModule = parser.getStatementModule();

        if (isVerbose) {
            Logger::print(statementModule);
            cout << endl;
        }

        // Append statements to existing module or create a new one
        if (statementsMap.contains(statementModule->getName())) {
            for (shared_ptr<Statement> &statement : statementModule->getStatements())
                statementsMap[statementModule->getName()].push_back(statement);
        } else {
            statementsMap[statementModule->getName()] = statementModule->getStatements();
        }
    }

    for (const auto &statementsEntry : statementsMap) {
        if (isVerbose)
            cout << format("🪄 Compiling module {}", statementsEntry.first) << endl << endl;

        ModuleBuilder moduleBuilder(statementsEntry.first, statementsEntry.second);
        shared_ptr<llvm::Module> module = moduleBuilder.getModule();

        if (isVerbose)
            module->print(llvm::outs(), nullptr);

        CodeGenerator codeGenerator(module);
        codeGenerator.generateObjectFile(outputKind, optimizationLevel);
    }

    return 0;
}