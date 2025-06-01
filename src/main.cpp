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
    std::vector<Token> tokens = lexer.getTokens();
    if (tokens.empty()) {
        exit(1);
    }
    for (Token &token : tokens)
        std::cout << token.toString() << " ";
    std::cout << std::endl;

    Parser parser(tokens);
    vector<shared_ptr<Statement>> statements = parser.getStatements();
    if (statements.empty()) {
        exit(1);
    }
    for (shared_ptr<Statement> &statement : statements) {
        cout << statement->toString();
        cout << endl;
    }
    //shared_ptr<Expression> expression = parser.getExpression();
    //if (!expression) {
    //    exit(1);
    //}
    //cout << expression->toString() << endl;

    //ModuleBuilder moduleBuilder(expression);
    //shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    //module->print(llvm::outs(), nullptr);

    //CodeGenerator codeGenerator(module);;
    //codeGenerator.generateObjectFile("dummy.s");

    return 0;
}