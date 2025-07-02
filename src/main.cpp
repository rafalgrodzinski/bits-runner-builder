#include <iostream>
#include <fstream>
#include <filesystem>

#include <llvm/Support/CommandLine.h>

#include "Lexer/Token.h"
#include "Lexer/Lexer.h"

#include "Parser/Parser.h"
#include "Parser/Statement/Statement.h"

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
        llvm::cl::desc("Choose generated output:"),
        llvm::cl::init(CodeGenerator::OutputKind::OBJECT),
        llvm::cl::values(
            clEnumValN(CodeGenerator::OutputKind::OBJECT, "c", "Generate object file"),
            clEnumValN(CodeGenerator::OutputKind::ASSEMBLY, "S", "Generate assembly file")
        )
    );
    llvm::cl::opt<string> inputFileName(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::Required);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bits Runner Builder - LLVM based compiler for the Bits Runner Code language");

    filesystem::path inputFilePath((string(inputFileName)));
    string source = readFile(inputFilePath);
    string moduleName = inputFilePath.filename().replace_extension();

    Lexer lexer(source);
    vector<shared_ptr<Token>> tokens = lexer.getTokens();
    if (isVerbose) {
        Logger::print(tokens);
        cout << endl;
    }

    Parser parser(tokens);
    vector<shared_ptr<Statement>> statements = parser.getStatements();
    if (isVerbose) {
        Logger::print(statements);
        cout << endl;
    }

    /*ModuleBuilder moduleBuilder(moduleName, inputFilePath, statements);
    shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    if (isVerbose) {
        module->print(llvm::outs(), nullptr);
    }

    CodeGenerator codeGenerator(module);
    codeGenerator.generateObjectFile(outputKind);*/

    return 0;
}