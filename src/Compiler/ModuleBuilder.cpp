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
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementBlock.h"

ModuleBuilder::ModuleBuilder(
    string moduleName,
    string defaultModuleName,
    int intSize,
    int pointerSize,
    llvm::CallingConv::ID callingConvention,
    vector<shared_ptr<Statement>> statements,
    vector<shared_ptr<Statement>> headerStatements,
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap
):
moduleName(moduleName),
defaultModuleName(defaultModuleName),
statements(statements),
callingConvention(callingConvention),
headerStatements(headerStatements),
exportedHeaderStatementsMap(exportedHeaderStatementsMap) {
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
    scope = make_shared<Scope>();

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
        markModuleError(errorMessage);

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

    return module;
}

//
// Statements
//
void ModuleBuilder::buildStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::META_IMPORT:
            buildImport(dynamic_pointer_cast<StatementImport>(statement));
            break;
        case StatementKind::FUNCTION_DECLARATION: {
            shared_ptr<StatementFunctionDeclaration> statementDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(statement);
            buildFunctionDeclaration(
                moduleName,
                statementDeclaration->getName(),
                statementDeclaration->getShouldExport(),
                statementDeclaration->getArguments(),
                statementDeclaration->getReturnValueType()
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
        case StatementKind::VARIABLE_DECLARATION: {
            shared_ptr<StatementVariableDeclaration> statementDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(statement);
            buildVariableDeclaration(
                moduleName,
                statementDeclaration->getIdentifier(),
                statementDeclaration->getShouldExport(),
                statementDeclaration->getValueType()
            );
            break;
        }
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
            shared_ptr<StatementMetaExternFunction> statementExtern = dynamic_pointer_cast<StatementMetaExternFunction>(statement);
            buildFunctionDeclaration(
                "",
                statementExtern->getName(),
                true,
                statementExtern->getArguments(),
                statementExtern->getReturnValueType()
            );
            break;
        }
        case StatementKind::META_EXTERN_VARIABLE: {
            shared_ptr<StatementMetaExternVariable> statementExtern = dynamic_pointer_cast<StatementMetaExternVariable>(statement);
            buildVariableDeclaration(
                "",
                statementExtern->getIdentifier(),
                true,
                statementExtern->getValueType()
            );
            break;
        }
        case StatementKind::EXPRESSION:
            buildExpression(dynamic_pointer_cast<StatementExpression>(statement));
            return;
        default:
            markError(statement->getLine(), statement->getColumn(), "Unexpected statement");
    }
}

void ModuleBuilder::buildImportStatement(shared_ptr<Statement> statement, string moduleName) {
    switch (statement->getKind()) {
        case StatementKind::FUNCTION_DECLARATION: {
            shared_ptr<StatementFunctionDeclaration> statementDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(statement);
            buildFunctionDeclaration(
                moduleName,
                statementDeclaration->getName(),
                true,
                statementDeclaration->getArguments(),
                statementDeclaration->getReturnValueType()
            );
            break;
        }
        case StatementKind::VARIABLE_DECLARATION: {
        shared_ptr<StatementVariableDeclaration> statementDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(statement);
            buildVariableDeclaration(
                moduleName,
                statementDeclaration->getIdentifier(),
                true,
                statementDeclaration->getValueType()
            );
            break;
        }
        default:
            markError(statement->getLine(), statement->getColumn(), "Unexpected imported statement");
    }
}

void ModuleBuilder::buildImport(shared_ptr<StatementImport> statement) {
    auto it = exportedHeaderStatementsMap.find(statement->getName());
    if (it == exportedHeaderStatementsMap.end()) {
        markError(statement->getLine(), statement->getColumn(), format("Module \"{}\" doesn't exist", statement->getName()));
        return;
    }
    for (shared_ptr<Statement> &importStatement  : it->second) {
        buildImportStatement(importStatement, statement->getName());
    }
}

void ModuleBuilder::buildFunctionDeclaration(string moduleName, string name, bool isExtern, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnType) {    
    // symbol name
    string symbolName = name;
    if (!moduleName.empty() && moduleName.compare(defaultModuleName) != 0)
        symbolName = format("{}.{}", moduleName, name);

    // register
    string internalName = name;
    if (moduleName.compare(this->moduleName) != 0)
        internalName = symbolName;

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
    llvm::Function *fun = llvm::Function::Create(funType, funLinkage, symbolName, *module);
    fun->setCallingConv(callingConvention);

    scope->setFunction(internalName, fun);
}

void ModuleBuilder::buildFunction(shared_ptr<StatementFunction> statement) {
    llvm::Function *fun = scope->getFunction(statement->getName());

    // Check if function not yet defined
    llvm::BasicBlock &entryBlock = fun->getEntryBlock();
    if (entryBlock.getParent() != nullptr) {
        markError(statement->getLine(), statement->getColumn(), format("Function \"{}\" already defined in scope", statement->getName()));
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

    scope->pushLevel();

    // build arguments
    int i=0;
    for (auto &arg : fun->args()) {
        string name = statement->getArguments()[i].first;
        llvm::Type *type = argTypes[i];
        arg.setName(name);

        if (type->isPointerTy()) {
            shared_ptr<ValueType> valueType = statement->getArguments().at(i).second;
            if(!scope->setPtrType(name, valueType))
                return;
        }

        llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr, name);
        if (!scope->setAlloca(name, alloca))
            return;
        builder->CreateStore(&arg, alloca);

        i++;
    }

    // build function body
    buildStatement(statement->getStatementBlock());
    // Remove extranouse block after the last return statement
    builder->GetInsertBlock()->eraseFromParent();

    scope->popLevel();

    builder->SetInsertPoint((llvm::BasicBlock*)nullptr);

    // verify function
    string errorMessage;
    llvm::raw_string_ostream llvmErrorMessage(errorMessage);
    if (llvm::verifyFunction(*fun, &llvmErrorMessage))
        markFunctionError(statement->getName(), errorMessage);
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
        markError(statement->getLine(), statement->getColumn(), format("Constraints \"{}\", are invalid", statement->getConstraints()));
        return;
    }
    llvm::InlineAsm *rawFun = llvm::InlineAsm::get(funType, statement->getRawSource(), statement->getConstraints(), true, false, llvm::InlineAsm::AsmDialect::AD_Intel);

    scope->setInlineAsm(statement->getName(), rawFun);
}

void ModuleBuilder::buildBlobDeclaration(shared_ptr<StatementBlobDeclaration> statement) {
    llvm::StructType::create(*context, statement->getName());
}

void ModuleBuilder::buildBlob(shared_ptr<StatementBlob> statement) {
    llvm::StructType *structType = llvm::StructType::getTypeByName(*context, statement->getName());
    if (structType == nullptr) {
        markError(statement->getLine(), statement->getColumn(), format("Blob \"{}\" not declared", statement->getName()));
        return;
    }

    // Generate types for body
    vector<string> memberNames;
    vector<llvm::Type *> types;
    for (pair<string, shared_ptr<ValueType>> &variable: statement->getMembers()) {
        memberNames.push_back(variable.first);
        llvm::Type *type = typeForValueType(variable.second);
        if (type == nullptr)
            return;
        types.push_back(type);
    }
    structType->setBody(types, false);
    scope->setStruct(statement->getName(), structType, memberNames);
}

void ModuleBuilder::buildVariableDeclaration(string moduleName, string name, bool isExtern, shared_ptr<ValueType> valueType) {
    // symbol name
    string symbolName = name;
    if (!moduleName.empty() && moduleName.compare(defaultModuleName) != 0)
        symbolName = format("{}.{}", moduleName, symbolName);

    // internal name
    string internalName = name;
    if (moduleName.compare(this->moduleName) != 0)
        internalName = symbolName;

    // type
    llvm::Type *type = typeForValueType(valueType, 0);
    if (type == nullptr)
        return;

    // linkage
    llvm::GlobalValue::LinkageTypes linkage = isExtern ?
        linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage :
        llvm::GlobalValue::LinkageTypes::InternalLinkage;

    llvm::GlobalVariable *global = new llvm::GlobalVariable(*module, type, false, linkage, nullptr, symbolName);

    // register
    scope->setGlobal(internalName, global);
    scope->setPtrType(internalName, valueType);
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
                        llvm::AllocaInst *alloca = scope->getAlloca(identifier);
                        if (alloca != nullptr && alloca->getAllocatedType()->isArrayTy()) {
                            count = ((llvm::ArrayType*)alloca->getAllocatedType())->getNumElements();
                        }
                        break;
                    }
                    case ExpressionKind::CALL: {
                        string funName = dynamic_pointer_cast<ExpressionCall>(statement->getExpression())->getName();
                        llvm::Function *fun = scope->getFunction(funName);
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
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getIdentifier());
            break;
        }
        case ValueTypeKind::BLOB: {
            valueType = (llvm::StructType *)typeForValueType(statement->getValueType(), 0);
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getIdentifier());
            break;
        }
        case ValueTypeKind::PTR: {
            valueType = (llvm::PointerType *)typeForValueType(statement->getValueType(), 0);
            if (valueType == nullptr)
                return;
            if (!scope->setPtrType(statement->getIdentifier(), statement->getValueType()))
                return;
            alloca = builder->CreateAlloca(valueType, nullptr, statement->getIdentifier());
            break;
        }
        default: {
            valueType = typeForValueType(statement->getValueType());
            if (valueType == nullptr)
                return;
            alloca = builder->CreateAlloca(valueType, 0, nullptr, statement->getIdentifier());
        }
    }

    // try registering new variable in scope
    if (!scope->setAlloca(statement->getIdentifier(), alloca))
        return;

    if (statement->getExpression() != nullptr)
        buildAssignment(alloca, valueType, statement->getExpression());
}

void ModuleBuilder::buildGlobalVariable(shared_ptr<StatementVariable> statement) {
    // variable
    llvm::GlobalVariable *global = (llvm::GlobalVariable*)scope->getGlobal(statement->getIdentifier());

    if (global->hasInitializer()) {
        markError(statement->getLine(), statement->getColumn(), format("Global \"{}\" already defined in scope", statement->getIdentifier()));
        return;
    }

    // type
    shared_ptr<ValueType> valueType = scope->getPtrType(statement->getIdentifier());
    llvm::Type *type = typeForValueType(valueType);

    // initialization
    llvm::Constant *initConstant = llvm::Constant::getNullValue(type);
    if (statement->getExpression() != nullptr) {
        initConstant = constantValueForExpression(statement->getExpression(), type);
        if (initConstant == nullptr) {
            markError(statement->getLine(), statement->getColumn(), "Not a constant expression");
            return;
        }
    }

    global->setInitializer(initConstant);
}

void ModuleBuilder::buildAssignmentChained(shared_ptr<StatementAssignment> statement) {
    llvm::Value *targetValue = valueForChainExpressions(statement->getChainExpressions());
    if (targetValue == nullptr)
        return;

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
    for (shared_ptr<Statement> &innerStatement : statement->getStatements()) {
        buildStatement(innerStatement);
        // skip any statements after a retrun (they wont' get exectuted anyway)
        if (innerStatement->getKind() == StatementKind::RETURN)
            return;
    }
}

void ModuleBuilder::buildReturn(shared_ptr<StatementReturn> statement) {
    llvm::BasicBlock *basicBlock = builder->GetInsertBlock();

    if (statement->getExpression() != nullptr) {
        llvm::Type *returnType = basicBlock->getParent()->getReturnType();
        llvm::Value *returnValue = valueForExpression(statement->getExpression(), returnType);
        builder->CreateRet(returnValue);
    } else {
        builder->CreateRetVoid();
    }

    // Create a new block in case the return is not the last statement
    llvm::BasicBlock *afterReturnBlock = llvm::BasicBlock::Create(*context, "afterReturnBlock");
    llvm::Function *fun = basicBlock->getParent();
    fun->insert(fun->end(), afterReturnBlock);
    builder->SetInsertPoint(afterReturnBlock);
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

    scope->pushLevel();

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

    scope->popLevel();
}

void ModuleBuilder::buildExpression(shared_ptr<StatementExpression> statement) {
    valueForExpression(statement->getExpression());
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
                    sourceValue = scope->getAlloca(expressionVariable->getIdentifier());
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

//
// Expressions
//
llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<Expression> expression, llvm::Type *castToType) {
    switch (expression->getKind()) {
        case ExpressionKind::NONE:
            return llvm::UndefValue::get(typeVoid);
        case ExpressionKind::LITERAL:
            return valueForLiteral(dynamic_pointer_cast<ExpressionLiteral>(expression), castToType);
        case ExpressionKind::COMPOSITE_LITERAL:
            return valueForCompositeLiteral(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression), castToType);
        case ExpressionKind::GROUPING:
            return valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getSubExpression());
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
            markError(expression->getLine(), expression->getColumn(), "Unexpected expression");
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
            value = valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression)->getSubExpression());
            break;
        case ExpressionKind::BINARY:
            value = valueForBinary(dynamic_pointer_cast<ExpressionBinary>(expression));
            break;
        case ExpressionKind::UNARY:
            value = valueForUnary(dynamic_pointer_cast<ExpressionUnary>(expression));
            break;
        default:
            markError(expression->getLine(), expression->getColumn(), "Invalid constant expression");
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
    return valueForExpression(expression->getSubExpression());
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

    markError(expression->getLine(), expression->getColumn(), "Unexpected binary operation");
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
    llvm::Value *value = valueForExpression(expression->getSubExpression());
    if (value == nullptr)
        return nullptr;
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

    markError(expression->getLine(), expression->getColumn(), "Unexpected unary operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForIfElse(shared_ptr<ExpressionIfElse> expression) {
    shared_ptr<Expression> conditionExpression = expression->getConditionExpression();

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
    scope->pushLevel();
    builder->SetInsertPoint(thenBlock);
    buildStatement(expression->getThenBlockExpression()->getStatementBlock());
    llvm::Value *thenValue = valueForExpression(expression->getThenBlockExpression()->getResultStatementExpression()->getExpression());
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();
    scope->popLevel();

    // Else
    llvm::Value *elseValue = nullptr;
    if (expression->getElseExpression() != nullptr) {
        scope->pushLevel();
        fun->insert(fun->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);
        elseValue = valueForExpression(expression->getElseExpression());
        builder->CreateBr(mergeBlock);
        elseBlock = builder->GetInsertBlock();
        scope->popLevel();
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

    llvm::AllocaInst *localAlloca = scope->getAlloca(expression->getIdentifier());
    llvm::Value *globalValuePtr = scope->getGlobal(expression->getIdentifier());
    llvm::Value *fun = scope->getFunction(expression->getIdentifier());
    if (localAlloca != nullptr) {
        value = localAlloca;
        type = localAlloca->getAllocatedType();
    } else if (globalValuePtr != nullptr) {
        shared_ptr<ValueType> valueType = scope->getPtrType(expression->getIdentifier());
        type = typeForValueType(valueType);
        value = globalValuePtr;
    } else if (fun != nullptr) {
        type = fun->getType();
        value = fun;
    }

    if (value == nullptr) {
        markError(expression->getLine(), expression->getColumn(), format("Variable \"{}\" not defined in scope", expression->getIdentifier()));
        return nullptr;
    }

    return valueForSourceValue(value, type, expression);
}

llvm::Value *ModuleBuilder::valueForCall(shared_ptr<ExpressionCall> expression) {
    llvm::Function *fun = scope->getFunction(expression->getName());
    if (fun != nullptr) {
        return valueForCall(fun, fun->getFunctionType(), expression);
    }

    llvm::InlineAsm *rawFun = scope->getInlineAsm(expression->getName());
    if (rawFun != nullptr) {
        vector<llvm::Value *>argValues;
        for (shared_ptr<Expression> &argumentExpression : expression->getArgumentExpressions()) {
            llvm::Value *argValue = valueForExpression(argumentExpression);
            argValues.push_back(argValue);
        }
        return builder->CreateCall(rawFun, llvm::ArrayRef(argValues));
    }

    markError(expression->getLine(), expression->getColumn(), format("Function \"{}\" not defined in scope", expression->getName()));
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForCall(llvm::Value *fun, llvm::FunctionType *funType, shared_ptr<ExpressionCall> expression) {
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

        // If the first expression is a cast, try doing a built-in on a type
        if (currentValue == nullptr && chainExpression->getKind() == ExpressionKind::CAST && chainExpressions.size() >= 2) {
            llvm::Type *type = typeForValueType(chainExpression->getValueType());
            shared_ptr<ExpressionVariable> childExpressionVariable = dynamic_pointer_cast<ExpressionVariable>(chainExpressions.at(++i));
            currentValue = valueForTypeBuiltIn(type, childExpressionVariable);
            continue;
        }

        // First in chain is treated as a single variable
        if (currentValue == nullptr) {
            currentValue = valueForExpression(chainExpression);
            if (currentValue == nullptr)
                return nullptr;
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

        // Check parent expression
        shared_ptr<ExpressionVariable> parentExpressionVariable = dynamic_pointer_cast<ExpressionVariable>(chainExpressions.at(i-1));
        if (parentExpressionVariable == nullptr) {
            markError(parentExpressionVariable->getLine(), parentExpressionVariable->getColumn(), "Invalid expression type");
            return nullptr;
        }

        // Built-in expression?
        llvm::Value *builtInValue = valueForBuiltIn(currentValue, parentExpressionVariable, chainExpression);
        if (builtInValue != nullptr) {
            currentValue = builtInValue;
            continue;
        }

        // Check chained expression type 
        shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(chainExpression);
        if (expressionVariable == nullptr) {
            markError(expressionVariable->getLine(), expressionVariable->getColumn(), "Invalid expression type");
            return nullptr;
        }

        // Variable expression?
        llvm::StructType *structType = (llvm::StructType*)currentValue->getType();
        if (!structType->isStructTy()) {
            markError(0, 0, "Something's fucky");
            return nullptr;
        }
        string structName = string(structType->getName());
        optional<int> memberIndex = scope->getStructMemberIndex(structName, expressionVariable->getIdentifier());
        if (!memberIndex) {
            markError(0, 0, format("Invalid member \"{}\" for \"blob<{}>\"", expressionVariable->getIdentifier(), structName));
            return nullptr;
        }
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

llvm::Value *ModuleBuilder::valueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType, shared_ptr<Expression> expression) {
    if (builder->GetInsertBlock() == nullptr)
        return nullptr;

    shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(expression);
    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression);

    if (expressionVariable != nullptr) {
        switch (expressionVariable->getVariableKind()) {
            case ExpressionVariableKind::SIMPLE: {
                return builder->CreateLoad(sourceType, sourceValue, expressionVariable->getIdentifier());
            }
            case ExpressionVariableKind::DATA: {
                llvm::Value *indexValue = valueForExpression(expressionVariable->getIndexExpression());
                llvm::Value *index[] = {
                    builder->getInt32(0),
                    indexValue
                };
                llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(sourceType);
                llvm::Value *elementPtr = builder->CreateGEP(sourceArrayType, sourceValue, index, format("{}[]", expressionVariable->getIdentifier()));
                return builder->CreateLoad(sourceArrayType->getArrayElementType(), elementPtr);
            }
        }
    } else if (expressionCall != nullptr) {
        llvm::FunctionType *funType = llvm::dyn_cast<llvm::FunctionType>(sourceType);
        return valueForCall(sourceValue, funType, expressionCall);
    }
}

llvm::Value *ModuleBuilder::valueForCompositeLiteral(shared_ptr<ExpressionCompositeLiteral> expression, llvm::Type *castToType) {
    if (castToType == nullptr) {
        markError(expression->getLine(), expression->getColumn(), "Don't know what to do with the composite");
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

llvm::Value *ModuleBuilder::valueForBuiltIn(llvm::Value *parentValue, shared_ptr<ExpressionVariable> parentExpression, shared_ptr<Expression> expression) {
    bool isArray = parentValue->getType()->isArrayTy();
    bool isPointer = parentValue->getType()->isPointerTy();

    bool isCount = false;
    bool isVal = false;
    bool isVadr = false;
    bool isAdr = false;
    bool isSize = false;

    shared_ptr<ExpressionVariable> expressionVariable = dynamic_pointer_cast<ExpressionVariable>(expression);
    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression);

    if (expressionVariable != nullptr) {
        isCount = expressionVariable->getIdentifier().compare("count") == 0;
        isVal = expressionVariable->getIdentifier().compare("val") == 0;
        isVadr = expressionVariable->getIdentifier().compare("vAdr") == 0;
        isAdr = expressionVariable->getIdentifier().compare("adr") == 0;
        isSize = expressionVariable->getIdentifier().compare("size") == 0;
    } else if (expressionCall != nullptr) {
        isVal = expressionCall->getName().compare("val") == 0;
    }

    // Return quickly if not built-in
    if (!isCount && !isVal && !isVadr && !isAdr && !isSize)
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
        return valueForLiteral(ExpressionLiteral::expressionLiteralForUInt(arrayType->getNumElements(), 0, 0));
    } else if (isPointer && isVal) {
        llvm::LoadInst *pointeeLoad = builder->CreateLoad(typePtr, parentOperand);

        shared_ptr<ValueType> pointeeValueType = scope->getPtrType(parentExpression->getIdentifier())->getSubType();
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
    } else if (isSize) {
        int sizeInBytes = sizeInBitsForType(parentValue->getType()) / 8;
        if (sizeInBytes > 0)
            return llvm::ConstantInt::get(typeUInt, sizeInBytes);
        else 
            return nullptr;
    }

    markError(0, 0, "Invalid built-in operation");
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionVariable> expression) {
    bool isSize = expression->getIdentifier().compare("size") == 0;

    if (isSize) {
        int sizeInBytes = sizeInBitsForType(type) / 8;
        if (sizeInBytes > 0)
            return llvm::ConstantInt::get(typeUInt, sizeInBytes);
        else 
            return nullptr;    
    }
    
    markError(expression->getLine(), expression->getColumn(), "Invalid built-in operation");
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

//
// Support
//
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
            return scope->getStructType(valueType->getBlobName());
        case ValueTypeKind::FUN: {
            // returnType
            llvm::Type *functionReturnType = typeForValueType(valueType->getReturnType());

            // argument types
            vector<llvm::Type *> functionArgumentTypes;
                for (shared_ptr<ValueType> &argumentType : valueType->getArgumentTypes()) {
                    llvm::Type *functionArgumentType = typeForValueType(argumentType);
                        if (functionArgumentType == nullptr)
                            return nullptr;
                        functionArgumentTypes.push_back(functionArgumentType);
                }

            return llvm::FunctionType::get(functionReturnType, functionArgumentTypes, false);
        }
        case ValueTypeKind::PTR:
            return typePtr;
        default:
            return nullptr;
    }
}

int ModuleBuilder::sizeInBitsForType(llvm::Type *type) {
    if (type->isIntegerTy()) {
        llvm::IntegerType *integerType = llvm::dyn_cast<llvm::IntegerType>(type);
        return max((int)integerType->getIntegerBitWidth(), 8);
    } else if (type->isFloatTy()) {
        return 32;
    } else if (type->isDoubleTy()) {
        return 64;
    } else if (type->isPointerTy()) {
        return typeIntPtr->getBitWidth();
    } else if (type->isArrayTy()) {
        llvm::ArrayType *arrayType = llvm::dyn_cast<llvm::ArrayType>(type);
        int elementsCount = arrayType->getNumElements();
        llvm::Type *elementType = arrayType->getElementType();
        int elementSize = sizeInBitsForType(elementType);
        return elementSize * elementsCount;
    } else if (type->isStructTy()) {
        int size = 0;
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(type);
        int elementsCount = structType->getNumElements();
        for (int i=0; i<elementsCount; i++) {
            llvm::Type *elementType = structType->getElementType(i);
            int elementSize = sizeInBitsForType(elementType);
            size += elementSize;
        }
        return size;
    }

    return 0;
}

void ModuleBuilder::markError(int line, int column, string message) {
    errors.push_back(Error::builderError(line, column, message));
}

void ModuleBuilder::markFunctionError(string functionName, string message) {
    errors.push_back(Error::builderFunctionError(functionName, message));
}

void ModuleBuilder::markModuleError(string message) {
    errors.push_back(Error::builderModuleError(moduleName, message));
}
