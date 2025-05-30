#ifndef COMPILER_H
#define COMPILER_H

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "Expression.h"

using namespace std;

class Compiler {
private:
    shared_ptr<llvm::LLVMContext> context;
    //llvm::LLVMContext context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    //shared_ptr<llvm::IntegerType> int32Type;
    llvm::IntegerType *int32Type;

    shared_ptr<Expression> expression;
    llvm::Value *valueForExpression(shared_ptr<Expression> expression);

public:
    Compiler(shared_ptr<Expression> expression);
    shared_ptr<llvm::Module> getModule();
};

#endif