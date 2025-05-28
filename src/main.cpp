#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Token.h"

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
    for (Token &token : tokens)
        std::cout << token.toString() << " ";
    std::cout << std::endl;

    return 0;
}