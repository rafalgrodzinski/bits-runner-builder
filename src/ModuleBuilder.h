#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "Expression.h"

using namespace std;

class ModuleBuilder {
private:
    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *voidType;
    llvm::IntegerType *int32Type;

    shared_ptr<Expression> expression;
    llvm::Value *valueForExpression(shared_ptr<Expression> expression);

public:
    ModuleBuilder(shared_ptr<Expression> expression);
    shared_ptr<llvm::Module> getModule();
};

#endif