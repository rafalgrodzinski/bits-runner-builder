#include "CodeGenerator.h"

using namespace std;

CodeGenerator::CodeGenerator(shared_ptr<llvm::Module> module): module(module) {
}

void CodeGenerator::generateObjectFile(OutputKind outputKind, OptimizationLevel optLevel, string targetTripleOption, string architectureOption, bool isVerbose, unsigned int optionBits) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    string targetTriple = llvm::sys::getDefaultTargetTriple();
    if (!targetTripleOption.empty())
        targetTriple = targetTripleOption;
    string errorString;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, errorString);
    if (!target) {
        cerr << errorString << endl;
        exit(1);
    }
    string architecture = "generic";
    if (!architectureOption.empty())
        architecture = architectureOption;
    string features = "";
    llvm::Reloc::Model relocationModel = llvm::Reloc::PIC_;
    llvm::CodeModel::Model codeModel = llvm::CodeModel::Model::Small;
    llvm::CodeGenOptLevel optimizationLevel;
    switch (optLevel) {
        case OptimizationLevel::O0:
            optimizationLevel = llvm::CodeGenOptLevel::None;
            break;
        case OptimizationLevel::O1:
            optimizationLevel = llvm::CodeGenOptLevel::Less;
            break;
        case OptimizationLevel::O2:
            optimizationLevel = llvm::CodeGenOptLevel::Default;
            break;
        case OptimizationLevel::O3:
            optimizationLevel = llvm::CodeGenOptLevel::Aggressive;
            break;
    }

    llvm::TargetOptions targetOptions;
    targetOptions.FunctionSections = optionBits & 1 << 0;
    llvm::TargetMachine *targetMachine = target->createTargetMachine(
        targetTriple,
        architecture,
        features,
        targetOptions,
        relocationModel,
        codeModel,
        optimizationLevel
    );

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

    if (isVerbose) {
        cout << format("🐉 Generating code for module \"{}\" targeting {}, {}...\n", string(module->getName()), targetTriple, architecture);
    }

    passManager.run(*module);
    outputFile.flush();
}