#include "ModuleBuilder.h"

#include "Error.h"
#include "Logger.h"
#include "WrappedValue.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionValue.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCast.h"

#include "Parser/Statement/StatementMetaImport.h"
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
    map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap
):
moduleName(moduleName),
defaultModuleName(defaultModuleName),
callingConvention(callingConvention),
statements(statements),
headerStatements(headerStatements),
importableHeaderStatementsMap(importableHeaderStatementsMap) {
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

    // build just the import statements
    for (shared_ptr<Statement> statement : statements) {
        if (statement->getKind() == StatementKind::META_IMPORT)
            buildStatement(statement);
    }

    // build header
    for (shared_ptr<Statement> &headerStatement : headerStatements)
        buildStatement(headerStatement);

    // build statements other than import
    for (shared_ptr<Statement> &statement : statements) {
        if (statement->getKind() != StatementKind::META_IMPORT)
            buildStatement(statement);
    }

    // verify module
    string errorMessage;
    llvm::raw_string_ostream llvmErrorMessage(errorMessage);
    if (llvm::verifyModule(*module, &llvmErrorMessage)) {
        if (errorMessage.at(errorMessage.length() - 1) == '\n')
            errorMessage = errorMessage.substr(0, errorMessage.length() - 1);
        markModuleError(errorMessage);
    }

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
        case StatementKind::ASSIGNMENT:
            buildStatement(dynamic_pointer_cast<StatementAssignment>(statement));
            break;
        case StatementKind::BLOB:
            buildStatement(dynamic_pointer_cast<StatementBlob>(statement));
            break;
        case StatementKind::BLOB_DECLARATION:
            buildStatement(dynamic_pointer_cast<StatementBlobDeclaration>(statement));
            break;
        case StatementKind::BLOCK:
            buildStatement(dynamic_pointer_cast<StatementBlock>(statement));
            break;
        case StatementKind::EXPRESSION:
            buildStatement(dynamic_pointer_cast<StatementExpression>(statement));
            break;
        case StatementKind::FUNCTION:
            buildStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::FUNCTION_DECLARATION:
            buildStatement(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            break;
        case StatementKind::META_EXTERN_FUNCTION:
            buildStatement(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
            break;
        case StatementKind::META_EXTERN_VARIABLE:
            buildStatement(dynamic_pointer_cast<StatementMetaExternVariable>(statement));
            break;
        case StatementKind::META_IMPORT:
            buildStatement(dynamic_pointer_cast<StatementMetaImport>(statement));
            break;
        case StatementKind::RAW_FUNCTION:
            buildStatement(dynamic_pointer_cast<StatementRawFunction>(statement));
            break;
        case StatementKind::REPEAT:
            buildStatement(dynamic_pointer_cast<StatementRepeat>(statement));
            break;
        case StatementKind::RETURN:
            buildStatement(dynamic_pointer_cast<StatementReturn>(statement));
            break;
        case StatementKind::VARIABLE:
            buildStatement(dynamic_pointer_cast<StatementVariable>(statement));
            break;
        case StatementKind::VARIABLE_DECLARATION:
            buildStatement(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            break;
        default:
            markError(statement->getLine(), statement->getColumn(), "Unexpected statement");
    }
}

void ModuleBuilder::buildStatement(shared_ptr<StatementAssignment> statementAssignment) {
    llvm::Value *targetValue = valueForExpression(statementAssignment->getExpressionChained());
    if (targetValue == nullptr)
        return;

    // Figure out opearand for the store operation
    llvm::Value *targetOperand;
    llvm::LoadInst *targetLoad = llvm::dyn_cast<llvm::LoadInst>(targetValue);
    if (targetLoad != nullptr)
        targetOperand = targetLoad->getOperand(0);
    else
        targetOperand = targetValue;

    buildAssignment(targetOperand, targetValue->getType(), statementAssignment->getValueExpression());
}

void ModuleBuilder::buildStatement(shared_ptr<StatementBlob> statementBlob) {
    buildBlobDefinition(
        moduleName,
        statementBlob->getName(),
        statementBlob->getMembers()
    );
}

void ModuleBuilder::buildStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration) {
    buildBlobDeclaration(
        moduleName,
        statementBlobDeclaration->getName()
    );
}

void ModuleBuilder::buildStatement(shared_ptr<StatementBlock> statementBlock) {
    for (shared_ptr<Statement> &innerStatement : statementBlock->getStatements()) {
        buildStatement(innerStatement);
        // skip any statements after a retrun (they wont' get exectuted anyway)
        if (innerStatement->getKind() == StatementKind::RETURN)
            return;
    }
}

void ModuleBuilder::buildStatement(shared_ptr<StatementExpression> statementExpression) {
    // ignore result
    valueForExpression(statementExpression->getExpression());
}

void ModuleBuilder::buildStatement(shared_ptr<StatementFunction> statementFunction) {
    // Check if declared
    llvm::Function *fun = scope->getFunction(statementFunction->getName());
    if (fun == nullptr) {
        markErrorNotDeclared(statementFunction->getLine(), statementFunction->getColumn(), format("function \"{}\"", statementFunction->getName()));
        return;
    }

    // Check if function not yet defined
    llvm::BasicBlock &entryBlock = fun->getEntryBlock();
    if (entryBlock.getParent() != nullptr) {
        markErrorAlreadyDefined(statementFunction->getLine(), statementFunction->getColumn(), format("function \"{}\"", statementFunction->getName()));
        return;
    }

    // define function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, statementFunction->getName(), fun);
    builder->SetInsertPoint(block);

    scope->pushLevel();

    // build arguments
    for (int i=0; i<statementFunction->getArguments().size(); i++) {
        pair<string, shared_ptr<ValueType>> argument = statementFunction->getArguments().at(i);

        llvm::Argument *funArgument = fun->getArg(i);
        funArgument->setName(argument.first);

        llvm::Type *funArgumentType = typeForValueType(argument.second);
        if (funArgumentType == nullptr)
            return;
        llvm::AllocaInst *alloca = builder->CreateAlloca(funArgumentType, nullptr, argument.first);
        if (!scope->setAlloca(argument.first, alloca)) {
            // TODO: mark error
            return;
        }
        builder->CreateStore(funArgument, alloca);
    }

    // build function body
    buildStatement(statementFunction->getStatementBlock());
    // Remove extranouse block after the last return statement
    builder->GetInsertBlock()->eraseFromParent();

    scope->popLevel();

    builder->SetInsertPoint((llvm::BasicBlock*)nullptr);

    // verify function
    string errorMessage;
    llvm::raw_string_ostream llvmErrorMessage(errorMessage);
    if (llvm::verifyFunction(*fun, &llvmErrorMessage)) {
        if (errorMessage.at(errorMessage.length() - 1) == '\n')
            errorMessage = errorMessage.substr(0, errorMessage.length() - 1);
        markFunctionError(statementFunction->getName(), errorMessage);
    }
}

void ModuleBuilder::buildStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration) {
    buildFunctionDeclaration(
        moduleName,
        statementFunctionDeclaration->getName(),
        statementFunctionDeclaration->getShouldExport(),
        statementFunctionDeclaration->getArguments(),
        statementFunctionDeclaration->getReturnValueType()
    );
}

void ModuleBuilder::buildStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction) {
    buildFunctionDeclaration(
        "",
        statementMetaExternFunction->getName(),
        true,
        statementMetaExternFunction->getArguments(),
        statementMetaExternFunction->getReturnValueType()
    );
}

void ModuleBuilder::buildStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable) {
    buildVariableDeclaration(
        "",
        statementMetaExternVariable->getIdentifier(),
        true,
        statementMetaExternVariable->getValueType()
    );
}

void ModuleBuilder::buildStatement(shared_ptr<StatementMetaImport> statementMetaImport) {
    auto it = importableHeaderStatementsMap.find(statementMetaImport->getName());
    if (it == importableHeaderStatementsMap.end()) {
        markErrorNotDefined(statementMetaImport->getLine(), statementMetaImport->getColumn(), format("module \"{}\"", statementMetaImport->getName()));
        return;
    }

    for (shared_ptr<Statement> &importedStatement : it->second) {
        switch (importedStatement->getKind()) {
            case StatementKind::FUNCTION_DECLARATION: {
                shared_ptr<StatementFunctionDeclaration> statementDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(importedStatement);
                buildFunctionDeclaration(
                    statementMetaImport->getName(),
                    statementDeclaration->getName(),
                    true,
                    statementDeclaration->getArguments(),
                    statementDeclaration->getReturnValueType()
                );
                break;
            }
            case StatementKind::VARIABLE_DECLARATION: {
                shared_ptr<StatementVariableDeclaration> statementDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(importedStatement);
                buildVariableDeclaration(
                    statementMetaImport->getName(),
                    statementDeclaration->getIdentifier(),
                    true,
                    statementDeclaration->getValueType()
                );
                break;
            }
            case StatementKind::BLOB_DECLARATION: {
                shared_ptr<StatementBlobDeclaration> statementDeclaration = dynamic_pointer_cast<StatementBlobDeclaration>(importedStatement);
                buildBlobDeclaration(
                    statementMetaImport->getName(),
                    statementDeclaration->getName()
                );
                break;
            }
            case StatementKind::BLOB: {
                shared_ptr<StatementBlob> statementBlobDefinition = dynamic_pointer_cast<StatementBlob>(importedStatement);
                buildBlobDefinition(
                    statementMetaImport->getName(),
                    statementBlobDefinition->getName(),
                    statementBlobDefinition->getMembers()
                );
                break;
            }
            default:
                markError(importedStatement->getLine(), importedStatement->getColumn(), "Unexpected imported statement");
        }
    }
}

void ModuleBuilder::buildStatement(shared_ptr<StatementRawFunction> statementRawFunction) {
    // function types
    llvm::Type *funReturnType = typeForValueType(statementRawFunction->getReturnValueType());
    if (funReturnType == nullptr)
        return;

    vector<llvm::Type *> funArgumentTypes;
    for (pair<string, shared_ptr<ValueType>> &argument : statementRawFunction->getArguments()) {
        llvm::Type *funArgumentType = typeForValueType(argument.second);
        if (funArgumentType == nullptr)
            return;
        funArgumentTypes.push_back(funArgumentType);
    }

    // build function declaration & body
    llvm::FunctionType *funType = llvm::FunctionType::get(funReturnType, funArgumentTypes, false);
    if(llvm::InlineAsm::verify(funType, statementRawFunction->getConstraints())) {
        markInvalidConstraints(
            statementRawFunction->getLine(),
            statementRawFunction->getColumn(),
            statementRawFunction->getName(),
            statementRawFunction->getConstraints()
        );
        return;
    }
    llvm::InlineAsm *rawFun = llvm::InlineAsm::get(
        funType,
        statementRawFunction->getRawSource(),
        statementRawFunction->getConstraints(),
        true,
        false,
        llvm::InlineAsm::AsmDialect::AD_Intel
    );

    scope->setInlineAsm(statementRawFunction->getName(), rawFun);
}

void ModuleBuilder::buildStatement(shared_ptr<StatementRepeat> statementRepeat) {
    shared_ptr<Statement> initStatement = statementRepeat->getInitStatement();
    shared_ptr<Statement> postStatement = statementRepeat->getPostStatement();
    shared_ptr<StatementBlock> bodyStatement = statementRepeat->getBodyBlockStatement();
    shared_ptr<Expression> preExpression = statementRepeat->getPreConditionExpression();
    shared_ptr<Expression> postExpression = statementRepeat->getPostConditionExpression();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *preBlock = llvm::BasicBlock::Create(*context, "loopPre", fun);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(*context, "loopBody");
    llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(*context, "loopPost");

    scope->pushLevel();

    // loop init
    if (initStatement != nullptr)
        buildStatement(statementRepeat->getInitStatement());
    
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

    buildStatement(bodyStatement);

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

void ModuleBuilder::buildStatement(shared_ptr<StatementReturn> statementReturn) {
    llvm::BasicBlock *basicBlock = builder->GetInsertBlock();

    if (!statementReturn->getExpression()->getValueType()->isEqual(ValueType::NONE)) {
        llvm::Value *returnValue = valueForExpression(statementReturn->getExpression());
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

void ModuleBuilder::buildStatement(shared_ptr<StatementVariable> statementVariable) {
    if (builder->GetInsertBlock() != nullptr)
        buildLocalVariable(statementVariable);
    else
        buildGlobalVariable(statementVariable);
}

void ModuleBuilder::buildStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    buildVariableDeclaration(
        moduleName,
        statementVariableDeclaration->getIdentifier(),
        statementVariableDeclaration->getShouldExport(),
        statementVariableDeclaration->getValueType()
    );
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
    llvm::Type *type = typeForValueType(valueType);
    if (type == nullptr)
        return;

    // linkage
    llvm::GlobalValue::LinkageTypes linkage = isExtern ?
        linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage :
        llvm::GlobalValue::LinkageTypes::InternalLinkage;

    llvm::GlobalVariable *global = new llvm::GlobalVariable(*module, type, false, linkage, nullptr, symbolName);

    // register
    scope->setGlobal(internalName, global);
}

void ModuleBuilder::buildBlobDeclaration(string moduleName, string name) {
    // symbol name
    string symbolName = name;
    if (!moduleName.empty() && moduleName.compare(defaultModuleName) != 0)
        symbolName = format("{}.{}", moduleName, symbolName);

    // internal name
    string internalName = name;
    if (moduleName.compare(this->moduleName) != 0)
        internalName = symbolName;

    llvm::StructType *structType = llvm::StructType::create(*context, symbolName);
    scope->setStruct(internalName, structType, {});
}

void ModuleBuilder::buildBlobDefinition(string moduleName, string name, vector<pair<string, shared_ptr<ValueType>>> members) {
    // symbol name
    string symbolName = name;
    if (!moduleName.empty() && moduleName.compare(defaultModuleName) != 0)
        symbolName = format("{}.{}", moduleName, symbolName);

    // internal name
    string internalName = name;
    if (moduleName.compare(this->moduleName) != 0)
        internalName = symbolName;

    llvm::StructType *structType = scope->getStructType(internalName);
    if (structType == nullptr) {
        markError(0, 0, format("Blob \"{}\" not declared", symbolName));
        return;
    }

    // Generate types for body
    vector<string> memberNames;
    vector<llvm::Type *> types;
    for (pair<string, shared_ptr<ValueType>> &member: members) {
        memberNames.push_back(member.first);
        llvm::Type *type = typeForValueType(member.second);
        if (type == nullptr)
            return;
        types.push_back(type);
    }
    structType->setBody(types, false);
    scope->setStruct(internalName, structType, memberNames);
}

void ModuleBuilder::buildLocalVariable(shared_ptr<StatementVariable> statement) {
    llvm::Type *valueType = typeForValueType(statement->getValueType());
    llvm::AllocaInst *alloca = builder->CreateAlloca(valueType, nullptr, statement->getIdentifier());

    // try registering new variable in scope
    if (!scope->setAlloca(statement->getIdentifier(), alloca))
        return;

    if (statement->getExpression() != nullptr)
        buildAssignment(alloca, valueType, statement->getExpression());
}

void ModuleBuilder::buildGlobalVariable(shared_ptr<StatementVariable> statement) {
    // variable
    llvm::GlobalVariable *global = (llvm::GlobalVariable*)scope->getGlobal(statement->getIdentifier());
    if (global == nullptr) {
        markError(statement->getLine(), statement->getColumn(), format("{} is not valid", statement->getIdentifier()));
        return;
    }

    if (global->hasInitializer()) {
        markError(statement->getLine(), statement->getColumn(), format("Global \"{}\" already defined in scope", statement->getIdentifier()));
        return;
    }

    // initialization
    llvm::Type *type = typeForValueType(statement->getValueType());
    llvm::Constant *constantValue = llvm::Constant::getNullValue(type);
    if (statement->getExpression() != nullptr) {
        llvm::Value *value = valueForExpression(statement->getExpression());
        if (value == nullptr)
            return;
        constantValue = llvm::dyn_cast<llvm::Constant>(value);
        if (constantValue == nullptr) {
            markError(statement->getLine(), statement->getColumn(), "not a constant");
            return;
        }
    }

    global->setInitializer(constantValue);
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
                for (int i=0; i<count; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *targetPtr = builder->CreateGEP(targetType, targetValue, index);
                    llvm::Value *sourceValue = valueForExpression(valueExpressions.at(i));
                    if (sourceValue == nullptr)
                        return;
                    builder->CreateStore(sourceValue, targetPtr);
                }
                break;
            }
            // data <- data
            // copy each value from one allocated array to another allocated array
            case ExpressionKind::VALUE:
            // data <- function()
            case ExpressionKind::CALL: 
            // data <- .var
            case ExpressionKind::CHAINED: {
                llvm::Value *sourceValue;
                llvm::Type *sourceType;

                if (valueExpression->getKind() == ExpressionKind::VALUE) {
                    shared_ptr<ExpressionValue> expressionVariable = dynamic_pointer_cast<ExpressionValue>(valueExpression);
                    sourceValue = scope->getAlloca(expressionVariable->getIdentifier());
                    if (sourceValue == nullptr)
                        return;
                    sourceType = ((llvm::AllocaInst*)sourceValue)->getAllocatedType();
                } else if (valueExpression->getKind() == ExpressionKind::CALL) {
                    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(valueExpression);
                    llvm::Value *sourceExpressionValue = valueForExpression(expressionCall);
                    if (sourceExpressionValue == nullptr)
                        return;
                    // store call result in a temporary alloca
                    sourceType = sourceExpressionValue->getType();
                    sourceValue = builder->CreateAlloca(sourceType);
                    builder->CreateStore(sourceExpressionValue, sourceValue);
                } else {
                    shared_ptr<ExpressionChained> expressionChained = dynamic_pointer_cast<ExpressionChained>(valueExpression);
                    sourceValue = valueForExpression(expressionChained);
                    sourceType = sourceValue->getType();
                    if (llvm::dyn_cast<llvm::LoadInst>(sourceValue)) {
                        sourceValue = llvm::dyn_cast<llvm::LoadInst>(sourceValue)->getPointerOperand();
                    } else if (llvm::dyn_cast<llvm::AllocaInst>(sourceValue) != nullptr) {
                        sourceType = llvm::dyn_cast<llvm::AllocaInst>(sourceValue)->getAllocatedType();
                    } else if (llvm::dyn_cast<llvm::CallInst>(sourceValue)) {
                        llvm::AllocaInst *alloca = builder->CreateAlloca(sourceType, nullptr);
                        builder->CreateStore(sourceValue, alloca);
                        sourceValue = alloca;
                    }
                }

                if (!sourceType->isArrayTy()) {
                    markError(valueExpression->getLine(), valueExpression->getColumn(), "Not an array type");
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
                markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid assignment to data type");
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
                    llvm::Value *sourceValue = valueForExpression(valueExpressions.at(i));
                    llvm::Value *targetMember = builder->CreateGEP(targetType, targetValue, index);
                    builder->CreateStore(sourceValue, targetMember);
                }
                break;
            }
            // blob <- blob
            case ExpressionKind::VALUE:
            case ExpressionKind::CHAINED:
            // blob <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = valueForExpression(valueExpression);
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            default:
                markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid assignment");
                break;
        }
    // pointer
    } else if (targetType->isPointerTy()) {
        switch (valueExpression->getKind()) {
            // ptr <- { }
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                if (valueExpressions.size() != 1) {
                    markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid composite assignment");
                    break;
                }
                llvm::Value *adrValue = valueForExpression(valueExpressions.at(0));
                if (adrValue == nullptr) {
                    markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid composite assignment");
                    break;
                }
                llvm::Value *sourceValue = builder->CreateIntToPtr(adrValue, typePtr);
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            // ptr <- ptr
            case ExpressionKind::VALUE:
            case ExpressionKind::CHAINED:
            // ptr <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = valueForExpression(valueExpression);
                if (sourceValue == nullptr)
                    return;
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            default:
                markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid assignment to pointer type");
                break;
        }
    // simple
    } else {
        switch (valueExpression->getKind()) {
            // simple <- literal
            case ExpressionKind::LITERAL:
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
            case ExpressionKind::VALUE:
            case ExpressionKind::CHAINED: {
                llvm::Value *sourceValue = valueForExpression(valueExpression);
                if (sourceValue == nullptr)
                    return;
                builder->CreateStore(sourceValue, targetValue);
                break;
            }
            // other
            default:
                markError(valueExpression->getLine(), valueExpression->getColumn(), "Invalid assignment");
                return;
        }
    }
}

//
// Expressions
//
llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
            return valueForExpression(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::BLOCK:
            return valueForExpression(dynamic_pointer_cast<ExpressionBlock>(expression));
        case ExpressionKind::CALL:
            return valueForExpression(dynamic_pointer_cast<ExpressionCall>(expression));
        // Cast ?
        case ExpressionKind::CHAINED:
            return valueForExpression(dynamic_pointer_cast<ExpressionChained>(expression));
        case ExpressionKind::COMPOSITE_LITERAL:
            return valueForExpression(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression));
        case ExpressionKind::GROUPING:
            return valueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::IF_ELSE:
            return valueForExpression(dynamic_pointer_cast<ExpressionIfElse>(expression));
        case ExpressionKind::LITERAL:
            return valueForExpression(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::NONE:
            return llvm::UndefValue::get(typeVoid);
        case ExpressionKind::UNARY:
            return valueForExpression(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::VALUE:
            return valueForExpression(dynamic_pointer_cast<ExpressionValue>(expression));
        default:
            markError(expression->getLine(), expression->getColumn(), "Unexpected expression");
            return nullptr;
    }
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    llvm::Value *leftValue = valueForExpression(expressionBinary->getLeft());
    llvm::Value *rightValue = valueForExpression(expressionBinary->getRight());

    if (leftValue == nullptr || rightValue == nullptr)
        return nullptr;

    // types will match in cases when it's important
    shared_ptr<ValueType> valueType = expressionBinary->getLeft()->getValueType();

    switch (expressionBinary->getOperation()) {
        // logical
        case ExpressionBinaryOperation::OR: {
            builder->CreateLogicalOr(leftValue, rightValue);
        }
        case ExpressionBinaryOperation::XOR: {
            return builder->CreateXor(leftValue, rightValue);
        }
        case ExpressionBinaryOperation::AND: {
            return builder->CreateLogicalAnd(leftValue, rightValue);
        }

        // bitwise
        case ExpressionBinaryOperation::BIT_OR: {
            return builder->CreateOr(leftValue, rightValue);
        }
        case ExpressionBinaryOperation::BIT_XOR: {
            return builder->CreateXor(leftValue, rightValue);
        }
        case ExpressionBinaryOperation::BIT_AND: {
            return builder->CreateAnd(leftValue, rightValue);
        }
        case ExpressionBinaryOperation::BIT_SHL: {
            if (valueType->isUnsignedInteger())
                return builder->CreateShl(leftValue, rightValue, "", true, false);
            else if (valueType->isSignedInteger())
                return builder->CreateShl(leftValue, rightValue, "", false, true);
            break;
        }
        case ExpressionBinaryOperation::BIT_SHR: {
            if (valueType->isUnsignedInteger())
                return builder->CreateLShr(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateAShr(leftValue, rightValue);
            break;
        }

        // comparison
        case ExpressionBinaryOperation::EQUAL: {
            if (valueType->isInteger() || valueType->isBool())
                return builder->CreateICmpEQ(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFCmpOEQ(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::NOT_EQUAL: {
            if (valueType->isInteger() || valueType->isBool())
                return builder->CreateICmpNE(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFCmpONE(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::LESS: {
            if (valueType->isUnsignedInteger())
                return builder->CreateICmpULT(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateICmpSLT(leftValue, rightValue);
            else if(valueType->isFloat())
                return builder->CreateFCmpOLT(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::LESS_EQUAL: {
            if (valueType->isUnsignedInteger())
                return builder->CreateICmpULE(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateICmpSLE(leftValue, rightValue);
            else if(valueType->isFloat())
                return builder->CreateFCmpOLE(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::GREATER: {
            if (valueType->isUnsignedInteger())
                return builder->CreateICmpUGT(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateICmpSGT(leftValue, rightValue);
            else if(valueType->isFloat())
                return builder->CreateFCmpOGT(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::GREATER_EQUAL: {
            if (valueType->isUnsignedInteger())
                return builder->CreateICmpUGE(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateICmpSGE(leftValue, rightValue);
            else if(valueType->isFloat())
                return builder->CreateFCmpOGE(leftValue, rightValue);
            break;
        }

        // mathematical
        case ExpressionBinaryOperation::ADD: {
            if (valueType->isUnsignedInteger())
                return builder->CreateNUWAdd(leftValue, rightValue); // No Unsigned Wrap
            else if (valueType->isSignedInteger())
                return builder->CreateNSWAdd(leftValue, rightValue); // No Signed Wrap
            else if (valueType->isFloat())
                return builder->CreateFAdd(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::SUB: {
            if (valueType->isUnsignedInteger())
                return builder->CreateNUWSub(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateNSWSub(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFSub(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::MUL: {
            if (valueType->isUnsignedInteger())
                return builder->CreateNUWMul(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateNSWMul(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFMul(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::DIV: {
            if (valueType->isUnsignedInteger())
                return builder->CreateUDiv(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateSDiv(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFDiv(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::MOD: {
            if (valueType->isUnsignedInteger())
                return builder->CreateURem(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                return builder->CreateSRem(leftValue, rightValue);
            else if (valueType->isFloat())
                return builder->CreateFRem(leftValue, rightValue);
            break;
        }
    }

    markErrorInvalidOperationBinary(
        expressionBinary->getLine(),
        expressionBinary->getColumn(),
        expressionBinary->getOperation(),
        expressionBinary->getLeft()->getValueType(),
        expressionBinary->getRight()->getValueType()
    );
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionBlock> expressionBlock) {
    buildStatement(expressionBlock->getStatementBlock());
    return valueForExpression(expressionBlock->getResultStatementExpression()->getExpression());
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionCall> expressionCall) {
    llvm::Function *fun = scope->getFunction(expressionCall->getName());
    if (fun != nullptr) {
        return wrappedValueForCall(fun, fun->getFunctionType(), expressionCall)->getValue();
    }

    llvm::InlineAsm *rawFun = scope->getInlineAsm(expressionCall->getName());
    if (rawFun != nullptr) {
        vector<llvm::Value *>argValues;
        for (shared_ptr<Expression> &argumentExpression : expressionCall->getArgumentExpressions()) {
            llvm::Value *argValue = valueForExpression(argumentExpression);
            argValues.push_back(argValue);
        }
        return builder->CreateCall(rawFun, llvm::ArrayRef(argValues));
    }

    markError(expressionCall->getLine(), expressionCall->getColumn(), format("Function \"{}\" not defined in scope", expressionCall->getName()));
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionChained> expressionChained) {
    shared_ptr<WrappedValue> currentWrappedValue;
    shared_ptr<Expression> parentExpression;

    vector<shared_ptr<Expression>> chainExpressions = expressionChained->getChainExpressions();
    for (int i=0; i<chainExpressions.size(); i++) {
        shared_ptr<Expression> chainExpression = chainExpressions.at(i);

        // If the first expression is a cast, try doing a built-in on a type
        if (currentWrappedValue == nullptr && chainExpression->getKind() == ExpressionKind::CAST && chainExpressions.size() >= 2) {
            llvm::Type *type = typeForValueType(chainExpression->getValueType());
            shared_ptr<ExpressionValue> childExpressionVariable = dynamic_pointer_cast<ExpressionValue>(chainExpressions.at(++i));
            currentWrappedValue = wrappedValueForTypeBuiltIn(type, childExpressionVariable);
            parentExpression = chainExpression;
            continue;
        }

        // First in chain is treated as a single variable
        if (currentWrappedValue == nullptr) {
            currentWrappedValue = wrappedValueForExpression(chainExpression);
            parentExpression = chainExpression;
            if (currentWrappedValue == nullptr)
                return nullptr;
            continue;
        }

        // Cast expression?
        if (shared_ptr<ExpressionCast> expressionCast = dynamic_pointer_cast<ExpressionCast>(chainExpression)) {
            currentWrappedValue = wrappedValueForCast(currentWrappedValue->getValue(), expressionCast->getValueType());
            parentExpression = chainExpression;
            if (currentWrappedValue == nullptr)
                return nullptr;
            continue;
        }

        // Check parent expression
        shared_ptr<ExpressionValue> parentExpressionVariable = dynamic_pointer_cast<ExpressionValue>(chainExpressions.at(i-1));
        if (parentExpressionVariable == nullptr) {
            markError(parentExpressionVariable->getLine(), parentExpressionVariable->getColumn(), "Invalid expression type");
            return nullptr;
        }

        // Built-in expression?
        if(shared_ptr<WrappedValue> builtInValue = wrappedValueForBuiltIn(currentWrappedValue, parentExpressionVariable, chainExpression)) {
            currentWrappedValue = builtInValue;
            parentExpression = chainExpression;
            continue;
        }

        // Check chained expression type 
        shared_ptr<ExpressionValue> expressionVariable = dynamic_pointer_cast<ExpressionValue>(chainExpression);
        if (expressionVariable == nullptr) {
            markError(expressionVariable->getLine(), expressionVariable->getColumn(), "Invalid expression type");
            return nullptr;
        }

        // Variable expression?
        if (!currentWrappedValue->isStruct()) {
            markError(expressionVariable->getLine(), expressionVariable->getColumn(), "Something's fucky");
            return nullptr;
        }
        string structName = *parentExpression->getValueType()->getBlobName();
        optional<int> memberIndex = scope->getStructMemberIndex(structName, expressionVariable->getIdentifier());
        if (!memberIndex) {
            markError(expressionVariable->getLine(), expressionVariable->getColumn(), format("Invalid member \"{}\" for \"blob<{}>\"", expressionVariable->getIdentifier(), structName));
            return nullptr;
        }
        llvm::Value *index[] = {
            builder->getInt32(0),
            builder->getInt32(*memberIndex)
        };

        llvm::Value *elementPtr = builder->CreateGEP(currentWrappedValue->getStructType(), currentWrappedValue->getPointerValue(), index);
        llvm::Type *elementType = currentWrappedValue->getStructType()->getElementType(*memberIndex);

        currentWrappedValue = wrappedValueForSourceValue(elementPtr, elementType, expressionVariable);
        parentExpression = chainExpression;
    }

    return currentWrappedValue->getValue();
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    llvm::Type *type = typeForValueType(expressionCompositeLiteral->getValueType());

    // First try making them constant
    if (expressionCompositeLiteral->getValueType()->isData()) {
        vector<llvm::Constant*> constantValues;
        int count = expressionCompositeLiteral->getValueType()->getValueArg();
        for (int i=0; i<count; i++) {
            shared_ptr<Expression> elementExpression = expressionCompositeLiteral->getExpressions().at(i);
            llvm::Value *value = valueForExpression(elementExpression);
            llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
            if (constantValue == nullptr)
                goto not_constant;
            constantValues.push_back(constantValue);
        }
        llvm::ArrayType *arrayType = llvm::dyn_cast<llvm::ArrayType>(type);
        return llvm::ConstantArray::get(arrayType, constantValues);
    } else if (expressionCompositeLiteral->getValueType()->isBlob()) {
        vector<llvm::Constant*> constantValues;
        for (shared_ptr<Expression> memberExpression : expressionCompositeLiteral->getExpressions()) {
            llvm::Value *value = valueForExpression(memberExpression);
            llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
            if (constantValue == nullptr)
                goto not_constant;
            constantValues.push_back(constantValue);
        }
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(type);
        return llvm::ConstantStruct::get(structType, constantValues);
    } else if (expressionCompositeLiteral->getValueType()->isPointer()) {
        llvm::Value *value = valueForExpression(expressionCompositeLiteral->getExpressions().at(0));
        llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
        if (constantValue == nullptr)
            goto not_constant;
        return llvm::ConstantExpr::getIntToPtr(constantValue, typePtr);
    }

    // Otherwise try normal dynamic alloca
    not_constant:
    if (builder->GetInsertBlock() == nullptr) {
        markErrorInvalidConstant(expressionCompositeLiteral->getLine(), expressionCompositeLiteral->getColumn());
        return nullptr;
    }

    llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr);
    buildAssignment(alloca, type, expressionCompositeLiteral);
    return builder->CreateLoad(type, alloca);
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionGrouping> expressionGrouping) {
    return valueForExpression(expressionGrouping->getSubExpression());
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionIfElse> expressionIfElse) {
    shared_ptr<Expression> conditionExpression = expressionIfElse->getConditionExpression();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::Value *conditionValue = valueForExpression(conditionExpression);

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(*context, "thenBlock", fun);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(*context, "elseBlock");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(*context, "mergeBlock");

    if (expressionIfElse->getElseExpression() != nullptr) {
        builder->CreateCondBr(conditionValue, thenBlock, elseBlock);
    } else {
        builder->CreateCondBr(conditionValue, thenBlock, mergeBlock);
    }

    // Then
    scope->pushLevel();
    builder->SetInsertPoint(thenBlock);
    llvm::Value *thenValue = valueForExpression(expressionIfElse->getThenExpression());
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();
    scope->popLevel();

    // Else
    llvm::Value *elseValue = nullptr;
    if (expressionIfElse->getElseExpression() != nullptr) {
        scope->pushLevel();
        fun->insert(fun->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);
        elseValue = valueForExpression(expressionIfElse->getElseExpression());
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

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    switch (expressionLiteral->getValueType()->getKind()) {
        case ValueTypeKind::BOOL:
            return llvm::ConstantInt::getBool(typeBool, expressionLiteral->getBoolValue());

        case ValueTypeKind::INT:
            return llvm::ConstantInt::get(typeSInt, expressionLiteral->getSIntValue());

        case ValueTypeKind::U8:
            return llvm::ConstantInt::get(typeU8, expressionLiteral->getUIntValue());
        case ValueTypeKind::U32:
            return llvm::ConstantInt::get(typeU32, expressionLiteral->getUIntValue());
        case ValueTypeKind::U64:
            return llvm::ConstantInt::get(typeU64, expressionLiteral->getUIntValue());

        case ValueTypeKind::S8:
            return llvm::ConstantInt::get(typeS8, expressionLiteral->getSIntValue());
        case ValueTypeKind::S32:
            return llvm::ConstantInt::get(typeS32, expressionLiteral->getSIntValue());
        case ValueTypeKind::S64:
            return llvm::ConstantInt::get(typeS64, expressionLiteral->getSIntValue());

        case ValueTypeKind::FLOAT:
            return llvm::ConstantFP::get(typeFloat, expressionLiteral->getFloatValue());

        case ValueTypeKind::F32:
            return llvm::ConstantFP::get(typeF32, expressionLiteral->getFloatValue());
        case ValueTypeKind::F64:
            return llvm::ConstantFP::get(typeF64, expressionLiteral->getFloatValue());

        default:
            break;
    }

    markErrorInvalidLiteral(
        expressionLiteral->getLine(),
        expressionLiteral->getColumn(),
        expressionLiteral->getValueType()
    );
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    shared_ptr<ValueType> valueType = expressionUnary->getSubExpression()->getValueType();
    llvm::Value *value = valueForExpression(expressionUnary->getSubExpression());
    if (value == nullptr)
        return nullptr;

    switch (expressionUnary->getOperation()) {
        case ExpressionUnaryOperation::BIT_NOT:
        case ExpressionUnaryOperation::NOT: {
            if (valueType->isBool() || valueType->isInteger()) {
                return builder->CreateNot(value);
            }
            break;
        }
        case ExpressionUnaryOperation::MINUS: {
            if (valueType->isUnsignedInteger()) {
                return builder->CreateNeg(value);
            } else if (valueType->isSignedInteger()) {
                return builder->CreateNSWNeg(value);
            } else if (valueType->isFloat()) {
                return builder->CreateFNeg(value);
            }
            break;
        }
        case ExpressionUnaryOperation::PLUS: {
            if (valueType->isNumeric()) {
                return value;
            }
            break;
        }
    }

    markErrorInvalidOperationUnary(
        expressionUnary->getLine(),
        expressionUnary->getColumn(),
        expressionUnary->getOperation(),
        valueType
    );
    return nullptr;
}

llvm::Value *ModuleBuilder::valueForExpression(shared_ptr<ExpressionValue> expressionValue) {
    llvm::Value *value = nullptr;
    llvm::Type *type = nullptr;

    llvm::AllocaInst *localAlloca = scope->getAlloca(expressionValue->getIdentifier());
    llvm::Value *globalValuePtr = scope->getGlobal(expressionValue->getIdentifier());
    llvm::Value *fun = scope->getFunction(expressionValue->getIdentifier());
    if (localAlloca != nullptr) {
        value = localAlloca;
        type = localAlloca->getAllocatedType();
    } else if (globalValuePtr != nullptr) {
        shared_ptr<ValueType> valueType = expressionValue->getValueType();
        type = typeForValueType(valueType);
        value = globalValuePtr;
    } else if (fun != nullptr) {
        type = fun->getType();
        value = fun;
    }

    if (value == nullptr) {
        markErrorNotDefined(expressionValue->getLine(), expressionValue->getColumn(), format("variable \"{}\"", expressionValue->getIdentifier()));
        return nullptr;
    }

    return wrappedValueForSourceValue(value, type, expressionValue)->getValue();
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<Expression> expression) {
    llvm::Value *value = valueForExpression(expression);
    if (value == nullptr)
        return nullptr;

    return WrappedValue::wrappedValue(builder, value);
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForBuiltIn(shared_ptr<WrappedValue> parentWrappedValue, shared_ptr<ExpressionValue> parentExpression, shared_ptr<Expression> expression) {
    bool isCount = false;
    bool isVal = false;
    bool isVadr = false;
    bool isAdr = false;
    bool isSize = false;

    shared_ptr<ExpressionValue> expressionVariable = dynamic_pointer_cast<ExpressionValue>(expression);
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

    // Return quickly if not a built-in
    if (!isCount && !isVal && !isVadr && !isAdr && !isSize)
        return nullptr;

    // Do the appropriate built-in operation
    if (parentWrappedValue->isArray() && isCount) {
        return WrappedValue::wrappedValue(
            builder,
            llvm::ConstantInt::get(typeSInt, parentWrappedValue->getArrayType()->getNumElements())
        );
    } else if (parentWrappedValue->isPointer() && isVal) {
        llvm::LoadInst *pointeeLoad = builder->CreateLoad(typePtr, parentWrappedValue->getPointerValue());

        shared_ptr<ValueType> pointeeValueType = parentExpression->getValueType()->getSubType();
        if (pointeeValueType == nullptr) {
            markError(parentExpression->getLine(), parentExpression->getColumn(), "No type for ptr");
            return nullptr;
        }
        llvm::Type *pointeeType = typeForValueType(pointeeValueType);
        if (pointeeType == nullptr) {
            markError(parentExpression->getLine(), parentExpression->getColumn(), "No type for ptr");
            return nullptr; 
        }

        return wrappedValueForSourceValue(pointeeLoad, pointeeType, expression);
    } else if (parentWrappedValue->isPointer() && isVadr) {
        llvm::LoadInst *pointeeLoad = (llvm::LoadInst*)builder->CreateLoad(typePtr, parentWrappedValue->getPointerValue());
        return WrappedValue::wrappedValue(
            builder,
            builder->CreatePtrToInt(pointeeLoad, typeIntPtr)
        );
    } else if (isAdr) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreatePtrToInt(parentWrappedValue->getPointerValue(), typeIntPtr)
        );
    } else if (isSize) {
        int sizeInBytes = sizeInBitsForType(parentWrappedValue->getType()) / 8;
        if (sizeInBytes > 0) {
            return WrappedValue::wrappedValue(
                builder,
                llvm::ConstantInt::get(typeUInt, sizeInBytes)
            );
        } else {
            return nullptr;
        }
    }

    markError(expression->getLine(), expression->getColumn(), "Invalid built-in operation");
    return nullptr;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForCall(llvm::Value *fun, llvm::FunctionType *funType, shared_ptr<ExpressionCall> expression) {
    vector<llvm::Value*> argValues;
    vector<shared_ptr<Expression>> argumentExpressions = expression->getArgumentExpressions();
    for (int i=0; i<argumentExpressions.size(); i++) {
        // pass along type for the specified argument
        shared_ptr<Expression> argumentExpression = argumentExpressions.at(i);
        llvm::Value *argValue = valueForExpression(argumentExpression);
        argValues.push_back(argValue);
    }
    return WrappedValue::wrappedValue(
        builder,
        builder->CreateCall(funType, fun, llvm::ArrayRef(argValues))
    );
}

  shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForCast(llvm::Value *sourceValue, shared_ptr<ValueType> targetValueType) {
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
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateZExtOrTrunc(sourceValue, targetType)
        );
    // uint to float
    } else if (isSourceUInt && isTargetFloat) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateUIToFP(sourceValue, targetType)
        );
    // sint to float
    } else if (isSourceSInt && isTargetFloat) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateSIToFP(sourceValue, targetType)
        );
    // float to float+
    } else if (isSourceFloat && isTargetFloat && targetSize >= sourceSize) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateFPExt(sourceValue, targetType)
        );
    // float to float-
    } else if (isSourceFloat && isTargetFloat && targetSize < sourceSize) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateFPTrunc(sourceValue, targetType)
        );
    // float to uint
    } else if (isSourceFloat && isTargetUInt) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateFPToUI(sourceValue, targetType)
        );
    // float to sint
    } else if (isSourceFloat && isTargetSInt) {
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateFPToSI(sourceValue, targetType)
        );
    // data to data
    } else if (isSourceData && isTargetData) {
        llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(sourceValue->getType());

        llvm::Constant *sourceConstantValue = llvm::dyn_cast<llvm::Constant>(sourceValue);
        if (llvm::dyn_cast<llvm::LoadInst>(sourceValue) != nullptr)
            sourceValue = llvm::dyn_cast<llvm::LoadInst>(sourceValue)->getPointerOperand();
        
        int targetCount = targetValueType->getValueArg();
        int sourceCount = sourceArrayType->getNumElements();
        if (targetCount == 0)
            targetCount = sourceCount;
        int copyCount = min(targetCount, sourceCount);

        llvm::Type *targetType = typeForValueType(targetValueType);
        llvm::Value *targetValue = builder->CreateAlloca(targetType);

        for (int i=0; i<copyCount; i++) {
            llvm::Value *index[] = {
                builder->getInt32(0),
                builder->getInt32(i)
            };

            // constant or non-constant copy?
            llvm::Value *sourceMemberValue;
            if (sourceConstantValue != nullptr) {
                sourceMemberValue = sourceConstantValue->getAggregateElement(i);
            } else {
                llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceArrayType, sourceValue, index);
                sourceMemberValue = builder->CreateLoad(sourceArrayType->getArrayElementType(), sourceMemberPtr);
            }

            llvm::Value *targetMemberPtr = builder->CreateGEP(targetType, targetValue, index);

            builder->CreateStore(sourceMemberValue, targetMemberPtr);
        }
        return WrappedValue::wrappedValue(
            builder,
            builder->CreateLoad(targetType, targetValue)
        );
    } else {
        markError(0, 0, "Invalid cast");
        return nullptr;
    }
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType, shared_ptr<Expression> expression) {
    if (builder->GetInsertBlock() == nullptr)
        return nullptr;

    shared_ptr<ExpressionValue> expressionVariable = dynamic_pointer_cast<ExpressionValue>(expression);
    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression);

    if (expressionVariable != nullptr) {
        switch (expressionVariable->getValueKind()) {
            case ExpressionValueKind::FUN:
            case ExpressionValueKind::SIMPLE:
            case ExpressionValueKind::BUILT_IN_VAL_SIMPLE: {
                return WrappedValue::wrappedValue(
                    builder,
                    builder->CreateLoad(sourceType, sourceValue, expressionVariable->getIdentifier())
                );
            }
            case ExpressionValueKind::DATA: 
            case ExpressionValueKind::BUILT_IN_VAL_DATA: {
                llvm::Value *indexValue = valueForExpression(expressionVariable->getIndexExpression());
                if (indexValue == nullptr)
                    return nullptr;
                llvm::Value *index[] = {
                    builder->getInt32(0),
                    indexValue
                };
                llvm::Type *expType = llvm::ArrayType::get(typeForValueType(expression->getValueType()), 0); // TODO: this is hack and should be fixed
                llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(expType);
                llvm::Value *elementPtr = builder->CreateGEP(sourceArrayType, sourceValue, index, format("{}[]", expressionVariable->getIdentifier()));
                return WrappedValue::wrappedValue(
                    builder,
                    builder->CreateLoad(sourceArrayType->getArrayElementType(), elementPtr)
                );
            }
        }
    } else if (expressionCall != nullptr) {
        llvm::FunctionType *funType = llvm::dyn_cast<llvm::FunctionType>(sourceType);
        return wrappedValueForCall(sourceValue, funType, expressionCall);
    }
    return nullptr;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionValue> expression) {
    bool isSize = expression->getIdentifier().compare("size") == 0;

    if (isSize) {
        int sizeInBytes = sizeInBitsForType(type) / 8;
        if (sizeInBytes <= 0)
            return nullptr;
        return WrappedValue::wrappedValue(
            builder,
            llvm::ConstantInt::get(typeUInt, sizeInBytes)
        );
    }
    
    markError(expression->getLine(), expression->getColumn(), "Invalid built-in operation");
    return nullptr;
}

//
// Support
//
llvm::Type *ModuleBuilder::typeForValueType(shared_ptr<ValueType> valueType) {
    if (valueType == nullptr)
        return nullptr;

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

            // sometimes the count can be empty (for example pointer to data)
            int elementsCount = 0;
            if (dynamic_pointer_cast<ExpressionLiteral>(valueType->getCountExpression()) != nullptr)
                elementsCount = dynamic_pointer_cast<ExpressionLiteral>(valueType->getCountExpression())->getUIntValue();

            return llvm::ArrayType::get(typeForValueType(valueType->getSubType()), elementsCount);
        }
        case ValueTypeKind::BLOB: {
            llvm::StructType *structType = scope->getStructType(*(valueType->getBlobName()));
            if (structType == nullptr)
                markErrorNotDefined(0, 0, *(valueType->getBlobName()));
            return structType;
        }
        case ValueTypeKind::FUN: {
            // returnType
            llvm::Type *functionReturnType = typeForValueType(valueType->getReturnType());

            // argument types
            vector<llvm::Type *> functionArgumentTypes;
            vector<shared_ptr<ValueType>> argumentTypes = *(valueType->getArgumentTypes());
            for (shared_ptr<ValueType> &argumentType : argumentTypes) {
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
            break;
    }

    return nullptr;
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

void ModuleBuilder::markErrorInvalidOperationUnary(int line, int column, ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    string message = format(
        "Invalid unary operation {} for type {}",
        Logger::toString(operation),
        Logger::toString(type)
    );
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorInvalidOperationBinary(int line, int column, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    string message = format(
        "Invalid binary operation {} for types {} and {}",
        Logger::toString(operation),
        Logger::toString(firstType),
        Logger::toString(secondType)
    );
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorAlreadyDefined(int line, int column, string identifier) {
    string message = format("{} has already been defined in scope", identifier);
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorNotDeclared(int line, int column, string identifier) {
    string message = format("{} is not declared in scope", identifier);
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorNotDefined(int line, int column, string identifier) {
    string message = format("{} is not defined in scope", identifier);
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markInvalidConstraints(int line, int column, string functionName, string constraints) {
    string message = format("Constraints \"{}\" for function \"{}\" is invalid", constraints, functionName);
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorInvalidLiteral(int line, int column, shared_ptr<ValueType> type) {
    string message = format("Invalid literal for type {}", Logger::toString(type));
    errors.push_back(Error::error(line, column, message));
}

void ModuleBuilder::markErrorInvalidConstant(int line, int column) {
    string message = format("Not a valid constant expression");
    errors.push_back(Error::error(line, column, message));
}