#include "CodeGenerator.h"

using namespace std;

CodeGenerator::CodeGenerator(shared_ptr<llvm::Module> module): module(module) {
}

void CodeGenerator::generateObjectFile(string fileName) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    string targetTriple = llvm::sys::getDefaultTargetTriple();
    string errorString;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, errorString);
    if (!target) {
        cerr << errorString << endl;
        exit(1);
    }

    llvm::TargetOptions options;
    llvm::TargetMachine *targetMachine = target->createTargetMachine(targetTriple, "generic", "", options, llvm::Reloc::PIC_);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);

    error_code errorCode;
    llvm::raw_fd_ostream outputFile(fileName, errorCode, llvm::sys::fs::OF_None);
    if (errorCode) {
        cerr << errorCode.message();
        exit(1);
    }

    llvm::legacy::PassManager passManager;
    if (targetMachine->addPassesToEmitFile(passManager, outputFile, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
        cerr << "Failed to emit file" << endl;
        exit(1);
    }

    passManager.run(*module);
    outputFile.flush();
}