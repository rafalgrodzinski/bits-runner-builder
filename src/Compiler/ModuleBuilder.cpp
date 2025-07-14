#include "ModuleBuilder.h"

#include "Error.h"
#include "Logger.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionArrayLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"

ModuleBuilder::ModuleBuilder(string moduleName, string sourceFileName, vector<shared_ptr<Statement>> statements):
moduleName(moduleName), sourceFileName(sourceFileName), statements(statements) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>(moduleName, *context);
    module->setSourceFileName(sourceFileName);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    typeVoid = llvm::Type::getVoidTy(*context);
    typeBool = llvm::Type::getInt1Ty(*context);
    typeSint32 = llvm::Type::getInt32Ty(*context);
    typeReal32 = llvm::Type::getFloatTy(*context);
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    scopes.push(Scope());
    for (shared_ptr<Statement> &statement : statements)
        buildStatement(statement);

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

    return module;
}

void ModuleBuilder::buildStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::FUNCTION:
            buildFunctionDeclaration(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::RAW_FUNCTION:
            buildRawFunction(dynamic_pointer_cast<StatementRawFunction>(statement));
            break;
        case StatementKind::VARIABLE:
            buildVarDeclaration(dynamic_pointer_cast<StatementVariable>(statement));
            break;
        case StatementKind::ASSIGNMENT:
            buildAssignment(dynamic_pointer_cast<StatementAssignment>(statement));
            break;
        case StatementKind::BLOCK:
            buildBlock(dynamic_pointer_cast<StatementBlock>(statement));
            break;
        case StatementKind::RETURN:
            buildReturn(dynamic_pointer_cast<StatementReturn>(statement));
            break;
        case StatementKind::REPEAT:
            buildLoop(dynamic_pointer_cast<StatementRepeat>(statement));
            break;
        case StatementKind::META_EXTERN_FUNCTION:
            buildMetaExternFunction(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
            break;
        case StatementKind::EXPRESSION:
            buildExpression(dynamic_pointer_cast<StatementExpression>(statement));
            return;
        default:
            markError(0, 0, "Unexpected statement");
    }
}

void ModuleBuilder::buildFunctionDeclaration(shared_ptr<StatementFunction> statement) {
    // get argument types
    vector<llvm::Type *> types;
    for (pair<string, shared_ptr<ValueType>> &arg : statement->getArguments()) {
        types.push_back(typeForValueType(arg.second));
    }

    // build function declaration
    llvm::FunctionType *funType = llvm::FunctionType::get(typeForValueType(statement->getReturnValueType()), types, false);
    llvm::Function *fun = llvm::Function::Create(funType, llvm::GlobalValue::ExternalLinkage, statement->getName(), module.get());
    if (!setFun(statement->getName(), fun))
        return;

    // define function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statement->getName(), fun);
    builder->SetInsertPoint(block);

    scopes.push(Scope());

    // build arguments
    int i=0;
    for (auto &arg : fun->args()) {
        string name = statement->getArguments()[i].first;
        llvm::Type *type = types[i];
        arg.setName(name);

        llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr, name);
        if (!setAlloca(name, alloca))
            return;
        builder->CreateStore(&arg, alloca);

        i++;
    }

    // build function body
    buildStatement(statement->getStatementBlock());

    scopes.pop();

    // verify
    string errorMessage;
    llvm::raw_string_ostream llvmErrorMessage(errorMessage);
    if (llvm::verifyFunction(*fun, &llvmErrorMessage))
        markError(0, 0, errorMessage);
}

void ModuleBuilder::buildRawFunction(shared_ptr<StatementRawFunction> statement) {
    // get argument types
    vector<llvm::Type *> types;

    llvm::FunctionType *funType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), types, false);
    llvm::InlineAsm *rawFun = llvm::InlineAsm::get(funType, statement->getRawSource(), statement->getConstraints(), false, false, llvm::InlineAsm::AsmDialect::AD_Intel);
    if (!setRawFun(statement->getName(), rawFun))
        return;

    /*int res;
    int a = 42;
    int b = 13;

    vector<llvm::Type *> types;
    types.push_back(typeSint32);
    types.push_back(typeSint32);
    llvm::FunctionType *asmType = llvm::FunctionType::get(typeSint32, types, false);
    llvm::InlineAsm *asmm = llvm::InlineAsm::get(asmType, "add $0, $1", "+{ebx},i", false, false, llvm::InlineAsm::AsmDialect::AD_Intel);

    vector<llvm::Value *>argValues;
    argValues.push_back(llvm::ConstantInt::get(typeSint32, 5, true));
    argValues.push_back(llvm::ConstantInt::get(typeSint32, 4, true));

    llvm::Value *valu = builder->CreateCall(asmm, llvm::ArrayRef(argValues));*/
}

void ModuleBuilder::buildVarDeclaration(shared_ptr<StatementVariable> statement) {
    if (statement->getValueType()->getKind() == ValueTypeKind::DATA) {
        vector<llvm::Value *> values = valuesForExpression(statement->getExpression());

        llvm::ArrayType *type = (llvm::ArrayType *)typeForValueType(statement->getValueType(), values.size());
        llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr, statement->getName());
        if (!setAlloca(statement->getName(), alloca))
            return;
        for (int i=0; i < type->getNumElements(); i++) {
            llvm::Value *index[] = {
                builder->getInt32(0),
                builder->getInt32(i)
            };
            llvm::Value *elementPtr = builder->CreateGEP(type, alloca, index, format("{}_{}", statement->getName(), i));

            builder->CreateStore(values[i], elementPtr);
        }
    } else {
        llvm::Value *value = valueForExpression(statement->getExpression());
        llvm::AllocaInst *alloca = builder->CreateAlloca(typeForValueType(statement->getValueType(), 0), nullptr, statement->getName());

        if (!setAlloca(statement->getName(), alloca))
            return;
        builder->CreateStore(value, alloca);
    }
}

void ModuleBuilder::buildAssignment(shared_ptr<StatementAssignment> statement) {
    llvm::AllocaInst *alloca = getAlloca(statement->getName());
    if (alloca == nullptr)
        return;

    llvm::Value *value = valueForExpression(statement->getExpression());

    if (statement->getIndexExpression()) {
        llvm::Value *indexValue = valueForExpression(statement->getIndexExpression());
        llvm::Value *index[] = {
            builder->getInt32(0),
            indexValue
        };
        llvm::ArrayType *type = (llvm::ArrayType *)alloca->getAllocatedType();
        llvm::Value *elementPtr = builder->CreateGEP(type, alloca, index, format("{}[]", statement->getName()));

        builder->CreateStore(value, elementPtr);
    } else {
        builder->CreateStore(value, alloca);
    }
}

void ModuleBuilder::buildBlock(shared_ptr<StatementBlock> statement) {
    for (shared_ptr<Statement> &innerStatement : statement->getStatements())
        buildStatement(innerStatement);
}

void ModuleBuilder::buildReturn(shared_ptr<StatementReturn> statement) {
    if (statement->getExpression() != nullptr) {
        llvm::Value *value = valueForExpression(statement->getExpression());
        builder->CreateRet(value);
    } else {
        builder->CreateRetVoid();
    }
}

void ModuleBuilder::buildLoop(shared_ptr<StatementRepeat> statement) {
    shared_ptr<Statement> initStatement = statement->getInitStatement();
    shared_ptr<StatementBlock> bodyStatement= statement->getBodyBlockStatement();
    shared_ptr<Expression> preExpression = statement->getPreConditionExpression();
    shared_ptr<Expression> postExpression = statement->getPostConditionExpression();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *preBlock = llvm::BasicBlock::Create(*context, "loopPre", fun);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(*context, "loopBody");
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(*context, "loopPost");

    scopes.push(Scope());

    // loop init
    if (initStatement != nullptr)
        buildStatement(statement->getInitStatement());
    builder->CreateBr(preBlock);

    // pre condition
    builder->SetInsertPoint(preBlock);
    if (preExpression != nullptr) {
        llvm::Value *preConditionValue = valueForExpression(preExpression);
        builder->CreateCondBr(preConditionValue, bodyBlock, afterBlock);
    } else {
        builder->CreateBr(bodyBlock);
    }

    // body
    fun->insert(fun->end(), bodyBlock);
    builder->SetInsertPoint(bodyBlock);
    buildBlock(bodyStatement);

    // post condition
    if (postExpression != nullptr) {
        llvm::Value *postConditionValue = valueForExpression(postExpression);
        builder->CreateCondBr(postConditionValue, preBlock, afterBlock);
    } else {
        builder->CreateBr(preBlock);
    }

    // loop post
    fun->insert(fun->end(), afterBlock);
    builder->SetInsertPoint(afterBlock);

    scopes.pop();
}

void ModuleBuilder::buildMetaExternFunction(shared_ptr<StatementMetaExternFunction> statement) {
    // get argument types
    vector<llvm::Type *> types;
    for (pair<string, shared_ptr<ValueType>> &arg : statement->getArguments()) {
        types.push_back(typeForValueType(arg.second));
    }

    // build function declaration
    llvm::FunctionType *funType = llvm::FunctionType::get(typeForValueType(statement->getReturnValueType()), types, false);
    llvm::Function *fun = llvm::Function::Create(funType, llvm::GlobalValue::ExternalLinkage, statement->getName(), module.get());
    if (!setFun(statement->getName(), fun))
        return;
    
    // build arguments
    int i=0;
    for (auto &arg : fun->args()) {
        string name = statement->getArguments()[i].first;
        arg.setName(name);
        i++;
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
        case ExpressionKind::VAR:
            return valueForVar(dynamic_pointer_cast<ExpressionVariable>(expression));
        case ExpressionKind::CALL:
            return valueForCall(dynamic_pointer_cast<ExpressionCall>(expression));
        default:
            markError(0, 0, "Unexpected expression");
            return nullptr;
    }
}

vector<llvm::Value*> ModuleBuilder::valuesForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::ARRAY_LITERAL:
            return valuesForArrayLiteral(dynamic_pointer_cast<ExpressionArrayLiteral>(expression));
        default:
            markError(0, 0, "Unexpected expression");
            return vector<llvm::Value*>();
    }
}

llvm::Value *ModuleBuilder::valueForLiteral(shared_ptr<ExpressionLiteral> expression) {
    if (expression->getValueType() == nullptr)
        return llvm::UndefValue::get(typeVoid);

    switch (expression->getValueType()->getKind()) {
        case ValueTypeKind::NONE:
            return llvm::UndefValue::get(typeVoid);
        case ValueTypeKind::BOOL:
            return llvm::ConstantInt::get(typeBool, expression->getBoolValue(), true);
        case ValueTypeKind::SINT32:
            return llvm::ConstantInt::get(typeSint32, expression->getSint32Value(), true);
        case ValueTypeKind::REAL32:
            return llvm::ConstantInt::get(typeReal32, expression->getReal32Value(), true);
    }
}

vector<llvm::Value*> ModuleBuilder::valuesForArrayLiteral(shared_ptr<ExpressionArrayLiteral> expression) {
    vector<llvm::Value*> values;
    for (shared_ptr<Expression> &expression : expression->getExpressions()) {
        values.push_back(valueForExpression(expression));
    }
    return values;
}

llvm::Value *ModuleBuilder::valueForGrouping(shared_ptr<ExpressionGrouping> expression) {
    return valueForExpression(expression->getExpression());
}

llvm::Value *ModuleBuilder::valueForBinary(shared_ptr<ExpressionBinary> expression) {
    llvm::Value *leftValue = valueForExpression(expression->getLeft());
    llvm::Value *rightValue = valueForExpression(expression->getRight());

    if (leftValue == nullptr || rightValue == nullptr)
        return nullptr;

    llvm::Type *type = leftValue->getType();

    if (type == typeBool) {
        return valueForBinaryBool(expression->getOperation(), leftValue, rightValue);
    } else if (type == typeSint32 || type == typeVoid) {
        return valueForBinaryInteger(expression->getOperation(), leftValue, rightValue);
    } else if (type == typeReal32) {
        return valueForBinaryReal(expression->getOperation(), leftValue, rightValue);
    }

    markError(0, 0, "Unexpected operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForBinaryBool(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
    switch (operation) {
    case ExpressionBinaryOperation::EQUAL:
        return builder->CreateICmpEQ(leftValue, rightValue);
    case ExpressionBinaryOperation::NOT_EQUAL:
        return builder->CreateICmpNE(leftValue, rightValue);
    default:
        markError(0, 0, "Unexpecgted operation for boolean operands");
        return nullptr;
    }
}

llvm::Value *ModuleBuilder::valueForBinaryInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
    switch (operation) {
    case ExpressionBinaryOperation::EQUAL:
        return builder->CreateICmpEQ(leftValue, rightValue);
    case ExpressionBinaryOperation::NOT_EQUAL:
        return builder->CreateICmpNE(leftValue, rightValue);
    case ExpressionBinaryOperation::LESS:
        return builder->CreateICmpSLT(leftValue, rightValue);
    case ExpressionBinaryOperation::LESS_EQUAL:
        return builder->CreateICmpSLE(leftValue, rightValue);
    case ExpressionBinaryOperation::GREATER:
        return builder->CreateICmpSGT(leftValue, rightValue);
    case ExpressionBinaryOperation::GREATER_EQUAL:
        return builder->CreateICmpSGE(leftValue, rightValue);
    case ExpressionBinaryOperation::ADD:
        return builder->CreateNSWAdd(leftValue, rightValue);
    case ExpressionBinaryOperation::SUB:
        return builder->CreateNSWSub(leftValue, rightValue);
    case ExpressionBinaryOperation::MUL:
        return builder->CreateNSWMul(leftValue, rightValue);
    case ExpressionBinaryOperation::DIV:
        return builder->CreateSDiv(leftValue, rightValue);
    case ExpressionBinaryOperation::MOD:
        return builder->CreateSRem(leftValue, rightValue);
    }
}

llvm::Value *ModuleBuilder::valueForBinaryReal(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
    switch (operation) {
    case ExpressionBinaryOperation::EQUAL:
        return builder->CreateFCmpOEQ(leftValue, rightValue);
    case ExpressionBinaryOperation::NOT_EQUAL:
        return builder->CreateFCmpONE(leftValue, rightValue);
    case ExpressionBinaryOperation::LESS:
        return builder->CreateFCmpOLT(leftValue, rightValue);
    case ExpressionBinaryOperation::LESS_EQUAL:
        return builder->CreateFCmpOLE(leftValue, rightValue);
    case ExpressionBinaryOperation::GREATER:
        return builder->CreateFCmpOGT(leftValue, rightValue);
    case ExpressionBinaryOperation::GREATER_EQUAL:
        return builder->CreateFCmpOGE(leftValue, rightValue);
    case ExpressionBinaryOperation::ADD:
        return builder->CreateNSWAdd(leftValue, rightValue);
    case ExpressionBinaryOperation::SUB:
        return builder->CreateNSWSub(leftValue, rightValue);
    case ExpressionBinaryOperation::MUL:
        return builder->CreateNSWMul(leftValue, rightValue);
    case ExpressionBinaryOperation::DIV:
        return builder->CreateSDiv(leftValue, rightValue);
    case ExpressionBinaryOperation::MOD:
        return builder->CreateSRem(leftValue, rightValue);
    }
}

llvm::Value *ModuleBuilder::valueForIfElse(shared_ptr<ExpressionIfElse> expression) {
    shared_ptr<Expression> conditionExpression = expression->getCondition();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::Value *conditionValue = valueForExpression(conditionExpression);

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(*context, "thenBlock", fun);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(*context, "elseBlock");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(*context, "mergeBlock");

    int valuesCount = 1;
    builder->CreateCondBr(conditionValue, thenBlock, elseBlock);

    // Then
    scopes.push(Scope());
    builder->SetInsertPoint(thenBlock);
    buildStatement(expression->getThenBlock()->getStatementBlock());
    llvm::Value *thenValue = valueForExpression(expression->getThenBlock()->getResultStatementExpression()->getExpression());
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();
    scopes.pop();

    // Else
    scopes.push(Scope());
    fun->insert(fun->end(), elseBlock);
    builder->SetInsertPoint(elseBlock);
    llvm::Value *elseValue = nullptr;
    if (expression->getElseBlock() != nullptr) {
        valuesCount++;
        buildStatement(expression->getElseBlock()->getStatementBlock());
        elseValue = valueForExpression(expression->getElseBlock()->getResultStatementExpression()->getExpression());
    }
    builder->CreateBr(mergeBlock);
    elseBlock = builder->GetInsertBlock();
    scopes.pop();

    // Merge
    fun->insert(fun->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    if (elseValue != nullptr && thenValue->getType() != elseValue->getType()) {
        return llvm::UndefValue::get(typeVoid);
    } else {
        llvm::PHINode *phi = builder->CreatePHI(thenValue->getType(), valuesCount, "ifElseResult");
        phi->addIncoming(thenValue, thenBlock);
        if (elseValue != nullptr)
            phi->addIncoming(elseValue, elseBlock);

        return phi;
    }
}

llvm::Value *ModuleBuilder::valueForVar(shared_ptr<ExpressionVariable> expression) {
    llvm::AllocaInst *alloca = getAlloca(expression->getName());
    if (alloca == nullptr)
        return nullptr;

    if (expression->getIndexExpression()) {
        llvm::Value *indexValue = valueForExpression(expression->getIndexExpression());
        llvm::Value *index[] = {
            builder->getInt32(0),
            indexValue
        };
        llvm::ArrayType *type = (llvm::ArrayType *)alloca->getAllocatedType();
        llvm::Value *elementPtr = builder->CreateGEP(type, alloca, index, format("{}[]", expression->getName()));

        return builder->CreateLoad(type->getArrayElementType(), elementPtr);
    } else {
        return builder->CreateLoad(alloca->getAllocatedType(), alloca, expression->getName());
    }
}

llvm::Value *ModuleBuilder::valueForCall(shared_ptr<ExpressionCall> expression) {
    llvm::Function *fun = getFun(expression->getName());
    if (fun != nullptr) {
        llvm::FunctionType *funType = fun->getFunctionType();
        vector<llvm::Value*> argValues;
        for (shared_ptr<Expression> &argumentExpression : expression->getArgumentExpressions()) {
            llvm::Value *argValue = valueForExpression(argumentExpression);
            argValues.push_back(argValue);
        }
        return builder->CreateCall(funType, fun, llvm::ArrayRef(argValues));
    }

    llvm::InlineAsm *rawFun = getRawFun(expression->getName());
    if (rawFun != nullptr) {
        vector<llvm::Value *>argValues;
        return builder->CreateCall(rawFun, llvm::ArrayRef(argValues));
    }

    markError(0, 0, format("Function \"{}\" not defined in scope", expression->getName()));
    return nullptr;
}

bool ModuleBuilder::setAlloca(string name, llvm::AllocaInst *alloca) {
    if (scopes.top().allocaMap[name] != nullptr) {
        markError(0, 0, format("Variable \"{}\" already defined", name));
        return false;
    }

    scopes.top().allocaMap[name] = alloca;
    return true;
}

llvm::AllocaInst* ModuleBuilder::getAlloca(string name) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        llvm::AllocaInst *alloca = scopes.top().allocaMap[name];
        if (alloca != nullptr)
            return alloca;
        scopes.pop();
    }

    markError(0, 0, format("Variable \"{}\" not defined in scope", name));
    return nullptr;
}

bool ModuleBuilder::setFun(string name, llvm::Function *fun) {
    if (scopes.top().funMap[name] != nullptr) {
        markError(0, 0, format("Function \"{}\" already defined in scope", name));
        return false;
    }

    scopes.top().funMap[name] = fun;
    return true;
}

llvm::Function* ModuleBuilder::getFun(string name) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        llvm::Function *fun = scopes.top().funMap[name];
        if (fun != nullptr)
            return fun;
        scopes.pop();
    }

    return nullptr;
}

bool ModuleBuilder::setRawFun(string name, llvm::InlineAsm *rawFun) {
    if (scopes.top().rawFunMap[name] != nullptr) {
        markError(0, 0, format("Raw function \"{}\" already defined in scope", name));
        return false;
    }
    
    scopes.top().rawFunMap[name] = rawFun;
    return true;
}

llvm::InlineAsm *ModuleBuilder::getRawFun(string name) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        llvm::InlineAsm *rawFun = scopes.top().rawFunMap[name];
        if (rawFun != nullptr)
            return rawFun;
        scopes.pop();
    }

    return nullptr;
}

llvm::Type *ModuleBuilder::typeForValueType(shared_ptr<ValueType> valueType, int count) {
    switch (valueType->getKind()) {
        case ValueTypeKind::NONE:
            return typeVoid;
        case ValueTypeKind::BOOL:
            return typeBool;
        case ValueTypeKind::SINT32:
            return typeSint32;
        case ValueTypeKind::REAL32:
            return typeReal32;
        case ValueTypeKind::DATA: {
            if (valueType->getSubType() == nullptr)
                return nullptr;
            if (valueType->getValueArg() > 0)
                count = valueType->getValueArg();
            return llvm::ArrayType::get(typeForValueType(valueType->getSubType(), count), count);
            return nullptr;
        }
    }
}

void ModuleBuilder::markError(int line, int column, string message) {
    errors.push_back(Error::builderError(line, column, message));
}
