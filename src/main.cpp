#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"

#include "Expression.h"
#include "Parser.h"
#include "ModuleBuilder.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/TargetParser/Host.h"


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
    for (Token &token : tokens)
        std::cout << token.toString() << " ";
    std::cout << std::endl;

    Parser parser(tokens);
    shared_ptr<Expression> expression = parser.getExpression();
    cout << expression->toString() << endl;

    ModuleBuilder moduleBuilder(expression);
    shared_ptr<llvm::Module> module = moduleBuilder.getModule();
    module->print(llvm::outs(), nullptr);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    string targetTriple = llvm::sys::getDefaultTargetTriple();
    cout << targetTriple << endl;
    string errorString;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, errorString);
    cout << errorString << endl;

    llvm::TargetOptions options;
    llvm::TargetMachine *targetMachine = target->createTargetMachine(targetTriple, "generic", "", options, llvm::Reloc::PIC_);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);

    error_code errorCode;
    llvm::raw_fd_ostream outputFile("test.o", errorCode, llvm::sys::fs::OF_None);
    if (errorCode) {
        cout << errorCode.message();
        exit(1);
    }

    llvm::legacy::PassManager passManager;
    if(targetMachine->addPassesToEmitFile(passManager, outputFile, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
        cout << "failed" << endl;
        exit(1);
    }
    passManager.run(*module);
    outputFile.flush();

    return 0;
}