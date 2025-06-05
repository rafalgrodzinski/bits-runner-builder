#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"

#include "Expression.h"
#include "Parser.h"

#include "ModuleBuilder.h"
#include "CodeGenerator.h"


using namespace std;

std::string readFile(std::string fileName) {
    std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        exit(1);
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> fileBytes(fileSize);
    file.read(fileBytes.data(), fileSize);
    return std::string(fileBytes.data(), fileSize);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Need to provide a file name" << std::endl;
        exit(1);
    }

    std::string source = readFile(std::string(argv[1]));
    Lexer lexer(source);
    std::vector<shared_ptr<Token>> tokens = lexer.getTokens();
    for (int i=0; i<tokens.size(); i++) {
        std::cout << tokens.at(i)->toString();
        if (i < tokens.size() - 1)
            std::cout << " ";
    }
    std::cout << std::endl;

    Parser parser(tokens);
    vector<shared_ptr<Statement>> statements = parser.getStatements();
    for (shared_ptr<Statement> &statement : statements) {
        cout << statement->toString();
        cout << endl;
    }

    ModuleBuilder moduleBuilder(statements);
    shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    module->print(llvm::outs(), nullptr);

    //CodeGenerator codeGenerator(module);
    //codeGenerator.generateObjectFile("dummy.s");

    return 0;
}