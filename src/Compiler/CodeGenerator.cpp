#include "CodeGenerator.h"

using namespace std;

CodeGenerator::CodeGenerator(shared_ptr<llvm::Module> module): module(module) {
}

void CodeGenerator::generateObjectFile(OutputKind outputKind) {
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

    string fileName;
    llvm::CodeGenFileType codeGenFileType;
    switch (outputKind) {
        case OutputKind::ASSEMBLY:
            fileName = string(module->getName()) + ".asm";
            codeGenFileType = llvm::CodeGenFileType::AssemblyFile;
            break;
        case OutputKind::OBJECT:
            fileName = string(module->getName()) + ".o";
            codeGenFileType = llvm::CodeGenFileType::ObjectFile;
            break;
    }

    error_code errorCode;
    llvm::raw_fd_ostream outputFile(fileName, errorCode, llvm::sys::fs::OF_None);
    if (errorCode) {
        cerr << errorCode.message();
        exit(1);
    }

    llvm::legacy::PassManager passManager;
    if (targetMachine->addPassesToEmitFile(passManager, outputFile, nullptr, codeGenFileType)) {
        cerr << "Failed to generate file " << fileName << endl;
        exit(1);
    }

    passManager.run(*module);
    outputFile.flush();
}