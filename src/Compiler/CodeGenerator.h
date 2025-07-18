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

private:
    shared_ptr<llvm::Module> module;

public:
    CodeGenerator(shared_ptr<llvm::Module> module);
    void generateObjectFile(OutputKind outputKind);
};

#endif