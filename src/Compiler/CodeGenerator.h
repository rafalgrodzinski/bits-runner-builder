#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <iostream>

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
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

    enum class Options {
        FUNCTION_SECTIONS,
        NO_BSS
    };


    enum class OutputKind {
        ASSEMBLY,
        OBJECT
    };

private:
    string targetTriple;
    string architecture;
    llvm::TargetMachine *targetMachine;
    llvm::DataLayout dataLayout;

public:
    CodeGenerator(
        string targetTripleOption,
        string architectureOption,
        RelocationModel relocationModelOption,
        CodeModel codeModelOption,
        OptimizationLevel optimizationLevelOption,
        unsigned int optionBits
    );
    void generateObjectFile(shared_ptr<llvm::Module> module, OutputKind outputKind, bool isVerbose);
    int getIntSize();
    int getPointerSize();
};

#endif