#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "Expression.h"
#include "Statement.h"

using namespace std;

class ModuleBuilder {
private:
    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *voidType;
    llvm::IntegerType *int32Type;

    vector<shared_ptr<Statement>> statements;

    void buildCodeForStatement(shared_ptr<Statement> statement);
    void buildFunction(shared_ptr<Statement> statement);
    void buildBlock(shared_ptr<Statement> statement);
    void buildReturn(shared_ptr<Statement> statement);
    void buildExpression(shared_ptr<Statement> statement);
    llvm::Value *valueForExpression(shared_ptr<Expression> expression);

public:
    ModuleBuilder(vector<shared_ptr<Statement>> statements);
    shared_ptr<llvm::Module> getModule();
};

#endif