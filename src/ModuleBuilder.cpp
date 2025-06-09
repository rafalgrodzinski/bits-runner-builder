#include "ModuleBuilder.h"

ModuleBuilder::ModuleBuilder(vector<shared_ptr<Statement>> statements): statements(statements) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>("dummy", *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    typeVoid = llvm::Type::getVoidTy(*context);
    typeBool = llvm::Type::getInt1Ty(*context);
    typeSInt32 = llvm::Type::getInt32Ty(*context);
    typeReal32 = llvm::Type::getFloatTy(*context);
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    for (shared_ptr<Statement> &statement : statements) {
        buildStatement(statement);
    }
    return module;
}

void ModuleBuilder::buildStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::FUNCTION_DECLARATION:
            buildFunctionDeclaration(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            break;
        case StatementKind::VAR_DECLARATION:
            buildVarDeclaration(dynamic_pointer_cast<StatementVarDeclaration>(statement));
            break;
        case StatementKind::BLOCK:
            buildBlock(dynamic_pointer_cast<StatementBlock>(statement));
            break;
        case StatementKind::RETURN:
            buildReturn(dynamic_pointer_cast<StatementReturn>(statement));
            break;
        case StatementKind::EXPRESSION:
            buildExpression(dynamic_pointer_cast<StatementExpression>(statement));
            return;
        default:
            exit(1);
    }
}

void ModuleBuilder::buildFunctionDeclaration(shared_ptr<StatementFunctionDeclaration> statement) {
    llvm::FunctionType *funType = llvm::FunctionType::get(typeForValueType(statement->getReturnValueType()), false);
    llvm::Function *fun = llvm::Function::Create(funType, llvm::GlobalValue::InternalLinkage, statement->getName(), module.get());
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statement->getName(), fun);
    builder->SetInsertPoint(block);
    buildStatement(statement->getStatementBlock());
}

void ModuleBuilder::buildVarDeclaration(shared_ptr<StatementVarDeclaration> statement) {
    llvm::Value *value = valueForExpression(statement->getExpression());
    llvm::AllocaInst *alloca = builder->CreateAlloca(typeForValueType(statement->getValueType()), nullptr, statement->getName());
    builder->CreateStore(value, alloca);
}

void ModuleBuilder::buildBlock(shared_ptr<StatementBlock> statement) {
    for (shared_ptr<Statement> &innerStatement : statement->getStatements()) {
        buildStatement(innerStatement);
    }
    if (statement->getStatementExpression() != nullptr)
        buildStatement(statement->getStatementExpression());
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
        case ExpressionKind::LITERAL:
            return valueForLiteral(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::GROUPING:
            return valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getExpression());
        case ExpressionKind::BINARY:
            return valueForBinary(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::IF_ELSE:
            return valueForIfElse(dynamic_pointer_cast<ExpressionIfElse>(expression));
        default:
            exit(1);
    }
}

llvm::Value *ModuleBuilder::valueForLiteral(shared_ptr<ExpressionLiteral> expression) {
    switch (expression->getValueType()) {
        case ValueType::VOID:
            return llvm::UndefValue::get(typeVoid);
        case ValueType::BOOL:
            return llvm::ConstantInt::get(typeBool, expression->getBoolValue(), true);
        case ValueType::SINT32:
            return llvm::ConstantInt::get(typeSInt32, expression->getSInt32Value(), true);
        case ValueType::REAL32:
            return llvm::ConstantInt::get(typeReal32, expression->getReal32Value(), true);
    }
}

llvm::Value *ModuleBuilder::valueForGrouping(shared_ptr<ExpressionGrouping> expression) {
    return valueForExpression(expression->getExpression());
}

llvm::Value *ModuleBuilder::valueForBinary(shared_ptr<ExpressionBinary> expression) {
    switch (expression->getLeft()->getValueType()) {
        case ValueType::BOOL:
            return valueForBinaryBool(expression);
        case ValueType::SINT32:
            return valueForBinaryInteger(expression);
        case ValueType::REAL32:
            return valueForBinaryReal(expression);
        default:
            exit(1);
    }
}

llvm::Value *ModuleBuilder::valueForBinaryBool(shared_ptr<ExpressionBinary> expression) {
    llvm::Value *leftValue = valueForExpression(expression->getLeft());
    llvm::Value *rightValue = valueForExpression(expression->getRight());

    switch (expression->getOperation()) {
    case ExpressionBinary::Operation::EQUAL:
        return builder->CreateICmpEQ(leftValue, rightValue);
    case ExpressionBinary::Operation::NOT_EQUAL:
        return builder->CreateICmpNE(leftValue, rightValue);
    default:
        exit(1);
    }
}

llvm::Value *ModuleBuilder::valueForBinaryInteger(shared_ptr<ExpressionBinary> expression) {
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

llvm::Value *ModuleBuilder::valueForBinaryReal(shared_ptr<ExpressionBinary> expression) {
    llvm::Value *leftValue = valueForExpression(expression->getLeft());
    llvm::Value *rightValue = valueForExpression(expression->getRight());

    switch (expression->getOperation()) {
    case ExpressionBinary::Operation::EQUAL:
        return builder->CreateFCmpOEQ(leftValue, rightValue);
    case ExpressionBinary::Operation::NOT_EQUAL:
        return builder->CreateFCmpONE(leftValue, rightValue);
    case ExpressionBinary::Operation::LESS:
        return builder->CreateFCmpOLT(leftValue, rightValue);
    case ExpressionBinary::Operation::LESS_EQUAL:
        return builder->CreateFCmpOLE(leftValue, rightValue);
    case ExpressionBinary::Operation::GREATER:
        return builder->CreateFCmpOGT(leftValue, rightValue);
    case ExpressionBinary::Operation::GREATER_EQUAL:
        return builder->CreateFCmpOGE(leftValue, rightValue);
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

llvm::Value *ModuleBuilder::valueForIfElse(shared_ptr<ExpressionIfElse> expression) {
    shared_ptr<Expression> conditionExpression = expression->getCondition();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    //llvm::Value *conditionValue = valueForBinary(conditionExpression);
    llvm::Value *conditionValue = valueForExpression(conditionExpression);

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(*context, "thenBlock", fun);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(*context, "elseBlock");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(*context, "mergeBlock");

    int valuesCount = 1;
    builder->CreateCondBr(conditionValue, thenBlock, elseBlock);

    // Then
    builder->SetInsertPoint(thenBlock);
    llvm::Value *thenValue = valueForExpression(expression->getThenBlock()->getStatementExpression()->getExpression());
    buildStatement(expression->getThenBlock());
    //llvm::Value *thenValue = llvm::ConstantInt::get(typeSInt32, 11, true);
    //buildStatement(expression->getThenBlock());
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();

    // Else
    fun->insert(fun->end(), elseBlock);
    //llvm::Value *elseValue = llvm::ConstantInt::get(typeSInt32, 22, true);
    builder->SetInsertPoint(elseBlock);
    llvm::Value *elseValue = nullptr;
    if (expression->getElseBlock() != nullptr) {
        valuesCount++;
        elseValue = valueForExpression(expression->getElseBlock()->getStatementExpression()->getExpression());
        buildStatement(expression->getElseBlock());
    }
    builder->CreateBr(mergeBlock);
    elseBlock = builder->GetInsertBlock();

    // Merge
    fun->insert(fun->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);
    llvm::PHINode *phi = builder->CreatePHI(typeForValueType(expression->getValueType()), valuesCount, "phii");
    phi->addIncoming(thenValue, thenBlock);
    if (elseValue != nullptr)
        phi->addIncoming(elseValue, elseBlock);

    //return llvm::ConstantInt::get(int32Type, 42, true);
    return phi;
}

llvm::Type *ModuleBuilder::typeForValueType(ValueType valueType) {
    switch (valueType) {
        case ValueType::VOID:
            return typeVoid;
        case ValueType::BOOL:
            return typeBool;
        case ValueType::SINT32:
            return typeSInt32;
        case ValueType::REAL32:
            return typeReal32;
    }
}
