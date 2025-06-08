#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"

#include "Expression.h"
#include "Parser.h"

#include "ModuleBuilder.h"
#include "CodeGenerator.h"


using namespace std;

string readFile(string fileName) {
    ifstream file(fileName.c_str(), ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Cannot open file " << fileName << endl;
        exit(1);
    }

    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);
    vector<char> fileBytes(fileSize);
    file.read(fileBytes.data(), fileSize);
    return string(fileBytes.data(), fileSize);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Need to provide a file name" << endl;
        exit(1);
    }

    string source = readFile(string(argv[1]));
    Lexer lexer(source);
    vector<shared_ptr<Token>> tokens = lexer.getTokens();
    for (int i=0; i<tokens.size(); i++) {
        cout << tokens.at(i)->toString();
        if (i < tokens.size() - 1)
            cout << " ";
    }
    cout << endl << endl;

    Parser parser(tokens);
    vector<shared_ptr<Statement>> statements = parser.getStatements();
    for (shared_ptr<Statement> &statement : statements) {
        cout << statement->toString(0);
        cout << endl;
    }
    cout << endl << endl;

    ModuleBuilder moduleBuilder(statements);
    shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    module->print(llvm::outs(), nullptr);

    //CodeGenerator codeGenerator(module);
    //codeGenerator.generateObjectFile("dummy.s");

    return 0;
}