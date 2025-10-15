#include "ModuleBuilder.h"

#include "Error.h"
#include "Logger.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCast.h"

#include "Parser/Statement/StatementImport.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementVariableDeclaration.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"

ModuleBuilder::ModuleBuilder(
    string moduleName,
    string defaultModuleName,
    int intSize,
    int pointerSize,
    vector<shared_ptr<Statement>> statements,
    vector<shared_ptr<Statement>> headerStatements,
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap
):
moduleName(moduleName), defaultModuleName(defaultModuleName), statements(statements), headerStatements(headerStatements), exportedHeaderStatementsMap(exportedHeaderStatementsMap) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>(moduleName, *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    typeVoid = llvm::Type::getVoidTy(*context);
    typeBool = llvm::Type::getInt1Ty(*context);

    typeU8 = llvm::Type::getInt8Ty(*context);
    typeU32 = llvm::Type::getInt32Ty(*context);
    typeU64 = llvm::Type::getInt64Ty(*context);
    typeUInt = llvm::Type::getIntNTy(*context, intSize);

    typeS8 = llvm::Type::getInt8Ty(*context);
    typeS32 = llvm::Type::getInt32Ty(*context);
    typeS64 = llvm::Type::getInt64Ty(*context);
    typeSInt = llvm::Type::getIntNTy(*context, intSize);

    typeF32 = llvm::Type::getFloatTy(*context);
    typeF64 = llvm::Type::getDoubleTy(*context);
    typeFloat = llvm::Type::getFloatTy(*context);

    typePtr = llvm::PointerType::get(*context, llvm::NVPTXAS::ADDRESS_SPACE_GENERIC);
    typeIntPtr = llvm::Type::getIntNTy(*context, pointerSize);

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
        case StatementKind::BLOB_DECLARATION:
            buildBlobDeclaration(dynamic_pointer_cast<StatementBlobDeclaration>(statement));
            break;
        case StatementKind::BLOB:
            buildBlob(dynamic_pointer_cast<StatementBlob>(statement));
            break;
        case StatementKind::VARIABLE_DECLARATION:
            buildVariableDeclaration(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            break;
        case StatementKind::VARIABLE:
            buildVariable(dynamic_pointer_cast<StatementVariable>(statement));
            break;
        case StatementKind::ASSIGNMENT:
            buildAssignmentChained(dynamic_pointer_cast<StatementAssignment>(statement));
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
        case StatementKind::VARIABLE_DECLARATION:
            buildVariableDeclaration(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            break;
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

        if (type->isPointerTy()) {
            shared_ptr<ValueType> valueType = statement->getArguments().at(i).second;
            if(!setPtrType(name, valueType))
                return;
        }

        llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr, name);
        if (!setAlloca(name, alloca))
            return;
        builder->CreateStore(&arg, alloca);

        i++;
    }

    // build function body
    buildStatement(statement->getStatementBlock());

    scopes.pop();

    builder->SetInsertPoint((llvm::BasicBlock*)nullptr);

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
    if(llvm::InlineAsm::verify(funType, statement->getConstraints())) {
        markError(0, 0, format("Constraints \"{}\", are invalid", statement->getConstraints()));
        return;
    }
    llvm::InlineAsm *rawFun = llvm::InlineAsm::get(funType, statement->getRawSource(), statement->getConstraints(), true, false, llvm::InlineAsm::AsmDialect::AD_Intel);

    if (!setRawFun(statement->getName(), rawFun))
        return;
}

void ModuleBuilder::buildBlobDeclaration(shared_ptr<StatementBlobDeclaration> statement) {
    llvm::StructType::create(*context, statement->getIdentifier());
}

void ModuleBuilder::buildBlob(shared_ptr<StatementBlob> statement) {
    llvm::StructType *structType = llvm::StructType::getTypeByName(*context, statement->getIdentifier());
    if (structType == nullptr) {
        markError(0, 0, "Blob not declared");
        return;
    }

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

void ModuleBuilder::buildVariableDeclaration(shared_ptr<StatementVariableDeclaration> statement) {
    // type
    llvm::Type *type = typeForValueType(statement->getValueType(), 0);
    if (type == nullptr)
        return;

    // linkage
    llvm::GlobalValue::LinkageTypes linkage = statement->getShouldExport() ?
        linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage :
        llvm::GlobalValue::LinkageTypes::InternalLinkage;

    llvm::GlobalVariable *global = new llvm::GlobalVariable(*module, type, false, linkage, nullptr, statement->getName());

    if (!setGlobal(statement->getName(), global))
        return;

    if (!setPtrType(statement->getName(), statement->getValueType()))
        return;
}

void ModuleBuilder::buildVariable(shared_ptr<StatementVariable> statement) {
    if (builder->GetInsertBlock() != nullptr)
        buildLocalVariable(statement);
    else
        buildGlobalVariable(statement);
}

void ModuleBuilder::buildLocalVariable(shared_ptr<StatementVariable> statement) {
    llvm::AllocaInst *alloca;
    llvm::Type *valueType;

    switch (statement->getValueType()->getKind()) {
        case ValueTypeKind::DATA: {
            int count = 0;
            if (statement->getExpression() != nullptr) {
                switch (statement->getExpression()->getKind()) {
                    case ExpressionKind::COMPOSITE_LITERAL: {
                        count = dynamic_pointer_cast<ExpressionCompositeLiteral>(statement->getExpression())->getExpressions().size();
                        break;
                    }
                    case ExpressionKind::VARIABLE: {
                        string identifier = dynamic_pointer_cast<ExpressionVariable>(statement->getExpression())->getIdentifier();
                        llvm::AllocaInst *alloca = getAlloca(identifier);
                        if (alloca != nullptr && alloca->getAllocatedType()->isArrayTy()) {
                            count = ((llvm::ArrayType*)alloca->getAllocatedType())->getNumElements();
                        }
                        break;
                    }
                    case ExpressionKind::CALL: {
                        string funName = dynamic_pointer_cast<ExpressionCall>(statement->getExpression())->getName();
                        llvm::Function *fun = getFun(funName);
                        if (fun != nullptr && fun->getReturnType()->isArrayTy()) {
                            count = ((llvm::ArrayType*)fun->getReturnType())->getNumElements();
                        }
                    }
                    default:
                        // should get it from the type itself
                        break;
                }
            }
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
            if (!setPtrType(statement->getName(), statement->getValueType()))
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getName());
            break;
        }
        default: {
            valueType = typeForValueType(statement->getValueType());
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, 0, nullptr, statement->getName());
        }
    }

    // try registering new variable in scope
    if (!setAlloca(statement->getName(), alloca))
        return;

    if (statement->getExpression() != nullptr)
        buildAssignment(alloca, valueType, statement->getExpression());
}

void ModuleBuilder::buildGlobalVariable(shared_ptr<StatementVariable> statement) {
    // variable
    llvm::GlobalVariable *global = (llvm::GlobalVariable*)getGlobal(statement->getName());

    if (global->hasInitializer()) {
        markError(0, 0, format("Global variable \"{}\" already defined in scope", statement->getName()));
        return;
    }

    // type
    shared_ptr<ValueType> valueType = getPtrType(statement->getName());
    llvm::Type *type = typeForValueType(valueType);

    // initialization
    llvm::Constant *initConstant = llvm::Constant::getNullValue(type);
    if (statement->getExpression() != nullptr) {
        initConstant = constantValueForExpression(statement->getExpression(), type);
        if (initConstant == nullptr) {
            markError(0, 0, "Not a constant expression");
            return;
        }
    }

    global->setInitializer(initConstant);
}

void ModuleBuilder::buildAssignmentChained(shared_ptr<StatementAssignment> statement) {
    llvm::Value *targetValue = valueForChainExpressions(statement->getChainExpressions());

    // Figure out opearand for the store operation
    llvm::Value *targetOperand;
    llvm::LoadInst *targetLoad = llvm::dyn_cast<llvm::LoadInst>(targetValue);
    if (targetLoad != nullptr)
        targetOperand = targetLoad->getOperand(0);
    else
        targetOperand = targetValue;

    buildAssignment(targetOperand, targetValue->getType(), statement->getValueExpression());
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
        case ExpressionKind::COMPOSITE_LITERAL:
            return valueForCompositeLiteral(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression), castToType);
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
        case ExpressionKind::CHAINED:
            return valueForChained(dynamic_pointer_cast<ExpressionChained>(expression));
        case ExpressionKind::BLOCK:
            return valueForBlock(dynamic_pointer_cast<ExpressionBlock>(expression));
        default:
            markError(0, 0, "Unexpected expression");
            return nullptr;
    }
}

llvm::Constant *ModuleBuilder::constantValueForExpression(shared_ptr<Expression> expression, llvm::Type *castToType) {
    llvm::Value *value = nullptr;
    switch (expression->getKind()) {
        case ExpressionKind::LITERAL:
            value = valueForLiteral(dynamic_pointer_cast<ExpressionLiteral>(expression), castToType);
            break;
        case ExpressionKind::COMPOSITE_LITERAL:
            value = constantValueForCompositeLiteral(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression), castToType);
            break;
        case ExpressionKind::GROUPING:
            value = valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getExpression());
            break;
        case ExpressionKind::BINARY:
            value = valueForBinary(dynamic_pointer_cast<ExpressionBinary>(expression));
            break;
        case ExpressionKind::UNARY:
            value = valueForUnary(dynamic_pointer_cast<ExpressionUnary>(expression));
            break;
        default:
            markError(0, 0, "Invalid constant expression");
            return nullptr;
    }
    return llvm::dyn_cast<llvm::Constant>(value);
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
            return llvm::ConstantInt::get(typeUInt, expression->getUIntValue(), true);
        else if (castToType == typeBool)
            return nullptr;
    }

    if (expression->getLiteralKind() == LiteralKind::SINT) {
        if (castToType == nullptr)
            return llvm::ConstantInt::get(typeSInt, expression->getSIntValue(), true);
        else if (castToType == typeBool)
            return nullptr;
    }

    if (expression->getLiteralKind() == LiteralKind::FLOAT) {
        if (castToType == nullptr)
            return llvm::ConstantFP::get(typeFloat, expression->getFloatValue());
        else if (castToType == typeBool)
            return nullptr;
    }

    if (castToType == typeBool) {
        return llvm::ConstantInt::get(typeBool, expression->getBoolValue(), true);
    } else if (castToType == typeU8 || castToType == typeU32 || castToType == typeU64 || castToType == typeUInt) {
        return llvm::ConstantInt::get(castToType, expression->getUIntValue(), true);
    } else if (castToType == typeS8 || castToType == typeS32 || castToType == typeS64 || castToType == typeSInt) {
        return llvm::ConstantInt::get(castToType, expression->getSIntValue(), true);
    } else if (castToType == typeF32 || castToType == typeF64 || castToType == typeFloat) {
        return llvm::ConstantFP::get(castToType, expression->getFloatValue());
    }

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
    } else if (type == typeF32 || type == typeF64 || type == typeFloat) {
        return valueForBinaryFloat(expression->getOperation(), leftValue, rightValue);
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
        return builder->CreateLogicalAnd(leftValue, rightValue);
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
        case ExpressionBinaryOperation::BIT_OR:
            return builder->CreateOr(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_XOR:
            return builder->CreateXor(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_AND:
            return builder->CreateAnd(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_SHL:
            return builder->CreateShl(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_SHR:
            return builder->CreateLShr(leftValue, rightValue);

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
        case ExpressionBinaryOperation::BIT_OR:
            return builder->CreateOr(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_XOR:
            return builder->CreateXor(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_AND:
            return builder->CreateAnd(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_SHL:
            return builder->CreateShl(leftValue, rightValue);
        case ExpressionBinaryOperation::BIT_SHR:
            return builder->CreateAShr(leftValue, rightValue);

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

llvm::Value *ModuleBuilder::valueForBinaryFloat(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue) {
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
    } else if (type == typeU8 || type == typeU32 || type == typeU64 || type == typeUInt) {
        if (expression->getOperation() == ExpressionUnaryOperation::BIT_NOT) {
            return builder->CreateNot(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::MINUS) {
            return builder->CreateNeg(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::PLUS) {
            return value;
        }
    } else if (type == typeS8 || type == typeS32 || type == typeS64 || type == typeSInt) {
        if (expression->getOperation() == ExpressionUnaryOperation::BIT_NOT) {
            return builder->CreateNot(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::MINUS) {
            return builder->CreateNSWNeg(value);
        } else if (expression->getOperation() == ExpressionUnaryOperation::PLUS) {
            return value;
        }
    } else if (type == typeF32 || type == typeF64 || type == typeFloat) {
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

    if (expression->getElseExpression() != nullptr) {
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
    if (expression->getElseExpression() != nullptr) {
        scopes.push(Scope());
        fun->insert(fun->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);
        elseValue = valueForExpression(expression->getElseExpression());
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
    llvm::Value *value = nullptr;
    llvm::Type *type = nullptr;

    llvm::AllocaInst *localAlloca = getAlloca(expression->getIdentifier());
    llvm::Value *globalValuePtr = getGlobal(expression->getIdentifier());
    if (localAlloca != nullptr) {
        value = localAlloca;
        type = localAlloca->getAllocatedType();
    } else if (globalValuePtr != nullptr) {
        shared_ptr<ValueType> valueType = getPtrType(expression->getIdentifier());
        type = typeForValueType(valueType);
        value = globalValuePtr;
    }

    if (value == nullptr) {
        markError(0, 0, format("Variable \"{}\" not defined in scope", expression->getIdentifier()));
        return nullptr;
    }

    return valueForSourceValue(value, type, expression);
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

llvm::Value *ModuleBuilder::valueForChained(shared_ptr<ExpressionChained> expression) {
    return valueForChainExpressions(expression->getChainExpressions());
}

llvm::Value *ModuleBuilder::valueForBlock(shared_ptr<ExpressionBlock> expression) {
    buildStatement(expression->getStatementBlock());
    return valueForExpression(expression->getResultStatementExpression()->getExpression());
}

llvm::Value *ModuleBuilder::valueForChainExpressions(vector<shared_ptr<Expression>> chainExpressions) {
    llvm::Value *currentValue = nullptr;

    for (int i=0; i<chainExpressions.size(); i++) {
        shared_ptr<Expression> chainExpression = chainExpressions.at(i);

        // First in chain is treated as a single variable
        if (currentValue == nullptr) {
            currentValue = valueForExpression(chainExpression);
            continue;
        }

        // Figure out opearand for the load operation
        llvm::Value *sourceOperand;
        llvm::LoadInst *sourceLoad = llvm::dyn_cast<llvm::LoadInst>(currentValue);
        if (sourceLoad != nullptr)
            sourceOperand = sourceLoad->getOperand(0);
        else
            sourceOperand = currentValue;

        // Cast expression?
        shared_ptr<ExpressionCast> expressionCast = dynamic_pointer_cast<ExpressionCast>(chainExpression);
        if (expressionCast != nullptr) {
            return valueForCast(currentValue, expressionCast->getValueType());
        }

        // Check required types
        shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(chainExpression);
        shared_ptr<ExpressionVariable> parentExpressionVariable = dynamic_pointer_cast<ExpressionVariable>(chainExpressions.at(i-1));
        if (expressionVariable == nullptr || parentExpressionVariable == nullptr) {
            markError(0, 0, "Invalid expression type");
            return nullptr;
        }

        // Built-in expression?
        llvm::Value *builtInValue = valueForBuiltIn(currentValue, parentExpressionVariable, expressionVariable);
        if (builtInValue != nullptr) {
            currentValue = builtInValue;
            continue;
        }

        // Variable expression?
        llvm::StructType *structType = (llvm::StructType*)currentValue->getType();
        if (!structType->isStructTy()) {
            markError(0, 0, "Something's fucky");
            return nullptr;
        }
        string structName = string(structType->getName());
        optional<int> memberIndex = getMemberIndex(structName, expressionVariable->getIdentifier());
        if (!memberIndex)
            return nullptr;
        llvm::Value *index[] = {
            builder->getInt32(0),
            builder->getInt32(*memberIndex)
        };

        llvm::Value *elementPtr = builder->CreateGEP(structType, sourceOperand, index);
        llvm::Type *elementType = structType->getElementType(*memberIndex);

        currentValue = valueForSourceValue(elementPtr, elementType, expressionVariable);
    }

    return currentValue;
}

llvm::Value *ModuleBuilder::valueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType, shared_ptr<ExpressionVariable> expression) {
    if (builder->GetInsertBlock() == nullptr)
        return nullptr;

    switch (expression->getVariableKind()) {
        case ExpressionVariableKind::SIMPLE: {
            return builder->CreateLoad(sourceType, sourceValue, expression->getIdentifier());
        }
        case ExpressionVariableKind::DATA: {
            llvm::Value *indexValue = valueForExpression(expression->getIndexExpression());
            llvm::Value *index[] = {
                builder->getInt32(0),
                indexValue
            };
            llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(sourceType);
            llvm::Value *elementPtr = builder->CreateGEP(sourceArrayType, sourceValue, index, format("{}[]", expression->getIdentifier()));
            return builder->CreateLoad(sourceArrayType->getArrayElementType(), elementPtr);
        }
    }
}

llvm::Value *ModuleBuilder::valueForCompositeLiteral(shared_ptr<ExpressionCompositeLiteral> expression, llvm::Type *castToType) {
    if (castToType == nullptr) {
        markError(0, 0, "Don't know what to do with the composite");
        return nullptr;
    }

    llvm::AllocaInst *targetAlloca = builder->CreateAlloca(castToType, nullptr);
    buildAssignment(targetAlloca, castToType, expression);
    return builder->CreateLoad(castToType, targetAlloca);
}

llvm::Constant *ModuleBuilder::constantValueForCompositeLiteral(shared_ptr<ExpressionCompositeLiteral> expression, llvm::Type *castToType) {
    bool isArray = castToType->isArrayTy();
    bool isStruct = castToType->isStructTy();
    bool isPointer = castToType->isPointerTy();

    if (isArray) {
        llvm::ArrayType *arrayType = llvm::dyn_cast<llvm::ArrayType>(castToType);
        int sourceCount = expression->getExpressions().size();
        int targetCount = arrayType->getNumElements();
        int count = targetCount == 0 ? sourceCount : targetCount;

        llvm::Type *elementType = arrayType->getArrayElementType();
        vector<llvm::Constant*> constantValues;
        for (int i=0; i<count; i++) {
            if (i < sourceCount) {
                shared_ptr<Expression> valueExpression = expression->getExpressions().at(i);
                llvm::Constant *constantValue = constantValueForExpression(valueExpression, elementType);
                constantValues.push_back(constantValue);
            } else {
                constantValues.push_back(llvm::Constant::getNullValue(elementType));
            }
        }
        return llvm::ConstantArray::get(llvm::ArrayType::get(elementType, count) , constantValues);
    } else if (isStruct) {
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(castToType);

        vector<llvm::Constant*> constantValues;
        for (int i=0; i<expression->getExpressions().size(); i++) {
            shared_ptr<Expression> valueExpression = expression->getExpressions().at(i);
            llvm::Constant *constantValue = constantValueForExpression(valueExpression, structType->getTypeAtIndex(i));
            constantValues.push_back(constantValue);
        }

        return llvm::ConstantStruct::get(structType, constantValues);
    } else if (isPointer) {
        if (expression->getExpressions().size() != 1) {
            markError(0, 0, "Invalid pointer literal");
            return nullptr;
        }
        llvm::Constant *adrValue = constantValueForExpression(expression->getExpressions().at(0), typeIntPtr);
        return llvm::ConstantExpr::getIntToPtr(adrValue, typePtr);
    }
    
    markError(0, 0, "Invalid type");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForBuiltIn(llvm::Value *parentValue, shared_ptr<ExpressionVariable> parentExpression, shared_ptr<ExpressionVariable> expression) {
    bool isArray = parentValue->getType()->isArrayTy();
    bool isPointer = parentValue->getType()->isPointerTy();

    bool isCount = expression->getIdentifier().compare("count") == 0;
    bool isVal = expression->getIdentifier().compare("val") == 0;
    bool isVadr = expression->getIdentifier().compare("vAdr") == 0;
    bool isAdr = expression->getIdentifier().compare("adr") == 0;

    // Return quickly if not built-in
    if (!isCount && !isVal && !isVadr && !isAdr)
        return nullptr;

    // Figure out opearand for the load operation
    llvm::Value *parentOperand;
    llvm::LoadInst *parentLoad = llvm::dyn_cast<llvm::LoadInst>(parentValue);
    if (parentLoad != nullptr)
        parentOperand = parentLoad->getOperand(0);
    else
        parentOperand = parentValue;

    // Then do the appropriate built-in operation
    if (isArray && isCount) {
        llvm::ArrayType *arrayType = llvm::dyn_cast<llvm::ArrayType>(parentValue->getType());
        return valueForLiteral(ExpressionLiteral::expressionLiteralForUInt(arrayType->getNumElements()));
    } else if (isPointer && isVal) {
        llvm::LoadInst *pointeeLoad = builder->CreateLoad(typePtr, parentOperand);

        shared_ptr<ValueType> pointeeValueType = getPtrType(parentExpression->getIdentifier())->getSubType();
        if (pointeeValueType == nullptr) {
            markError(0, 0, "No type for ptr");
            return nullptr;
        }
        llvm::Type *pointeeType = typeForValueType(pointeeValueType);
        if (pointeeType == nullptr) {
            markError(0, 0, "No type for ptr");
            return nullptr; 
        }

        return valueForSourceValue(pointeeLoad, pointeeType, expression);
    } else if (isPointer && isVadr) {
        llvm::LoadInst *pointeeLoad = (llvm::LoadInst*)builder->CreateLoad(typePtr, parentOperand);
        return builder->CreatePtrToInt(pointeeLoad, typeIntPtr);
    } else if (isAdr) {
        return builder->CreatePtrToInt(parentOperand, typeIntPtr);
    }

    markError(0, 0, "Invalid built-in operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForCast(llvm::Value *sourceValue, shared_ptr<ValueType> targetValueType) {
    // Figure out target type
    llvm::Type *targetType = typeForValueType(targetValueType);
    bool isTargetUInt = false;
    bool isTargetSInt = false;
    bool isTargetFloat = false;
    bool isTargetData = false;
    int targetSize = 0;
    switch (targetValueType->getKind()) {
        case ValueTypeKind::U8:
            isTargetUInt = true;
            targetSize = 8;
            break;
        case ValueTypeKind::U32:
            isTargetUInt = true;
            targetSize = 32;
            break;
        case ValueTypeKind::U64:
            isTargetUInt = true;
            targetSize = 64;
            break;
        case ValueTypeKind::S8:
            isTargetSInt = true;
            targetSize = 8;
            break;
        case ValueTypeKind::S32:
            isTargetSInt = true;
            targetSize = 32;
            break;
        case ValueTypeKind::S64:
            isTargetSInt = true;
            targetSize = 64;
            break;
        case ValueTypeKind::F32:
            isTargetFloat = true;
            targetSize = 32;
            break;
        case ValueTypeKind::F64:
            isTargetFloat = true;
            targetSize = 64;
            break;
        case ValueTypeKind::DATA:
            isTargetData = true;
            targetSize = targetValueType->getValueArg();
            break;
        default:
            markError(0, 0, "Invalid cast");
            return nullptr;
    }

    // Figure out source type
    llvm::Type *sourceType = sourceValue->getType();
    if (llvm::dyn_cast<llvm::AllocaInst>(sourceValue) != nullptr)
        sourceType = llvm::dyn_cast<llvm::AllocaInst>(sourceValue)->getAllocatedType();
    bool isSourceUInt = false;
    bool isSourceSInt = false;
    bool isSourceFloat = false;
    bool isSourceData = false;
    int sourceSize = 0;
    if (sourceType->isIntegerTy()) {
        isSourceUInt = true;
        sourceSize = sourceType->getIntegerBitWidth();
    } else if (sourceType->isFloatTy()) {
        isSourceFloat = true;
        sourceSize = 32;
    } else if (sourceType->isDoubleTy()) {
        isSourceFloat = true;
        sourceSize = 64;
    } else if (sourceType->isArrayTy()) {
        isSourceData = true;
        sourceSize = llvm::dyn_cast<llvm::ArrayType>(sourceType)->getNumElements();
    } else {
        markError(0, 0, "Invalid cast");
        return nullptr;
    }

    // Match source to target
    // int to int TODO: handle sint to uint
    if ((isSourceUInt || isSourceSInt) && (isTargetUInt || isTargetSInt)) {
        return builder->CreateZExtOrTrunc(sourceValue, targetType);
    // uint to float
    } else if (isSourceUInt && isTargetFloat) {
        return builder->CreateUIToFP(sourceValue, targetType);
    // sint to float
    } else if (isSourceSInt && isTargetFloat) {
        return builder->CreateSIToFP(sourceValue, targetType);
    // float to float+
    } else if (isSourceFloat && isTargetFloat && targetSize >= sourceSize) {
        return builder->CreateFPExt(sourceValue, targetType);
    // float to float-
    } else if (isSourceFloat && isTargetFloat && targetSize < sourceSize) {
        return builder->CreateFPTrunc(sourceValue, targetType);
    // float to uint
    } else if (isSourceFloat && isTargetUInt) {
        return builder->CreateFPToUI(sourceValue, targetType);
    // float to sint
    } else if (isSourceFloat && isTargetSInt) {
        return builder->CreateFPToSI(sourceValue, targetType);
    // data to data
    } else if (isSourceData && isTargetData) {
        llvm::LoadInst *sourceLoad = llvm::dyn_cast<llvm::LoadInst>(sourceValue);
        if (sourceLoad != nullptr)
            sourceValue = sourceLoad->getOperand(0);

        llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(sourceType);

        int targetCount = targetValueType->getValueArg();
        int sourceCount = sourceArrayType->getNumElements();
        if (targetCount == 0)
            targetCount = sourceCount;
        int copyCount = min(targetCount, sourceCount);

        llvm::Type *targetMemberType = typeForValueType(targetValueType->getSubType());
        llvm::ArrayType *targetArrayType = llvm::ArrayType::get(targetMemberType, targetCount);
        llvm::Value *targetValue = builder->CreateAlloca(targetArrayType, targetCount);

        for (int i=0; i<copyCount; i++) {
            llvm::Value *index[] = {
                builder->getInt32(0),
                builder->getInt32(i)
            };

            llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceArrayType, sourceValue, index);
            llvm::Value *sourceMemberLoad = builder->CreateLoad(sourceArrayType->getArrayElementType(), sourceMemberPtr);
            llvm::Value *castSourceMember = valueForCast(sourceMemberLoad, targetValueType->getSubType());
            llvm::Value *targetMemberPtr = builder->CreateGEP(targetArrayType, targetValue, index);
            builder->CreateStore(castSourceMember, targetMemberPtr);
        }
        return targetValue;
    } else {
        markError(0, 0, "Invalid cast");
        return nullptr;
    }
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
            // data <- { }
            // copy values from literal expression into an allocated array
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                int sourceCount = valueExpressions.size();
                int targetCount = ((llvm::ArrayType *)targetType)->getNumElements();
                int count = min(sourceCount, targetCount);
                llvm::Type *elementType = ((llvm::ArrayType *)targetType)->getArrayElementType();
                for (int i=0; i<count; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *targetPtr = builder->CreateGEP(targetType, targetValue, index);
                    llvm::Value *sourceValue = valueForExpression(valueExpressions.at(i), elementType);
                    builder->CreateStore(sourceValue, targetPtr);
                }
                break;
            }
            // data <- data
            // copy each value from one allocated array to another allocated array
            case ExpressionKind::VARIABLE:
            // data <- function()
            case ExpressionKind::CALL: 
            // data <- .var
            case ExpressionKind::CHAINED: {
                llvm::Value *sourceValue;
                llvm::Type *sourceType;

                if (valueExpression->getKind() == ExpressionKind::VARIABLE) {
                    shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(valueExpression);
                    sourceValue = getAlloca(expressionVariable->getIdentifier());
                    if (sourceValue == nullptr)
                        return;
                    sourceType = ((llvm::AllocaInst*)sourceValue)->getAllocatedType();
                } else if (valueExpression->getKind() == ExpressionKind::CALL) {
                    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(valueExpression);
                    llvm::Value *sourceExpressionValue = valueForCall(expressionCall);
                    if (sourceExpressionValue == nullptr)
                        return;
                    // store call result in a temporary alloca
                    sourceType = sourceExpressionValue->getType();
                    sourceValue = builder->CreateAlloca(sourceType);
                    builder->CreateStore(sourceExpressionValue, sourceValue);
                } else {
                    shared_ptr<ExpressionChained> expressionChained = dynamic_pointer_cast<ExpressionChained>(valueExpression);
                    sourceValue = valueForChained(expressionChained);
                    sourceType = llvm::dyn_cast<llvm::AllocaInst>(sourceValue)->getAllocatedType();
                }

                if (!sourceType->isArrayTy()) {
                    markError(0, 0, "Not an array type");
                    return;
                }

                // make sure we don't go over the bounds
                int sourceCount = ((llvm::ArrayType *)sourceType)->getNumElements();
                int targetCount = ((llvm::ArrayType *)targetType)->getNumElements();
                int count = min(sourceCount, targetCount);

                for (int i=0; i<count; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };

                    // get pointers for both source and target
                    llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceType, sourceValue, index);
                    llvm::Value *targetMemberPtr = builder->CreateGEP(targetType, targetValue, index);
                    // load value from source pointer
                    llvm::Value *sourceMemberValue = builder->CreateLoad(sourceType->getArrayElementType(), sourceMemberPtr);
                    // and then store it at the target pointer
                    builder->CreateStore(sourceMemberValue, targetMemberPtr);
                }
                break;
            }
            default:
                markError(0, 0, "Invalid assignment to data type");
                return;
        }
    // blob
    } else if (targetType->isStructTy()) {
        switch (valueExpression->getKind()) {
            // blob <- { }
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
            // blob <- blob
            case ExpressionKind::VARIABLE:
            case ExpressionKind::CHAINED:
            // blob <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = valueForExpression(valueExpression);
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            default:
                markError(0, 0, "Invalid assignment");
                break;
        }
    // pointer
    } else if (targetType->isPointerTy()) {
        switch (valueExpression->getKind()) {
            // ptr <- { }
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                if (valueExpressions.size() != 1) {
                    markError(0, 0, "Invalid composite assignment");
                    break;
                }
                llvm::Value *adrValue = valueForExpression(valueExpressions.at(0));
                if (adrValue == nullptr) {
                    markError(0, 0, "Invalid composite assignment");
                    break;
                }
                llvm::Value *sourceValue = builder->CreateIntToPtr(adrValue, typePtr);
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            // ptr <- ptr
            case ExpressionKind::VARIABLE:
            case ExpressionKind::CHAINED:
            // ptr <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = valueForExpression(valueExpression);
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            default:
                markError(0, 0, "Invalid assignment to pointer type");
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
            case ExpressionKind::VARIABLE:
            case ExpressionKind::CHAINED: {
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

bool ModuleBuilder::setPtrType(string name, shared_ptr<ValueType> ptrType) {
    if (scopes.top().ptrTypeMap[name] != nullptr) {
        markError(0, 0, format("Ptr type \"{}\" already declared in scope", name));
        return false;
    }

    scopes.top().ptrTypeMap[name] = ptrType;
    return true;
}

shared_ptr<ValueType> ModuleBuilder::getPtrType(string name) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        shared_ptr<ValueType> ptrType = scopes.top().ptrTypeMap[name];
        if (ptrType != nullptr)
            return ptrType;
        scopes.pop();
    }

    return nullptr;
}

bool ModuleBuilder::setGlobal(string name, llvm::Value *global) {
    if (scopes.top().globalMap[name] != nullptr) {
        markError(0, 0, format("Global \"{}\" already declared in scope", name));
        return false;
    }

    scopes.top().globalMap[name] = global;
    return true;
}

llvm::Value *ModuleBuilder::getGlobal(string name) {
    stack<Scope> scopes = this->scopes;

    while (!scopes.empty()) {
        llvm::Value *global = scopes.top().globalMap[name];
        if (global != nullptr)
            return global;
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
        case ValueTypeKind::F32:
            return typeF32;
        case ValueTypeKind::F64:
            return typeF64;
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
