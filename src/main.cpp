#include <iostream>
#include <fstream>
#include <filesystem>

#include "llvm/Support/CommandLine.h"

#include "Token.h"
#include "Lexer.h"

#include "Expression.h"
#include "Parser.h"

#include "ModuleBuilder.h"
#include "CodeGenerator.h"


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
    llvm::cl::opt<string> inputFileName(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::Required);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bits Runner Builder - LLVM based compiler for the Bits Runner Code language");

    filesystem::path inputFilePath((string(inputFileName)));
    string source = readFile(inputFilePath);
    string moduleName = inputFilePath.filename().replace_extension();

    Lexer lexer(source);
    vector<shared_ptr<Token>> tokens = lexer.getTokens();
    if (isVerbose) {
        for (int i=0; i<tokens.size(); i++) {
            cout << i << "|" << tokens.at(i)->toString();
            if (i < tokens.size() - 1)
                cout << ", ";
        }
        cout << endl << endl;
    }

    Parser parser(tokens);
    vector<shared_ptr<Statement>> statements = parser.getStatements();
    if (isVerbose) {
        for (shared_ptr<Statement> &statement : statements) {
            cout << statement->toString(0);
            cout << endl;
        }
        cout << endl << endl;
    }

    ModuleBuilder moduleBuilder(moduleName, inputFilePath, statements);
    shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    if (isVerbose) {
        module->print(llvm::outs(), nullptr);
    }

    //CodeGenerator codeGenerator(module);
    //codeGenerator.generateObjectFile("dummy.s");

    return 0;
}