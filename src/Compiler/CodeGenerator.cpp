#include "CodeGenerator.h"

using namespace std;

CodeGenerator::CodeGenerator(OptimizationLevel optLevel, RelocationModel relocationModelOption, string targetTripleOption, string architectureOption, unsigned int optionBits) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    targetTriple = llvm::sys::getDefaultTargetTriple();
    if (!targetTripleOption.empty())
        targetTriple = targetTripleOption;
    string errorString;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, errorString);
    if (!target) {
        cerr << errorString << endl;
        exit(1);
    }
    architecture = "generic";
    if (!architectureOption.empty())
        architecture = architectureOption;
    string features = "";
    llvm::Reloc::Model relocationModel;
    switch (relocationModelOption) {
        case RelocationModel::STATIC:
            relocationModel = llvm::Reloc::Static;
            break;
        case RelocationModel::PIC:
            relocationModel = llvm::Reloc::PIC_;
            break;
    }
    llvm::CodeModel::Model codeModel = llvm::CodeModel::Model::Kernel;
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
    targetOptions.FunctionSections = (optionBits >> int(Options::FUNCTION_SECTIONS)) & 0x01;
    targetOptions.NoZerosInBSS = (optionBits >> int(Options::NO_BSS)) & 0x01;
    targetMachine = target->createTargetMachine(
        targetTriple,
        architecture,
        features,
        targetOptions,
        relocationModel,
        codeModel,
        optimizationLevel
    );

    dataLayout = targetMachine->createDataLayout();
}

void CodeGenerator::generateObjectFile(shared_ptr<llvm::Module> module, OutputKind outputKind, bool isVerbose) {
    module->setDataLayout(dataLayout);
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
        cout << format("🐉 Generating code for module \"{}\" targeting {}, {}...\n\n", string(module->getName()), targetTriple, architecture);
    }

    passManager.run(*module);
    outputFile.flush();
}

int CodeGenerator::getIntSize() {
    return dataLayout.getLargestLegalIntTypeSizeInBits();
}

int CodeGenerator::getPointerSize() {
    return dataLayout.getPointerSizeInBits();
}
