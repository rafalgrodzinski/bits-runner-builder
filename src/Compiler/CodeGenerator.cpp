#include "CodeGenerator.h"

using namespace std;

CodeGenerator::CodeGenerator(
    string targetTripleOption,
    string architectureOption,
    RelocationModel relocationModelOption,
    CodeModel codeModelOption,
    OptimizationLevel optimizationLevelOption,
    CallingConvention callingConventionOption,
    unsigned int optionBits
) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    // target triple
    targetTriple = llvm::sys::getDefaultTargetTriple();
    if (!targetTripleOption.empty())
        targetTriple = targetTripleOption;
    string errorString;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, errorString);
    if (!target) {
        cerr << errorString << endl;
        exit(1);
    }

    // architecture
    architecture = "generic";
    if (!architectureOption.empty())
        architecture = architectureOption;
    string features = "";

    // relocation model
    llvm::Reloc::Model relocationModel;
    switch (relocationModelOption) {
        case RelocationModel::STATIC:
            relocationModel = llvm::Reloc::Static;
            break;
        case RelocationModel::PIC:
            relocationModel = llvm::Reloc::PIC_;
            break;
    }

    // code model
    llvm::CodeModel::Model codeModel = llvm::CodeModel::Model::Small;
    switch (codeModelOption) {
        case CodeModel::TINY:
            codeModel = llvm::CodeModel::Model::Tiny;
            break;
        case CodeModel::SMALL:
            codeModel = llvm::CodeModel::Model::Small;
            break;
        case CodeModel::KERNEL:
            codeModel = llvm::CodeModel::Model::Kernel;
            break;
        case CodeModel::MEDIUM:
            codeModel = llvm::CodeModel::Model::Medium;
            break;
        case CodeModel::LARGE:
            codeModel = llvm::CodeModel::Model::Large;
            break;
    }

    // optimization level
    llvm::CodeGenOptLevel optimizationLevel;
    switch (optimizationLevelOption) {
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

    // calling convention
    switch (callingConventionOption) {
        case CallingConvention::CDECL:
            callingConvention = llvm::CallingConv::C;
            break;
        case CallingConvention::STDCALL:
            callingConvention = llvm::CallingConv::X86_StdCall;
            break;
        case CallingConvention::FASTCALL:
            callingConvention = llvm::CallingConv::Fast;
            break;
        case CallingConvention::TAIL:
            callingConvention = llvm::CallingConv::Tail;
            break;
    }

    // options
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

llvm::CallingConv::ID CodeGenerator::getCallingConvetion() {
    return callingConvention;
}
