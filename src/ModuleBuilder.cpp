#include "ModuleBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

/*ModuleBuilder::ModuleBuilder(vector<shared_ptr<Statement>> statements): statements(statements) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>("dummy", *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    voidType = llvm::Type::getVoidTy(*context);
    int32Type = llvm::Type::getInt32Ty(*context);
}

void ModuleBuilder::buildCodeForStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case Statement::Kind::FUNCTION_DECLARATION:
            buildFunction(statement);
            break;
        case Statement::Kind::BLOCK:
            buildBlock(statement);
            break;
        case Statement::Kind::RETURN:
            buildReturn(statement);
            break;
        case Statement::Kind::EXPRESSION:
            buildExpression(statement);
            break;
        default:
            exit(1);
    }
}

void ModuleBuilder::buildFunction(shared_ptr<Statement> statement) {
    llvm::FunctionType *funType = llvm::FunctionType::get(int32Type, false);
    llvm::Function *fun = llvm::Function::Create(funType, llvm::GlobalValue::InternalLinkage, statement->getName(), module.get());
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statement->getName(), fun);
    builder->SetInsertPoint(block);
    buildCodeForStatement(statement->getBlockStatement());
}

void ModuleBuilder::buildBlock(shared_ptr<Statement> statement) {
    for (shared_ptr<Statement> &innerStatement : statement->getStatements()) {
        buildCodeForStatement(innerStatement);
    }
}

void ModuleBuilder::buildReturn(shared_ptr<Statement> statement) {
    if (statement->getExpression() != nullptr) {
        llvm::Value *value = valueForExpression(statement->getExpression());
        builder->CreateRet(value);
    } else {
        builder->CreateRetVoid();
    }
}

void ModuleBuilder::buildExpression(shared_ptr<Statement> statement) {

}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case Expression::Kind::LITERAL:
            return llvm::ConstantInt::get(int32Type, expression->getInteger(), true);
        case Expression::Kind::GROUPING:
            return valueForExpression(expression->getLeft());
        case Expression::Kind::BINARY:
            llvm::Value *leftValue = valueForExpression(expression->getLeft());
            llvm::Value *rightValue = valueForExpression(expression->getRight());
            switch (expression->getOperator()) {
                case Expression::Operator::ADD:
                    return builder->CreateNSWAdd(leftValue, rightValue);
                case Expression::Operator::SUB:
                    return builder->CreateNSWSub(leftValue, rightValue);
                case Expression::Operator::MUL:
                    return builder->CreateNSWMul(leftValue, rightValue);
                case Expression::Operator::DIV:
                    return builder->CreateSDiv(leftValue, rightValue);
                case Expression::Operator::MOD:
                    return builder->CreateSRem(leftValue, rightValue);
            }
            break;
    }
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    for (shared_ptr<Statement> &statement : statements) {
        buildCodeForStatement(statement);
    }
    return module;
}*/