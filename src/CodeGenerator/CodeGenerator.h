#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <iostream>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/MemCpyOptimizer.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>

using namespace std;

class CodeGenerator {
public:
    enum class RelocationModel {
        STATIC,
        PIC
    };

    enum class CodeModel {
        TINY,
        SMALL,
        KERNEL,
        MEDIUM,
        LARGE
    };

    enum class OptimizationLevel {
        O0,
        O1,
        O2,
        O3
    };

    enum class CallingConvention {
        CDECL,
        STDCALL,
        FASTCALL,
        TAIL
    };

    enum class Options {
        FUNCTION_SECTIONS,
        NO_BSS
    };


    enum class OutputKind {
        ASSEMBLY,
        OBJECT,
        IR
    };

private:
    string targetTriple;
    string architecture;
    llvm::TargetMachine *targetMachine;
    llvm::DataLayout dataLayout;
    llvm::CallingConv::ID callingConvention;

public:
    CodeGenerator(
        string targetTripleOption,
        string architectureOption,
        RelocationModel relocationModelOption,
        CodeModel codeModelOption,
        OptimizationLevel optimizationLevelOption,
        CallingConvention callingConventionOption,
        unsigned int optionBits
    );
    void generateObjectFile(shared_ptr<llvm::Module> module, OutputKind outputKind, bool isVerbose);
    int getIntSize();
    int getPointerSize();
    llvm::CallingConv::ID getCallingConvetion();
};

#endif