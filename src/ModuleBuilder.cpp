#include "ModuleBuilder.h"

ModuleBuilder::ModuleBuilder(vector<shared_ptr<Statement>> statements): statements(statements) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>("dummy", *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    voidType = llvm::Type::getVoidTy(*context);
    int32Type = llvm::Type::getInt32Ty(*context);
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    for (shared_ptr<Statement> &statement : statements) {
        buildStatement(statement);
    }
    return module;
}

void ModuleBuilder::buildStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case Statement::Kind::FUNCTION_DECLARATION:
            buildFunctionDeclaration(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            break;
        case Statement::Kind::BLOCK:
            buildBlock(dynamic_pointer_cast<StatementBlock>(statement));
            break;
        case Statement::Kind::RETURN:
            buildReturn(dynamic_pointer_cast<StatementReturn>(statement));
            break;
        case Statement::Kind::EXPRESSION:
            buildExpression(dynamic_pointer_cast<StatementExpression>(statement));
            return;
        default:
            exit(1);
    }
}

void ModuleBuilder::buildFunctionDeclaration(shared_ptr<StatementFunctionDeclaration> statement) {
    llvm::FunctionType *funType = llvm::FunctionType::get(int32Type, false);
    llvm::Function *fun = llvm::Function::Create(funType, llvm::GlobalValue::InternalLinkage, statement->getName(), module.get());
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statement->getName(), fun);
    builder->SetInsertPoint(block);
    buildStatement(statement->getStatementBlock());
}

void ModuleBuilder::buildBlock(shared_ptr<StatementBlock> statement) {
    for (shared_ptr<Statement> &innerStatement : statement->getStatements()) {
        buildStatement(innerStatement);
    }
}

void ModuleBuilder::buildReturn(shared_ptr<StatementReturn> statement) {
    if (statement->getExpression() != nullptr) {
        llvm::Value *value = valueForExpression(statement->getExpression());
        builder->CreateRet(value);
    } else {
        builder->CreateRetVoid();
    }
}

void ModuleBuilder::buildExpression(shared_ptr<StatementExpression> statement) {
    valueForExpression(statement->getExpression());
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case Expression::Kind::LITERAL:
            return valueForLiteral(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case Expression::Kind::GROUPING:
            return valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getExpression());
        case Expression::Kind::BINARY:
            return valueForBinary(dynamic_pointer_cast<ExpressionBinary>(expression));
        default:
            exit(1);
    }
}

llvm::Value *ModuleBuilder::valueForLiteral(shared_ptr<ExpressionLiteral> expression) {
    return llvm::ConstantInt::get(int32Type, expression->getInteger(), true);
}

llvm::Value *ModuleBuilder::valueForGrouping(shared_ptr<ExpressionGrouping> expression) {
    return valueForExpression(expression->getExpression());
}

llvm::Value *ModuleBuilder::valueForBinary(shared_ptr<ExpressionBinary> expression) {
    llvm::Value *leftValue = valueForExpression(expression->getLeft());
    llvm::Value *rightValue = valueForExpression(expression->getRight());

    switch (expression->getOperation()) {
    case ExpressionBinary::Operation::EQUAL:
        return builder->CreateICmpEQ(leftValue, rightValue);
    case ExpressionBinary::Operation::NOT_EQUAL:
        return builder->CreateICmpNE(leftValue, rightValue);
    case ExpressionBinary::Operation::LESS:
        return builder->CreateICmpSLT(leftValue, rightValue);
    case ExpressionBinary::Operation::LESS_EQUAL:
        return builder->CreateICmpSLE(leftValue, rightValue);
    case ExpressionBinary::Operation::GREATER:
        return builder->CreateICmpSGT(leftValue, rightValue);
    case ExpressionBinary::Operation::GREATER_EQUAL:
        return builder->CreateICmpSGE(leftValue, rightValue);
    case ExpressionBinary::Operation::ADD:
        return builder->CreateNSWAdd(leftValue, rightValue);
    case ExpressionBinary::Operation::SUB:
        return builder->CreateNSWSub(leftValue, rightValue);
    case ExpressionBinary::Operation::MUL:
        return builder->CreateNSWMul(leftValue, rightValue);
    case ExpressionBinary::Operation::DIV:
        return builder->CreateSDiv(leftValue, rightValue);
    case ExpressionBinary::Operation::MOD:
        return builder->CreateSRem(leftValue, rightValue);
    }
}
