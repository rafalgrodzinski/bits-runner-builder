#include "ModuleBuilder.h"

#include "Error.h"
#include "Logger.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionArrayLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBlock.h"

#include "Parser/Statement/StatementImport.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"

ModuleBuilder::ModuleBuilder(string moduleName, string defaultModuleName, vector<shared_ptr<Statement>> statements, vector<shared_ptr<Statement>> headerStatements, map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap):
moduleName(moduleName), defaultModuleName(defaultModuleName), statements(statements), headerStatements(headerStatements), exportedHeaderStatementsMap(exportedHeaderStatementsMap) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>(moduleName, *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    typeVoid = llvm::Type::getVoidTy(*context);
    typeBool = llvm::Type::getInt1Ty(*context);
    typeU8 = llvm::Type::getInt8Ty(*context);
    typeU32 = llvm::Type::getInt32Ty(*context);
    typeU64 = llvm::Type::getInt64Ty(*context);
    typeS8 = llvm::Type::getInt8Ty(*context);
    typeS32 = llvm::Type::getInt32Ty(*context);
    typeS64 = llvm::Type::getInt64Ty(*context);
    typeR32 = llvm::Type::getFloatTy(*context);
    typePtr = llvm::PointerType::get(*context, llvm::NVPTXAS::ADDRESS_SPACE_GENERIC);
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    scopes.push(Scope());

    // build header
    for (shared_ptr<Statement> &headerStatement : headerStatements)
        buildStatement(headerStatement);

    // build body
    for (shared_ptr<Statement> &statement : statements)
        buildStatement(statement);

    // verify module
    string errorMessage;
    llvm::raw_string_ostream llvmErrorMessage(errorMessage);
    if (llvm::verifyModule(*module, &llvmErrorMessage))
        markError(0, 0, errorMessage);

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

    return module;
}

void ModuleBuilder::buildStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::META_IMPORT:
            buildImport(dynamic_pointer_cast<StatementImport>(statement));
            break;
        case StatementKind::FUNCTION_DECLARATION: {
            shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(statement);
            buildFunctionDeclaration(
                moduleName,
                statementFunctionDeclaration->getName(),
                statementFunctionDeclaration->getShouldExport(),
                statementFunctionDeclaration->getArguments(),
                statementFunctionDeclaration->getReturnValueType()
            );
            break;
        }
        case StatementKind::FUNCTION:
            buildFunction(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::RAW_FUNCTION:
            buildRawFunction(dynamic_pointer_cast<StatementRawFunction>(statement));
            break;
        case StatementKind::BLOB:
            buildBlob(dynamic_pointer_cast<StatementBlob>(statement));
            break;
        case StatementKind::VARIABLE:
            buildVariable(dynamic_pointer_cast<StatementVariable>(statement));
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
            buildRepeat(dynamic_pointer_cast<StatementRepeat>(statement));
            break;
        case StatementKind::META_EXTERN_FUNCTION: {
            shared_ptr<StatementMetaExternFunction> statementFunctionDeclaration = dynamic_pointer_cast<StatementMetaExternFunction>(statement);
            buildFunctionDeclaration(
                "",
                statementFunctionDeclaration->getName(),
                true,
                statementFunctionDeclaration->getArguments(),
                statementFunctionDeclaration->getReturnValueType()
            );
            break;
        }
        case StatementKind::EXPRESSION:
            buildExpression(dynamic_pointer_cast<StatementExpression>(statement));
            return;
        default:
            markError(0, 0, "Unexpected statement");
    }
}

void ModuleBuilder::buildImportStatement(shared_ptr<Statement> statement, string moduleName) {
    switch (statement->getKind()) {
        case StatementKind::FUNCTION_DECLARATION: {
            shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(statement);
            buildFunctionDeclaration(
                moduleName,
                statementFunctionDeclaration->getName(),
                true,
                statementFunctionDeclaration->getArguments(),
                statementFunctionDeclaration->getReturnValueType()
            );
            break;
        }
        default:
            markError(0, 0, "Unexpected imported statement");
    }
}

void ModuleBuilder::buildImport(shared_ptr<StatementImport> statement) {
    for (shared_ptr<Statement> &importStatement : exportedHeaderStatementsMap[statement->getName()])
        buildImportStatement(importStatement, statement->getName());
}

void ModuleBuilder::buildFunction(shared_ptr<StatementFunction> statement) {
    llvm::Function *fun = getFun(statement->getName());

    // Check if function not yet defined
    llvm::BasicBlock &entryBlock = fun->getEntryBlock();
    if (entryBlock.getParent() != nullptr) {
        markError(0, 0, format("Function \"{}\" already defined in scope", statement->getName()));
        return;
    }
    llvm::Function *par = entryBlock.getParent();

    vector<llvm::Type *> argTypes;
    for (pair<string, shared_ptr<ValueType>> &arg : statement->getArguments()) {
        llvm::Type *argType = typeForValueType(arg.second);
        if (argType == nullptr)
            return;
        argTypes.push_back(argType);
    }

    // define function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statement->getName(), fun);
    builder->SetInsertPoint(block);

    scopes.push(Scope());

    // build arguments
    int i=0;
    for (auto &arg : fun->args()) {
        string name = statement->getArguments()[i].first;
        llvm::Type *type = argTypes[i];
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
    // function types
    llvm::Type *returnType = typeForValueType(statement->getReturnValueType());
    vector<llvm::Type *> argTypes;
    for (pair<string, shared_ptr<ValueType>> &arg : statement->getArguments())
        argTypes.push_back(typeForValueType(arg.second));

    // build function declaration & body
    llvm::FunctionType *funType = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::InlineAsm *rawFun = llvm::InlineAsm::get(funType, statement->getRawSource(), statement->getConstraints(), true, false, llvm::InlineAsm::AsmDialect::AD_Intel);
    if (!setRawFun(statement->getName(), rawFun))
        return;
}

void ModuleBuilder::buildBlob(shared_ptr<StatementBlob> statement) {
    llvm::StructType *structType = llvm::StructType::create(*context, statement->getIdentifier());

    // Generate types for body
    vector<string> memberNames;
    vector<llvm::Type *> types;
    for (pair<string, shared_ptr<ValueType>> &variable: statement->getVariables()) {
        memberNames.push_back(variable.first);
        llvm::Type *type = typeForValueType(variable.second);
        if (type == nullptr)
            return;
        types.push_back(type);
    }
    structType->setBody(types, false);
    if (!registerStruct(statement->getIdentifier(), structType, memberNames))
        return;
}

void ModuleBuilder::buildVariable(shared_ptr<StatementVariable> statement) {
    llvm::AllocaInst *alloca;
    llvm::Type *valueType;

    switch (statement->getValueType()->getKind()) {
        case ValueTypeKind::DATA: {
            int count = 0;
            if (statement->getExpression() != nullptr && statement->getExpression()->getKind() == ExpressionKind::ARRAY_LITERAL)
                count = dynamic_pointer_cast<ExpressionArrayLiteral>(statement->getExpression())->getExpressions().size();
            // TODO: get number of values from existing array
            valueType = (llvm::ArrayType *)typeForValueType(statement->getValueType(), count);
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getName());
            break;
        }
        case ValueTypeKind::BLOB: {
            valueType = (llvm::StructType *)typeForValueType(statement->getValueType(), 0);
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getName());
            break;
        }
        case ValueTypeKind::PTR: {
            valueType = (llvm::PointerType *)typeForValueType(statement->getValueType(), 0);
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getName());
            break;
        }
        default: {
            valueType = typeForValueType(statement->getValueType());
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, 0, nullptr, statement->getName());
            break;
        }
    }

    // try registering new variable in scope
    if (!setAlloca(statement->getName(), alloca))
            return;

    if (statement->getExpression() != nullptr)
        buildAssignment(alloca, valueType, statement->getExpression());
}

void ModuleBuilder::buildAssignment(shared_ptr<StatementAssignment> statement) {
    llvm::Value *targetValue;
    llvm::Type *targetType;

    llvm::AllocaInst *alloca = getAlloca(statement->getIdentifier());
    if (alloca == nullptr)
        return;

    switch (statement->getAssignmentKind()) {
        case StatementAssignmentKind::SIMPLE: {
            targetValue = alloca;
            targetType = alloca->getAllocatedType();
            break;
        }
        case StatementAssignmentKind::DATA: {
            llvm::ArrayType *arrayType = (llvm::ArrayType *)alloca->getAllocatedType();
            llvm::Value *indexValue = valueForExpression(statement->getIndexExpression());
            llvm::Value *index[] = {
                builder->getInt32(0),
                indexValue
            };
            targetValue = builder->CreateGEP(arrayType, alloca, index, format("{}[]", statement->getIdentifier()));
            targetType = arrayType->getElementType();
            break;
        }
        case StatementAssignmentKind::BLOB: {      
            // First check for built-ins
            if (buildAssignmentForBuiltIn(alloca, statement->getMemberName(), statement->getValueExpression())) {
                return;
            }
            
            // Thend do a normal member assign
            llvm::StructType *structType = (llvm::StructType *)alloca->getAllocatedType();
            if (!structType->isStructTy()) {
                markError(0, 0, format("Variable {} is not of type blob", statement->getIdentifier()));
                return;
            }
            string structName = string(structType->getName());
            optional<int> memberIndex = getMemberIndex(structName, statement->getMemberName());
            if (!memberIndex)
                return;
            llvm::Value *index[] = {
                builder->getInt32(0),
                builder->getInt32(*memberIndex)
            };
            targetValue = builder->CreateGEP(structType, alloca, index, format("{}.{}", statement->getIdentifier(), statement->getMemberName()));
            targetType = structType->getElementType(*memberIndex);
            break;
        }
    }

    buildAssignment(targetValue, targetType, statement->getValueExpression());
}

void ModuleBuilder::buildBlock(shared_ptr<StatementBlock> statement) {
    for (shared_ptr<Statement> &innerStatement : statement->getStatements())
        buildStatement(innerStatement);
}

void ModuleBuilder::buildReturn(shared_ptr<StatementReturn> statement) {
    if (statement->getExpression() != nullptr) {
        llvm::Function *fun = builder->GetInsertBlock()->getParent();
        llvm::Value *value = valueForExpression(statement->getExpression(), fun->getReturnType());
        builder->CreateRet(value);
    } else {
        builder->CreateRetVoid();
    }
}

void ModuleBuilder::buildRepeat(shared_ptr<StatementRepeat> statement) {
    shared_ptr<Statement> initStatement = statement->getInitStatement();
    shared_ptr<Statement> postStatement = statement->getPostStatement();
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
    
    // Store the current stack location, stack shouldn't change accross the runs, for example because of allocas
    llvm::Type *ptrType = llvm::PointerType::get(*context, llvm::NVPTXAS::ADDRESS_SPACE_GENERIC);
    llvm::Function *stackSaveIntrinscic = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::stacksave, {ptrType});
    llvm::Value *stackValue = builder->CreateCall(stackSaveIntrinscic);

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

    // Restore stack to expected location
    llvm::Function *stackRestoreIntrinscic = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::stackrestore, {ptrType});
    builder->CreateCall(stackRestoreIntrinscic, llvm::ArrayRef({stackValue}));

    buildBlock(bodyStatement);

    // post statement
    if (postStatement != nullptr)
        buildStatement(postStatement);

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

void ModuleBuilder::buildExpression(shared_ptr<StatementExpression> statement) {
    valueForExpression(statement->getExpression());
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<Expression> expression, llvm::Type *castToType) {
    switch (expression->getKind()) {
        case ExpressionKind::LITERAL:
            return valueForLiteral(dynamic_pointer_cast<ExpressionLiteral>(expression), castToType);
        case ExpressionKind::ARRAY_LITERAL:
            return valueForArrayLiteral(dynamic_pointer_cast<ExpressionArrayLiteral>(expression), castToType);
        case ExpressionKind::GROUPING:
            return valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getExpression());
        case ExpressionKind::BINARY:
            return valueForBinary(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::UNARY:
            return valueForUnary(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::IF_ELSE:
            return valueForIfElse(dynamic_pointer_cast<ExpressionIfElse>(expression));
        case ExpressionKind::VARIABLE:
            return valueForVariable(dynamic_pointer_cast<ExpressionVariable>(expression));
        case ExpressionKind::CALL:
            return valueForCall(dynamic_pointer_cast<ExpressionCall>(expression));
        default:
            markError(0, 0, "Unexpected expression");
            return nullptr;
    }
}

llvm::Value *ModuleBuilder::valueForLiteral(shared_ptr<ExpressionLiteral> expression, llvm::Type *castToType) {
    if (expression->getLiteralKind() == LiteralKind::BOOL) {
        if (castToType == nullptr)
            return llvm::ConstantInt::get(typeBool, expression->getBoolValue(), true);
        else if (castToType != typeBool)
            return nullptr;
    }

    if (expression->getLiteralKind() == LiteralKind::UINT) {
        if (castToType == nullptr)
            return llvm::ConstantInt::get(typeU64, expression->getUIntValue(), true);
        else if (castToType == typeBool)
            return nullptr;
    }

    if (expression->getLiteralKind() == LiteralKind::SINT) {
        if (castToType == nullptr)
            return llvm::ConstantInt::get(typeS64, expression->getSIntValue(), true);
        else if (castToType == typeBool)
            return nullptr;
    }

    if (expression->getLiteralKind() == LiteralKind::REAL) {
        if (castToType == nullptr)
            return llvm::ConstantFP::get(typeR32, expression->getSIntValue());
        else if (castToType == typeBool)
            return nullptr;
    }

    if (castToType == typeBool)
        return llvm::ConstantInt::get(typeBool, expression->getBoolValue(), true);
    else if (castToType == typeU8)
        return llvm::ConstantInt::get(typeU8, expression->getUIntValue(), true);
    else if (castToType == typeU32)
        return llvm::ConstantInt::get(typeU32, expression->getUIntValue(), true);
    else if (castToType == typeU64)
        return llvm::ConstantInt::get(typeU64, expression->getUIntValue(), true);
    else if (castToType == typeS8)
        return llvm::ConstantInt::get(typeU8, expression->getSIntValue(), true);
    else if (castToType == typeS32)
        return llvm::ConstantInt::get(typeU32, expression->getSIntValue(), true);
    else if (castToType == typeS64)
        return llvm::ConstantInt::get(typeU64, expression->getSIntValue(), true);
    else if (castToType == typeR32)
        return llvm::ConstantFP::get(typeR32, expression->getSIntValue());

    return nullptr;
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
    } else if (type == typeU8 || type == typeU32 || type == typeU64) {
        return valueForBinaryUnsignedInteger(expression->getOperation(), leftValue, rightValue);
    } else if (type == typeS8 || type == typeS32 || type == typeS64) {
        return valueForBinarySignedInteger(expression->getOperation(), leftValue, rightValue);
    } else if (type == typeR32) {
        return valueForBinaryReal(expression->getOperation(), leftValue, rightValue);
    } else { // FIXME (we have missing value types)
        return valueForBinarySignedInteger(expression->getOperation(), leftValue, rightValue);
    }

    markError(0, 0, "Unexpected operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForBinaryBool(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
    switch (operation) {
    case ExpressionBinaryOperation::OR:
        return builder->CreateOr(leftValue, rightValue);
    case ExpressionBinaryOperation::XOR:
        return builder->CreateXor(leftValue, rightValue);
    case ExpressionBinaryOperation::AND:
        return builder->CreateAnd(leftValue, rightValue);
    case ExpressionBinaryOperation::EQUAL:
        return builder->CreateICmpEQ(leftValue, rightValue);
    case ExpressionBinaryOperation::NOT_EQUAL:
        return builder->CreateICmpNE(leftValue, rightValue);
    default:
        markError(0, 0, "Unexpected operation for boolean operands");
        return nullptr;
    }
}

llvm::Value *ModuleBuilder::valueForBinaryUnsignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
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
            return builder->CreateNUWAdd(leftValue, rightValue);
        case ExpressionBinaryOperation::SUB:
            return builder->CreateNUWSub(leftValue, rightValue);
        case ExpressionBinaryOperation::MUL:
            return builder->CreateNUWMul(leftValue, rightValue);
        case ExpressionBinaryOperation::DIV:
            return builder->CreateUDiv(leftValue, rightValue);
        case ExpressionBinaryOperation::MOD:
            return builder->CreateURem(leftValue, rightValue);
    }
}

llvm::Value *ModuleBuilder::valueForBinarySignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
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

llvm::Value *ModuleBuilder::valueForUnary(shared_ptr<ExpressionUnary> expression) {
    llvm::Value *value = valueForExpression(expression->getExpression());
    llvm::Type *type = value->getType();

    if (type == typeBool) {
        if (expression->getOperation() == ExpressionUnaryOperation::NOT) {
            return builder->CreateNot(value);
        }
    } else if (type == typeU8 || type == typeU32 || type == typeU64) {
        if (expression->getOperation() == ExpressionUnaryOperation::MINUS) {
            return builder->CreateNeg(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::PLUS) {
            return value;
        }
    } else if (type == typeS8 || type == typeS32 || type == typeS64) {
        if (expression->getOperation() == ExpressionUnaryOperation::MINUS) {
            return builder->CreateNSWNeg(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::PLUS) {
            return value;
        }
    } else if (type == typeR32) {
        if (expression->getOperation() == ExpressionUnaryOperation::MINUS) {
            return builder->CreateFNeg(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::PLUS) {
            return value;
        }
    }

    markError(0, 0, "Unexpected operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForIfElse(shared_ptr<ExpressionIfElse> expression) {
    shared_ptr<Expression> conditionExpression = expression->getCondition();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::Value *conditionValue = valueForExpression(conditionExpression);

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(*context, "thenBlock", fun);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(*context, "elseBlock");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(*context, "mergeBlock");

    if (expression->getElseBlock() != nullptr) {
        builder->CreateCondBr(conditionValue, thenBlock, elseBlock);
    } else {
        builder->CreateCondBr(conditionValue, thenBlock, mergeBlock);
    }

    // Then
    scopes.push(Scope());
    builder->SetInsertPoint(thenBlock);
    buildStatement(expression->getThenBlock()->getStatementBlock());
    llvm::Value *thenValue = valueForExpression(expression->getThenBlock()->getResultStatementExpression()->getExpression());
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();
    scopes.pop();

    // Else
    llvm::Value *elseValue = nullptr;
    if (expression->getElseBlock() != nullptr) {
        scopes.push(Scope());
        fun->insert(fun->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);
        buildStatement(expression->getElseBlock()->getStatementBlock());
        elseValue = valueForExpression(expression->getElseBlock()->getResultStatementExpression()->getExpression());
        builder->CreateBr(mergeBlock);
        elseBlock = builder->GetInsertBlock();
        scopes.pop();
    }

    // Merge
    fun->insert(fun->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    // we can only have a return value if else is also present and both then & else return the same type
    if (thenValue == nullptr || thenValue->getType()->isVoidTy() || elseValue == nullptr || thenValue->getType() != elseValue->getType()) {
        return llvm::UndefValue::get(typeVoid);
    } else {
        llvm::PHINode *phi = builder->CreatePHI(thenValue->getType(), 2, "ifElseResult");
        phi->addIncoming(thenValue, thenBlock);
        phi->addIncoming(elseValue, elseBlock);

        return phi;
    }
}

llvm::Value *ModuleBuilder::valueForVariable(shared_ptr<ExpressionVariable> expression) {
    llvm::AllocaInst *alloca = getAlloca(expression->getIdentifier());
    if (alloca == nullptr)
        return nullptr;

    switch (expression->getVariableKind()) {
        case ExpressionVariableKind::SIMPLE: {
            return builder->CreateLoad(alloca->getAllocatedType(), alloca, expression->getIdentifier());
        }
        case ExpressionVariableKind::DATA: {
            llvm::Value *indexValue = valueForExpression(expression->getIndexExpression());
            llvm::Value *index[] = {
                builder->getInt32(0),
                indexValue
            };
            llvm::ArrayType *type = (llvm::ArrayType *)alloca->getAllocatedType();
            llvm::Value *elementPtr = builder->CreateGEP(type, alloca, index, format("{}[]", expression->getIdentifier()));

            return builder->CreateLoad(type->getArrayElementType(), elementPtr);
        }
        case ExpressionVariableKind::BLOB: {
            // First check for built-ins
            llvm::Value *builtInValue = valueForBuiltIn(alloca, expression->getMemberName());
            if (builtInValue != nullptr) {
                return builtInValue;
            }

            // Then do a normal member check
            llvm::StructType *structType = (llvm::StructType *)alloca->getAllocatedType();
            if (!structType->isStructTy()) {
                markError(0, 0, format("Variable {} is not of type blob", expression->getIdentifier()));
                return nullptr;
            }
            string structName = string(structType->getName());
            optional<int> memberIndex = getMemberIndex(structName, expression->getMemberName());
            if (!memberIndex)
                return nullptr;
            llvm::Value *index[] = {
                builder->getInt32(0),
                builder->getInt32(*memberIndex)
            };
            llvm::Value *elementPtr = builder->CreateGEP(structType, alloca, index, format("{}.{}", expression->getIdentifier(), expression->getMemberName()));
            return builder->CreateLoad(structType->getElementType(*memberIndex), elementPtr);
        }
    }
}

llvm::Value *ModuleBuilder::valueForCall(shared_ptr<ExpressionCall> expression) {
    llvm::Function *fun = getFun(expression->getName());
    if (fun != nullptr) {
        llvm::FunctionType *funType = fun->getFunctionType();        
        vector<llvm::Value*> argValues;
        vector<shared_ptr<Expression>> argumentExpressions = expression->getArgumentExpressions();
        for (int i=0; i<argumentExpressions.size(); i++) {
            // pass along type for the specified argument
            llvm::Type *argumentType = funType->getParamType(i);
            shared_ptr<Expression> argumentExpression = argumentExpressions.at(i);
            llvm::Value *argValue = valueForExpression(argumentExpression, argumentType);
            argValues.push_back(argValue);
        }
        return builder->CreateCall(funType, fun, llvm::ArrayRef(argValues));
    }

    llvm::InlineAsm *rawFun = getRawFun(expression->getName());
    if (rawFun != nullptr) {
        vector<llvm::Value *>argValues;
        for (shared_ptr<Expression> &argumentExpression : expression->getArgumentExpressions()) {
            llvm::Value *argValue = valueForExpression(argumentExpression);
            argValues.push_back(argValue);
        }
        return builder->CreateCall(rawFun, llvm::ArrayRef(argValues));
    }

    markError(0, 0, format("Function \"{}\" not defined in scope", expression->getName()));
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForArrayLiteral(shared_ptr<ExpressionArrayLiteral> expression, llvm::Type *castToType) {
    int count;
    llvm::Type *elementType;
    if (castToType != nullptr) {
        elementType = ((llvm::ArrayType*)castToType)->getElementType();
        count = ((llvm::ArrayType*)castToType)->getArrayNumElements();
    } else if (!expression->getExpressions().empty()) {
        elementType = valueForExpression(expression->getExpressions().at(0))->getType();
        count = expression->getExpressions().size();
    } else {
        elementType = typeU64;
        count = 0;
    }

    llvm::Type *targetType = llvm::ArrayType::get(elementType, count);
    llvm::AllocaInst *targetAlloca = builder->CreateAlloca(targetType, nullptr);

    buildAssignment(targetAlloca, targetType, expression);
    return builder->CreateLoad(targetType, targetAlloca);
}

llvm::Value *ModuleBuilder::valueForBuiltIn(llvm::AllocaInst *alloca, string memberName) {
    bool isArray = alloca->getAllocatedType()->isArrayTy();
    bool isPointer = alloca->getAllocatedType()->isPointerTy();

    if (isArray && memberName.compare("count") == 0) {
        llvm::ArrayType *arrayType = (llvm::ArrayType*)alloca->getAllocatedType();
        return valueForLiteral(ExpressionLiteral::expressionLiteralForUInt(arrayType->getNumElements()));
        //return llvm::ConstantInt::get(typeU32, arrayType->getNumElements());
    } else if (isPointer && memberName.compare("val") == 0) {
        llvm::Value *pointee = builder->CreateLoad(typePtr, alloca);
        llvm::Type *pointeeType = pointee->getType();
        // TODO: This needs to get proper type from the pointee
        //return builder->CreateLoad(pointeeType, pointeeAlloca);
        return builder->CreateLoad(typeU32, pointee);
    } else if (isPointer && memberName.compare("vAddr") == 0) {
        llvm::AllocaInst *pointeeAlloca = (llvm::AllocaInst*)builder->CreateLoad(typePtr, alloca);
        return builder->CreatePtrToInt(pointeeAlloca, typePtr);
    } else if (memberName.compare("adr") == 0) {
        return builder->CreatePtrToInt(alloca, typePtr);
    }

    return nullptr;
}

void ModuleBuilder::buildFunctionDeclaration(string moduleName, string name, bool isExtern, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnType) {    
    // name
    string funName = name;
    if (!moduleName.empty() && moduleName.compare(defaultModuleName) != 0)
        funName = format("{}.{}", moduleName, name);

    // arguments
    vector<llvm::Type *> funArgTypes;
    for (pair<string, shared_ptr<ValueType>> &argument : arguments) {
        llvm::Type *funArgType = typeForValueType(argument.second);
        if (funArgType == nullptr)
            return;
        funArgTypes.push_back(funArgType);
    }

    // return type
    llvm::Type *funReturnType = typeForValueType(returnType);
    if (funReturnType == nullptr)
        return;
    
    // linkage
    llvm::GlobalValue::LinkageTypes funLinkage = llvm::GlobalValue::LinkageTypes::InternalLinkage;
    if (isExtern)
        funLinkage = llvm::GlobalValue::ExternalLinkage;

    // build function declaration
    llvm::FunctionType *funType = llvm::FunctionType::get(funReturnType, funArgTypes, false);
    llvm::Function *fun = llvm::Function::Create(funType, funLinkage, funName, *module);

    // register
    string internalName = name;
    if (moduleName.compare(this->moduleName) != 0)
        internalName = funName;
    setFun(internalName, fun);
}

void ModuleBuilder::buildAssignment(llvm::Value *targetValue, llvm::Type *targetType, shared_ptr<Expression> valueExpression) {
    // data
    if (targetType->isArrayTy()) {
        switch (valueExpression->getKind()) {
            // data <- array literal
            // copy values from literal expression into an allocated array
            case ExpressionKind::ARRAY_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionArrayLiteral>(valueExpression)->getExpressions();
                int sourceCount = valueExpressions.size();
                int targetCount = ((llvm::ArrayType *)targetType)->getNumElements();
                int count = min(sourceCount, targetCount);
                string targetName = string(targetValue->getName());
                llvm::Type *elementType = ((llvm::ArrayType *)targetType)->getArrayElementType();
                for (int i=0; i<count; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *targetPtr = builder->CreateGEP(targetType, targetValue, index, format("{}[{}]", targetName, i));
                    llvm::Value *sourceValue = valueForExpression(valueExpressions.at(i), elementType);
                    builder->CreateStore(sourceValue, targetPtr);
                }
                break;
            }
            // data <- var
            // copy each value from one allocated array to another allocated array
            case ExpressionKind::VARIABLE: {
                shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(valueExpression);
                llvm::AllocaInst *sourceValue = getAlloca(expressionVariable->getIdentifier());
                if (sourceValue == nullptr)
                    return;

                llvm::Type *sourceType = sourceValue->getAllocatedType();
                if (sourceType->isArrayTy()) {
                    // make sure we don't go over the bounds
                    int sourceCount = ((llvm::ArrayType *)sourceType)->getNumElements();
                    int targetCount = ((llvm::ArrayType *)targetType)->getNumElements();
                    int count = min(sourceCount, targetCount);

                    string sourceName = string(sourceValue->getName());
                    string targetName = string(targetValue->getName());

                    for (int i=0; i<count; i++) {
                        llvm::Value *index[] = {
                            builder->getInt32(0),
                            builder->getInt32(i)
                        };

                        // get pointers for both source and target
                        llvm::Value *sourcePtr = builder->CreateGEP(sourceType, sourceValue, index, format("{}[{}]", sourceName, i));
                        llvm::Value *targetPtr = builder->CreateGEP(targetType, targetValue, index, format("{}[{}]", targetName, i));

                        // load value from source pointer
                        llvm::Value *sourcePtrValue = builder->CreateLoad(sourceType->getArrayElementType(), sourcePtr);

                        // and then store it in the target pointer
                        builder->CreateStore(sourcePtrValue, targetPtr);
                    }
                }
                break;
            }
            case ExpressionKind::CALL: {
                shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(valueExpression);
                llvm::Value *sourceValue = valueForCall(expressionCall);
                if (sourceValue == nullptr)
                    return;
                    
                llvm::Type *sourceType = sourceValue->getType();
                if (sourceType->isArrayTy()) {
                    // store call result in a temporary alloca
                    llvm::AllocaInst *sourceAlloca = builder->CreateAlloca(sourceType);
                    builder->CreateStore(sourceValue, sourceAlloca);

                    // make sure we don't go over the bounds
                    int sourceCount = ((llvm::ArrayType *)sourceType)->getNumElements();
                    int targetCount = ((llvm::ArrayType *)targetType)->getNumElements();
                    int count = min(sourceCount, targetCount);

                    string targetName = string(targetValue->getName());

                    // copy from the temporary alloca
                    for (int i=0; i<count; i++) {
                        llvm::Value *index[] = {
                            builder->getInt32(0),
                            builder->getInt32(i)
                        };

                        // get pointers for both source and target
                        llvm::Value *sourcePtr = builder->CreateGEP(sourceType, sourceAlloca, index, format("[{}]", i));
                        llvm::Value *targetPtr = builder->CreateGEP(targetType, targetValue, index, format("{}[{}]", targetName, i));

                        // load value from source pointer
                        llvm::Value *sourcePtrValue = builder->CreateLoad(sourceType->getArrayElementType(), sourcePtr);

                        builder->CreateStore(sourcePtrValue, targetPtr);
                    }
                }

                break;
            }
            // other
            default:
                markError(0, 0, "Invalid assignment to data type");
                return;
        }
    // blob
    } else if (targetType->isStructTy()) {
        switch (valueExpression->getKind()) {
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                int membersCount = ((llvm::StructType*)targetType)->getStructNumElements();
                for (int i=0; i<membersCount; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Type *memberType = ((llvm::StructType*)targetType)->getElementType(i);
                    llvm::Value *sourceValue = valueForExpression(valueExpressions.at(i), memberType);
                    llvm::Value *targetMember = builder->CreateGEP(targetType, targetValue, index);
                    builder->CreateStore(sourceValue, targetMember);
                }
                break;
            }
            case ExpressionKind::VARIABLE: {
                shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(valueExpression);
                llvm::AllocaInst *sourceValue = getAlloca(expressionVariable->getIdentifier());
                llvm::Type *sourceType = sourceValue->getAllocatedType();
                if (!sourceType->isStructTy()) {
                    markError(0, 0, "Not a blob type");
                    return;
                }
                string targetTypeName = string(targetType->getStructName());
                string sourceTypeName = string(sourceType->getStructName());
                if (targetTypeName.compare(sourceTypeName) != 0) {
                    markError(0, 0, "Incompatible blob types");
                    return;
                }

                int membersCount = ((llvm::StructType*)targetType)->getStructNumElements();
                for (int i=0; i<membersCount; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *sourceMemberValue = builder->CreateGEP(sourceType, sourceValue, index);
                    llvm::Value *targetMemberValue = builder->CreateGEP(targetType, targetValue, index);
                    builder->CreateStore(sourceMemberValue, targetMemberValue);
                }
                break;
            }
            default:
                markError(0, 0, "Invalid assignment");
                break;
        }
    // simple
    } else {
        llvm::Type *castToType = nullptr;
        switch (valueExpression->getKind()) {
            // simple <- literal
            case ExpressionKind::LITERAL:
                castToType = targetType;
            // simple <- binary expression
            case ExpressionKind::BINARY:
            // simple <- ( expression )
            case ExpressionKind::GROUPING:
            case ExpressionKind::UNARY:
            // simple <- if else
            case ExpressionKind::IF_ELSE:
            // simple <- function call
            case ExpressionKind::CALL:
            // simple <- var
            case ExpressionKind::VARIABLE: {
                llvm::Value *sourceValue = valueForExpression(valueExpression, castToType);
                if (sourceValue == nullptr)
                    return;
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            // other
            default:
                markError(0, 0, "Invalid assignment");
                return;
        }
    }
}

bool ModuleBuilder::buildAssignmentForBuiltIn(llvm::AllocaInst *alloca, string memberName, shared_ptr<Expression> valueExpression) {
    bool isPointer = alloca->getAllocatedType()->isPointerTy();

    if (isPointer && memberName.compare("vAdr") == 0) {
        llvm::Value *adrValue = valueForExpression(valueExpression);
        llvm::Value *newPtr = builder->CreateIntToPtr(adrValue, typePtr);
        builder->CreateStore(newPtr, alloca);
        return true;
    } else if (isPointer && memberName.compare("val") == 0) {
        llvm::Value *newValue = valueForExpression(valueExpression);
        llvm::Value *pointee = builder->CreateLoad(typePtr, alloca);
        builder->CreateStore(newValue, pointee);
        return true;
    }

    return false;
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
        markError(0, 0, format("Function \"{}\" already declared in scope", name));
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
        markError(0, 0, format("Raw function \"{}\" already declared in scope", name));
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

bool ModuleBuilder::registerStruct(string structName, llvm::StructType *structType, vector<string> memberNames) {
    if (scopes.top().structTypeMap[structName] != nullptr) {
        markError(0, 0, format("Blob \"{}\" already defined in scope",structName));
        return false;
    }

    scopes.top().structTypeMap[structName] = structType;
    scopes.top().structMembersMap[structName] = memberNames;

    return true;
}

llvm::StructType *ModuleBuilder::getStructType(string structName) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        llvm::StructType *structType = scopes.top().structTypeMap[structName];
        if (structType != nullptr)
            return structType;
        scopes.pop();
    }

    return nullptr;
}

optional<int> ModuleBuilder::getMemberIndex(string structName, string memberName) {
        stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        if (scopes.top().structMembersMap.contains(structName)) {
            vector<string> memberNames = scopes.top().structMembersMap[structName];
            for (int i=0; i<memberNames.size(); i++) {
                if (memberNames[i].compare(memberName) == 0)
                    return i;
            }
        }
        scopes.pop();
    }

    return {};
}

llvm::Type *ModuleBuilder::typeForValueType(shared_ptr<ValueType> valueType, int count) {
    if (valueType == nullptr) {
        markError(0, 0, "Missing type");
        return nullptr;
    }

    switch (valueType->getKind()) {
        case ValueTypeKind::NONE:
            return typeVoid;
        case ValueTypeKind::BOOL:
            return typeBool;
        case ValueTypeKind::U8:
            return typeU8;
        case ValueTypeKind::U32:
            return typeU32;
        case ValueTypeKind::U64:
            return typeU64;
        case ValueTypeKind::S8:
            return typeS8;
        case ValueTypeKind::S32:
            return typeS32;
        case ValueTypeKind::S64:
            return typeS64;
        case ValueTypeKind::R32:
            return typeR32;
        case ValueTypeKind::DATA: {
            if (valueType->getSubType() == nullptr)
                return nullptr;
            if (valueType->getValueArg() > 0)
                count = valueType->getValueArg();
            return llvm::ArrayType::get(typeForValueType(valueType->getSubType(), count), count);
        }
        case ValueTypeKind::BLOB:
            return getStructType(valueType->getTypeName());
        case ValueTypeKind::PTR:
            return typePtr;
        default:
            return nullptr;
    }
}

void ModuleBuilder::markError(int line, int column, string message) {
    errors.push_back(Error::builderError(line, column, message));
}
