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
    enum class OutputKind {
        ASSEMBLY,
        OBJECT
    };

    enum class OptimizationLevel {
        O0,
        O1,
        O2,
        O3
    };

    enum class RelocationModel {
        STATIC,
        PIC
    };

    enum class Options {
        FUNCTION_SECTIONS,
        NO_BSS
    };

private:
    string targetTriple;
    string architecture;
    llvm::TargetMachine *targetMachine;
    llvm::DataLayout dataLayout;

public:
    CodeGenerator(OptimizationLevel optLevel, RelocationModel relocationModelOption, string targetTripleOption, string architectureOption, unsigned int optionBits);
    void generateObjectFile(shared_ptr<llvm::Module> module, OutputKind outputKind, bool isVerbose);
    int getIntSize();
    int getPointerSize();
};

#endif