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
    typeA = llvm::Type::getIntNTy(*context, pointerSize);
}

shared_ptr<llvm::Module> ModuleBuilder::getModule() {
    scope = make_shared<Scope>();

    // build just the import statements
    for (auto statement : statements | views::filter([](auto it) { return it->getKind() == StatementKind::META_IMPORT; })) {
        buildStatement(statement);
    }

    // build header
    for (shared_ptr<Statement> &headerStatement : headerStatements)
        buildStatement(headerStatement);

    // build statements other than import
    for (auto statement : statements | views::filter([](auto it) { return it->getKind() != StatementKind::META_IMPORT; })) {
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
            markErrorUnexpected(statement->getLocation(), "statement");
    }
}

void ModuleBuilder::buildStatement(shared_ptr<StatementAssignment> statementAssignment) {
    llvm::Value *targetValue = wrappedValueForExpression(statementAssignment->getExpressionChained())->getValue();
    if (targetValue == nullptr)
        return;

    buildAssignment(
        WrappedValue::wrappedValue(module, builder, targetValue, statementAssignment->getValueExpression()->getValueType()),
        statementAssignment->getValueExpression()
    );
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
    wrappedValueForExpression(statementExpression->getExpression());
}

void ModuleBuilder::buildStatement(shared_ptr<StatementFunction> statementFunction) {
    // Check if declared
    llvm::Function *fun = scope->getFunction(statementFunction->getName());
    if (fun == nullptr) {
        markErrorNotDeclared(statementFunction->getLocation(), format("function \"{}\"", statementFunction->getName()));
        return;
    }

    // Check if function not yet defined
    llvm::BasicBlock &entryBlock = fun->getEntryBlock();
    if (entryBlock.getParent() != nullptr) {
        markErrorAlreadyDefined(statementFunction->getLocation(), format("function \"{}\"", statementFunction->getName()));
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

        // allocate argument
        llvm::Type *funArgumentType = typeForValueType(argument.second);
        if (funArgumentType == nullptr)
            return;
        llvm::AllocaInst *alloca = builder->CreateAlloca(funArgumentType, nullptr, argument.first);
        builder->CreateStore(funArgument, alloca);

        scope->setWrappedValue(
            argument.first,
            WrappedValue::wrappedValue(
                module,
                builder,
                alloca,
                argument.second
            )
        );
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
        markErrorInvalidImport(statementMetaImport->getLocation(), statementMetaImport->getName());
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
                markErrorUnexpected(importedStatement->getLocation(), "imported statement");
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
            statementRawFunction->getLocation(),
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
        llvm::Value *preConditionValue = wrappedValueForExpression(preExpression)->getValue();
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
        llvm::Value *postConditionValue = wrappedValueForExpression(postExpression)->getValue();
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
        llvm::Value *returnValue = wrappedValueForExpression(statementReturn->getExpression())->getValue();
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
    scope->setWrappedValue(
        internalName,
        WrappedValue::wrappedValue(
            module,
            builder,
            global,
            valueType
        )
    );
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
        markErrorNotDeclared(nullptr, format("blob \"{}\"", symbolName));
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
    llvm::Type *type = typeForValueType(statement->getValueType());
    llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr, statement->getIdentifier());

    // try registering new variable in scope
    scope->setWrappedValue(
        statement->getIdentifier(),
        WrappedValue::wrappedValue(
            module,
            builder,
            alloca,
            statement->getValueType()
        )
    );

    if (statement->getExpression() == nullptr)
        return;

    buildAssignment(
        WrappedValue::wrappedValue(module, builder, alloca, statement->getValueType()),
        statement->getExpression()
    );
}

void ModuleBuilder::buildGlobalVariable(shared_ptr<StatementVariable> statement) {
    // variable
    shared_ptr<WrappedValue> globalWrappedValue = scope->getWrappedValue(statement->getIdentifier());
    if (globalWrappedValue == nullptr) {
        markErrorNotDeclared(statement->getLocation(), format("global \"{}\"", statement->getIdentifier()));
        return;
    }

    llvm::GlobalVariable *global = globalWrappedValue->getGlobalValue();
    if (global == nullptr) {
        markErrorInvalidGlobal(statement->getLocation());
        return;
    }

    if (global->hasInitializer()) {
        markErrorAlreadyDefined(statement->getLocation(), format("global \"{}\"", statement->getIdentifier()));
        return;
    }

    // initialization
    llvm::Type *type = typeForValueType(statement->getValueType());
    llvm::Constant *constantValue = llvm::Constant::getNullValue(type);
    if (statement->getExpression() != nullptr) {
        shared_ptr<WrappedValue> wrappedValue = wrappedValueForExpression(statement->getExpression());
        if (wrappedValue == nullptr || (constantValue = wrappedValue->getConstantValue()) == nullptr) {
            markErrorInvalidConstant(statement->getLocation());
            return;
        }
    }

    global->setInitializer(constantValue);
}

void ModuleBuilder::buildAssignment(shared_ptr<WrappedValue> targetWrappedValue, shared_ptr<Expression> valueExpression) {
    // data
    if (targetWrappedValue->isArray()) {
        switch (valueExpression->getKind()) {
            // data <- { }
            // copy values from literal expression into an allocated array
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                int sourceCount = valueExpressions.size();
                int targetCount = targetWrappedValue->getArrayType()->getNumElements();
                int count = min(sourceCount, targetCount);
                for (int i=0; i<count; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *targetPtr = builder->CreateGEP(targetWrappedValue->getType(), targetWrappedValue->getPointerValue(), index);
                    llvm::Value *sourceValue = wrappedValueForExpression(valueExpressions.at(i))->getValue();
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
                shared_ptr<WrappedValue> sourceWrappedValue;

                if (valueExpression->getKind() == ExpressionKind::VALUE) {
                    shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(valueExpression);
                    sourceWrappedValue = scope->getWrappedValue(expressionValue->getIdentifier());
                } else if (valueExpression->getKind() == ExpressionKind::CALL) {
                    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(valueExpression);
                    sourceWrappedValue = wrappedValueForExpression(expressionCall);
                } else {
                    shared_ptr<ExpressionChained> expressionChained = dynamic_pointer_cast<ExpressionChained>(valueExpression);
                    sourceWrappedValue = wrappedValueForExpression(expressionChained);
                }
                if (sourceWrappedValue == nullptr)
                    return;

                if (!sourceWrappedValue->isArray()) {
                    markErrorInvalidType(valueExpression->getLocation());
                    return;
                }

                // make sure we don't go over the bounds
                int sourceCount = sourceWrappedValue->getArrayType()->getArrayNumElements();
                int targetCount = targetWrappedValue->getArrayType()->getNumElements();
                int elementsCount = min(sourceCount, targetCount);

                for (int i=0; i<elementsCount; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };

                    // get pointers for both source and target
                    llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceWrappedValue->getArrayType(), sourceWrappedValue->getPointerValue(), index);
                    llvm::Value *targetMemberPtr = builder->CreateGEP(targetWrappedValue->getArrayType(), targetWrappedValue->getPointerValue(), index);
                    // load value from source pointer
                    llvm::Value *sourceMemberValue = builder->CreateLoad(sourceWrappedValue->getArrayType()->getArrayElementType(), sourceMemberPtr);
                    // and then store it at the target pointer
                    builder->CreateStore(sourceMemberValue, targetMemberPtr);
                }
                break;
            }
            default:
                markErrorInvalidAssignment(valueExpression->getLocation());
                return;
        }
    // blob
    } else if (targetWrappedValue->isStruct()) {
        switch (valueExpression->getKind()) {
            // blob <- { }
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                int membersCount = targetWrappedValue->getStructType()->getStructNumElements();
                for (int i=0; i<membersCount; i++) {
                    llvm::Value *index[] = {
                        builder->getInt32(0),
                        builder->getInt32(i)
                    };
                    llvm::Value *sourceValue = wrappedValueForExpression(valueExpressions.at(i))->getValue();
                    llvm::Value *targetMember = builder->CreateGEP(targetWrappedValue->getType(), targetWrappedValue->getPointerValue(), index);
                    builder->CreateStore(sourceValue, targetMember);
                }
                break;
            }
            // blob <- blob
            case ExpressionKind::VALUE:
            case ExpressionKind::CHAINED:
            // blob <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = wrappedValueForExpression(valueExpression)->getValue();
                builder->CreateStore(sourceValue, targetWrappedValue->getPointerValue());
                break;
            }
            default:
                markErrorInvalidAssignment(valueExpression->getLocation());
                break;
        }
    // pointer
    } else if (targetWrappedValue->isPointer()) {
        switch (valueExpression->getKind()) {
            // ptr <- { }
            case ExpressionKind::COMPOSITE_LITERAL: {
                vector<shared_ptr<Expression>> valueExpressions = dynamic_pointer_cast<ExpressionCompositeLiteral>(valueExpression)->getExpressions();
                if (valueExpressions.size() != 1) {
                    markErrorInvalidAssignment(valueExpression->getLocation());
                    break;
                }
                llvm::Value *adrValue = wrappedValueForExpression(valueExpressions.at(0))->getValue();
                if (adrValue == nullptr) {
                    markErrorInvalidAssignment(valueExpression->getLocation());
                    break;
                }
                llvm::Value *sourceValue = builder->CreateIntToPtr(adrValue, typePtr);
                builder->CreateStore(sourceValue, targetWrappedValue->getPointerValue());
                break;
            }
            // ptr <- ptr
            case ExpressionKind::VALUE:
            case ExpressionKind::CHAINED:
            // ptr <- function()
            case ExpressionKind::CALL: {
                llvm::Value *sourceValue = wrappedValueForExpression(valueExpression)->getValue();
                if (sourceValue == nullptr)
                    return;
                builder->CreateStore(sourceValue, targetWrappedValue->getPointerValue());
                break;
            }
            default:
                markErrorInvalidAssignment(valueExpression->getLocation());
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
                llvm::Value *sourceValue = wrappedValueForExpression(valueExpression)->getValue();
                if (sourceValue == nullptr)
                    return;
                builder->CreateStore(sourceValue, targetWrappedValue->getPointerValue());
                break;
            }
            // other
            default:
                markErrorInvalidAssignment(valueExpression->getLocation());
                return;
        }
    }
}

//
// Expressions
//
shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::BLOCK:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionBlock>(expression));
        case ExpressionKind::CALL:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionCall>(expression));
        // Cast ?
        case ExpressionKind::CHAINED:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionChained>(expression));
        case ExpressionKind::COMPOSITE_LITERAL:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression));
        case ExpressionKind::GROUPING:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::IF_ELSE:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionIfElse>(expression));
        case ExpressionKind::LITERAL:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::NONE:
            return WrappedValue::wrappedNone(typeVoid, ValueType::NONE);
        case ExpressionKind::UNARY:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::VALUE:
            return wrappedValueForExpression(dynamic_pointer_cast<ExpressionValue>(expression));
        default:
            markErrorUnexpected(expression->getLocation(), "expression");
            return nullptr;
    }
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    shared_ptr<WrappedValue> leftWrappedValue = wrappedValueForExpression(expressionBinary->getLeft());
    shared_ptr<WrappedValue> rightWrappedValue = wrappedValueForExpression(expressionBinary->getRight());
    if (leftWrappedValue == nullptr || rightWrappedValue == nullptr)
        return nullptr;
    llvm::Value *leftValue = leftWrappedValue->getValue();
    llvm::Value *rightValue = rightWrappedValue->getValue();

    if (leftValue == nullptr || rightValue == nullptr)
        return nullptr;

    // types will match in cases when it's important
    shared_ptr<ValueType> valueType = expressionBinary->getLeft()->getValueType();

    llvm::Value *resultValue;
    switch (expressionBinary->getOperation()) {
        // logical
        case ExpressionBinaryOperation::OR: {
            resultValue = builder->CreateLogicalOr(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::XOR: {
            resultValue = builder->CreateXor(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::AND: {
            resultValue = builder->CreateLogicalAnd(leftValue, rightValue);
            break;
        }

        // bitwise
        case ExpressionBinaryOperation::BIT_OR: {
            resultValue = builder->CreateOr(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::BIT_XOR: {
            resultValue = builder->CreateXor(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::BIT_AND: {
            resultValue = builder->CreateAnd(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::BIT_SHL: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateShl(leftValue, rightValue, "", true, false);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateShl(leftValue, rightValue, "", false, true);
            break;
        }
        case ExpressionBinaryOperation::BIT_SHR: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateLShr(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateAShr(leftValue, rightValue);
            break;
        }

        // comparison
        case ExpressionBinaryOperation::EQUAL: {
            if (valueType->isInteger() || valueType->isBool())
                resultValue = builder->CreateICmpEQ(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFCmpOEQ(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::NOT_EQUAL: {
            if (valueType->isInteger() || valueType->isBool())
                resultValue = builder->CreateICmpNE(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFCmpONE(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::LESS: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateICmpULT(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateICmpSLT(leftValue, rightValue);
            else if(valueType->isFloat())
                resultValue = builder->CreateFCmpOLT(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::LESS_EQUAL: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateICmpULE(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateICmpSLE(leftValue, rightValue);
            else if(valueType->isFloat())
                resultValue = builder->CreateFCmpOLE(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::GREATER: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateICmpUGT(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateICmpSGT(leftValue, rightValue);
            else if(valueType->isFloat())
                resultValue = builder->CreateFCmpOGT(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::GREATER_EQUAL: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateICmpUGE(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateICmpSGE(leftValue, rightValue);
            else if(valueType->isFloat())
                resultValue = builder->CreateFCmpOGE(leftValue, rightValue);
            break;
        }

        // mathematical
        case ExpressionBinaryOperation::ADD: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateNUWAdd(leftValue, rightValue); // No Unsigned Wrap
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateNSWAdd(leftValue, rightValue); // No Signed Wrap
            else if (valueType->isFloat())
                resultValue = builder->CreateFAdd(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::SUB: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateNUWSub(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateNSWSub(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFSub(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::MUL: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateNUWMul(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateNSWMul(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFMul(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::DIV: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateUDiv(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateSDiv(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFDiv(leftValue, rightValue);
            break;
        }
        case ExpressionBinaryOperation::MOD: {
            if (valueType->isUnsignedInteger())
                resultValue = builder->CreateURem(leftValue, rightValue);
            else if (valueType->isSignedInteger())
                resultValue = builder->CreateSRem(leftValue, rightValue);
            else if (valueType->isFloat())
                resultValue = builder->CreateFRem(leftValue, rightValue);
            break;
        }
    }

    if (resultValue == nullptr) {
        markErrorInvalidOperationBinary(
            expressionBinary->getLocation(),
            expressionBinary->getOperation(),
            expressionBinary->getLeft()->getValueType(),
            expressionBinary->getRight()->getValueType()
        );
        return nullptr;  
    }

    return WrappedValue::wrappedValue(module, builder, resultValue, expressionBinary->getValueType());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionBlock> expressionBlock) {
    buildStatement(expressionBlock->getStatementBlock());
    return wrappedValueForExpression(expressionBlock->getResultStatementExpression()->getExpression());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionCall> expressionCall) {
    if (llvm::Function *fun = scope->getFunction(expressionCall->getName())) {
        return wrappedValueForSourceValue(fun, fun->getFunctionType(), expressionCall);
    }

    if (llvm::InlineAsm *rawFun = scope->getInlineAsm(expressionCall->getName())) {
        vector<llvm::Value *>argValues;
        for (shared_ptr<Expression> &argumentExpression : expressionCall->getArgumentExpressions()) {
            llvm::Value *argValue = wrappedValueForExpression(argumentExpression)->getValue();
            argValues.push_back(argValue);
        }
        llvm::Value *resultValue = builder->CreateCall(rawFun, llvm::ArrayRef(argValues));
        return WrappedValue::wrappedValue(module, builder, resultValue, expressionCall->getValueType());
    }

    markErrorNotDefined(expressionCall->getLocation(), format("function \"{}\"", expressionCall->getName()));
    return nullptr;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionChained> expressionChained) {
    shared_ptr<WrappedValue> currentWrappedValue;
    shared_ptr<Expression> parentExpression;

    vector<shared_ptr<Expression>> chainExpressions = expressionChained->getChainExpressions();
    for (int i=0; i<chainExpressions.size(); i++) {
        shared_ptr<Expression> chainExpression = chainExpressions.at(i);

        // If the first expression is a cast, try doing a built-in on a type
        if (currentWrappedValue == nullptr && chainExpression->getKind() == ExpressionKind::CAST && chainExpressions.size() >= 2) {
            llvm::Type *type = typeForValueType(chainExpression->getValueType());
            shared_ptr<ExpressionValue> childExpressionValue = dynamic_pointer_cast<ExpressionValue>(chainExpressions.at(++i));
            currentWrappedValue = wrappedValueForTypeBuiltIn(type, childExpressionValue);
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
            currentWrappedValue = wrappedValueForCast(currentWrappedValue, expressionCast->getValueType());
            parentExpression = chainExpression;
            if (currentWrappedValue == nullptr)
                return nullptr;
            continue;
        }

        // Check parent expression
        shared_ptr<ExpressionValue> parentExpressionValue = dynamic_pointer_cast<ExpressionValue>(chainExpressions.at(i-1));
        if (parentExpressionValue == nullptr) {
            markErrorInvalidType(parentExpressionValue->getLocation());
            return nullptr;
        }

        // Built-in expression?
        if(shared_ptr<WrappedValue> builtInValue = wrappedValueForBuiltIn(currentWrappedValue, parentExpressionValue, chainExpression)) {
            currentWrappedValue = builtInValue;
            parentExpression = chainExpression;
            continue;
        }

        // Check chained expression type 
        shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(chainExpression);
        if (expressionValue == nullptr) {
            markErrorInvalidType(expressionValue->getLocation());
            return nullptr;
        }

        // Variable expression?
        if (!currentWrappedValue->isStruct()) {
            markErrorInvalidType(expressionValue->getLocation());
            return nullptr;
        }

        string blobName = *parentExpression->getValueType()->getBlobName();
        optional<int> memberIndex = scope->getStructMemberIndex(blobName, expressionValue->getIdentifier());
        if (!memberIndex) {
            markErrorInvalidMember(expressionValue->getLocation(), blobName, expressionValue->getIdentifier());
            return nullptr;
        }

        llvm::Value *index[] = {
            builder->getInt32(0),
            builder->getInt32(*memberIndex)
        };

        llvm::Value *elementPtr = builder->CreateGEP(currentWrappedValue->getStructType(), currentWrappedValue->getPointerValue(), index);
        llvm::Type *elementType = currentWrappedValue->getStructType()->getElementType(*memberIndex);

        currentWrappedValue = wrappedValueForSourceValue(elementPtr, elementType, expressionValue);
        parentExpression = chainExpression;
    }

    return currentWrappedValue;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    llvm::Type *type = typeForValueType(expressionCompositeLiteral->getValueType());
    if (type == nullptr)
        return nullptr;

    // First try making them constant
    if (expressionCompositeLiteral->getValueType()->isData()) {
        vector<llvm::Constant*> constantValues;
        int count = expressionCompositeLiteral->getValueType()->getValueArg();
        for (int i=0; i<count; i++) {
            shared_ptr<Expression> elementExpression = expressionCompositeLiteral->getExpressions().at(i);
            llvm::Value *value = wrappedValueForExpression(elementExpression)->getValue();
            llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
            if (constantValue == nullptr)
                goto not_constant;
            constantValues.push_back(constantValue);
        }
        llvm::ArrayType *arrayType = llvm::dyn_cast<llvm::ArrayType>(type);
        llvm::Constant *constantArray = llvm::ConstantArray::get(arrayType, constantValues);
        return WrappedValue::wrappedValue(module, builder, constantArray, expressionCompositeLiteral->getValueType());
    } else if (expressionCompositeLiteral->getValueType()->isBlob()) {
        vector<llvm::Constant*> constantValues;
        for (shared_ptr<Expression> memberExpression : expressionCompositeLiteral->getExpressions()) {
            llvm::Value *value = wrappedValueForExpression(memberExpression)->getValue();
            llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
            if (constantValue == nullptr)
                goto not_constant;
            constantValues.push_back(constantValue);
        }
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(type);
        llvm::Constant *constantStruct = llvm::ConstantStruct::get(structType, constantValues);
        return WrappedValue::wrappedValue(module, builder, constantStruct, expressionCompositeLiteral->getValueType());
    } else if (expressionCompositeLiteral->getValueType()->isPointer()) {
        llvm::Value *value = wrappedValueForExpression(expressionCompositeLiteral->getExpressions().at(0))->getValue();
        llvm::Constant *constantValue = llvm::dyn_cast<llvm::Constant>(value);
        if (constantValue == nullptr)
            goto not_constant;
        llvm::Constant *constant = llvm::ConstantExpr::getIntToPtr(constantValue, typePtr);
        return WrappedValue::wrappedValue(module, builder, constant, expressionCompositeLiteral->getValueType());
    }

    // Otherwise try normal dynamic alloca
    not_constant:
    if (builder->GetInsertBlock() == nullptr) {
        markErrorInvalidConstant(expressionCompositeLiteral->getLocation());
        return nullptr;
    }

    llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr);
    buildAssignment(
        WrappedValue::wrappedValue(module, builder, alloca, expressionCompositeLiteral->getValueType()),
        expressionCompositeLiteral
    );
    return WrappedValue::wrappedValue(module, builder, alloca, expressionCompositeLiteral->getValueType());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionGrouping> expressionGrouping) {
    return wrappedValueForExpression(expressionGrouping->getSubExpression());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionIfElse> expressionIfElse) {
    shared_ptr<Expression> conditionExpression = expressionIfElse->getConditionExpression();

    llvm::Function *fun = builder->GetInsertBlock()->getParent();
    llvm::Value *conditionValue = wrappedValueForExpression(conditionExpression)->getValue();

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
    llvm::Value *thenValue = wrappedValueForExpression(expressionIfElse->getThenExpression())->getValue();
    builder->CreateBr(mergeBlock);
    thenBlock = builder->GetInsertBlock();
    scope->popLevel();

    // Else
    llvm::Value *elseValue = nullptr;
    if (expressionIfElse->getElseExpression() != nullptr) {
        scope->pushLevel();
        fun->insert(fun->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);
        elseValue = wrappedValueForExpression(expressionIfElse->getElseExpression())->getValue();
        builder->CreateBr(mergeBlock);
        elseBlock = builder->GetInsertBlock();
        scope->popLevel();
    }

    // Merge
    fun->insert(fun->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    // we can only have a return value if else is also present and both then & else return the same type
    if (thenValue == nullptr || thenValue->getType()->isVoidTy() || elseValue == nullptr || thenValue->getType() != elseValue->getType()) {
        return WrappedValue::wrappedNone(typeVoid, expressionIfElse->getValueType());
    } else {
        llvm::PHINode *phi = builder->CreatePHI(thenValue->getType(), 2, "ifElseResult");
        phi->addIncoming(thenValue, thenBlock);
        phi->addIncoming(elseValue, elseBlock);

        return WrappedValue::wrappedValue(module, builder, phi, expressionIfElse->getValueType());
    }
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    llvm::Value *resultValue;
    switch (expressionLiteral->getValueType()->getKind()) {
        case ValueTypeKind::BOOL:
            resultValue = llvm::ConstantInt::getBool(typeBool, expressionLiteral->getBoolValue());
            break;

        case ValueTypeKind::INT:
            resultValue = llvm::ConstantInt::get(typeSInt, expressionLiteral->getSIntValue());
            break;

        case ValueTypeKind::U8:
            resultValue = llvm::ConstantInt::get(typeU8, expressionLiteral->getUIntValue());
            break;
        case ValueTypeKind::U32:
            resultValue = llvm::ConstantInt::get(typeU32, expressionLiteral->getUIntValue());
            break;
        case ValueTypeKind::U64:
            resultValue = llvm::ConstantInt::get(typeU64, expressionLiteral->getUIntValue());
            break;

        case ValueTypeKind::S8:
            resultValue = llvm::ConstantInt::get(typeS8, expressionLiteral->getSIntValue());
            break;
        case ValueTypeKind::S32:
            resultValue = llvm::ConstantInt::get(typeS32, expressionLiteral->getSIntValue());
            break;
        case ValueTypeKind::S64:
            resultValue = llvm::ConstantInt::get(typeS64, expressionLiteral->getSIntValue());
            break;

        case ValueTypeKind::FLOAT:
            resultValue = llvm::ConstantFP::get(typeFloat, expressionLiteral->getFloatValue());
            break;

        case ValueTypeKind::F32:
            resultValue = llvm::ConstantFP::get(typeF32, expressionLiteral->getFloatValue());
            break;
        case ValueTypeKind::F64:
            resultValue = llvm::ConstantFP::get(typeF64, expressionLiteral->getFloatValue());
            break;

        default:
            break;
    }

    if (resultValue == nullptr) {
        markErrorInvalidLiteral(expressionLiteral->getLocation(), expressionLiteral->getValueType());
        return nullptr;
    }

    return WrappedValue::wrappedValue(module, builder, resultValue, expressionLiteral->getValueType());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    shared_ptr<ValueType> valueType = expressionUnary->getSubExpression()->getValueType();
    llvm::Value *value = wrappedValueForExpression(expressionUnary->getSubExpression())->getValue();

    llvm::Value *resultValue;
    switch (expressionUnary->getOperation()) {
        case ExpressionUnaryOperation::BIT_NOT:
        case ExpressionUnaryOperation::NOT: {
            if (valueType->isBool() || valueType->isInteger())
                resultValue = builder->CreateNot(value);
            break;
        }
        case ExpressionUnaryOperation::MINUS: {
            if (valueType->isUnsignedInteger()) {
                resultValue = builder->CreateNeg(value);
            } else if (valueType->isSignedInteger()) {
                resultValue = builder->CreateNSWNeg(value);
            } else if (valueType->isFloat()) {
                resultValue = builder->CreateFNeg(value);
            }
            break;
        }
        case ExpressionUnaryOperation::PLUS: {
            if (valueType->isNumeric())
                resultValue = value;
            break;
        }
    }

    if (resultValue == nullptr) {
        markErrorInvalidOperationUnary(expressionUnary->getLocation(), expressionUnary->getOperation(), valueType);
        return nullptr;
    }

    return WrappedValue::wrappedValue(module, builder, resultValue, expressionUnary->getValueType());
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForExpression(shared_ptr<ExpressionValue> expressionValue) {
    llvm::Value *value = nullptr;
    llvm::Type *type = nullptr;

    shared_ptr<WrappedValue> wrappedValue = scope->getWrappedValue(expressionValue->getIdentifier());
    llvm::Value *fun = scope->getFunction(expressionValue->getIdentifier());
    if (wrappedValue != nullptr) {
        value = wrappedValue->getPointerValue();
        type = wrappedValue->getType();
    } else if (fun != nullptr) {
        value = fun;
        type = fun->getType();
    }

    if (value == nullptr) {
        markErrorNotDefined(expressionValue->getLocation(), format("variable \"{}\"", expressionValue->getIdentifier()));
        return nullptr;
    }

    return wrappedValueForSourceValue(value, type, expressionValue);
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForBuiltIn(shared_ptr<WrappedValue> parentWrappedValue, shared_ptr<ExpressionValue> parentExpression, shared_ptr<Expression> expression) {
    bool isCount = false;
    bool isVal = false;
    bool isVadr = false;
    bool isAdr = false;
    bool isSize = false;

    shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(expression);
    shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression);

    if (expressionValue != nullptr) {
        isCount = expressionValue->getIdentifier().compare("count") == 0;
        isVal = expressionValue->getIdentifier().compare("val") == 0;
        isVadr = expressionValue->getIdentifier().compare("vadr") == 0;
        isAdr = expressionValue->getIdentifier().compare("adr") == 0;
        isSize = expressionValue->getIdentifier().compare("size") == 0;
    } else if (expressionCall != nullptr) {
        isVal = expressionCall->getName().compare("val") == 0;
    }

    // Return quickly if not a built-in
    if (!isCount && !isVal && !isVadr && !isAdr && !isSize)
        return nullptr;

    // Do the appropriate built-in operation
    if (parentWrappedValue->isArray() && isCount) {
        return WrappedValue::wrappedUIntValue(typeUInt, parentWrappedValue->getArrayType()->getNumElements(), ValueType::INT);
    } else if (parentWrappedValue->isPointer() && isVal) {
        llvm::LoadInst *pointeeLoad = builder->CreateLoad(typePtr, parentWrappedValue->getPointerValue());
        pointeeLoad->setVolatile(true);

        shared_ptr<ValueType> pointeeValueType = parentExpression->getValueType()->getSubType();
        if (pointeeValueType == nullptr) {
            markErrorNoTypeForPointer(parentExpression->getLocation());
            return nullptr;
        }
        llvm::Type *pointeeType = typeForValueType(pointeeValueType);
        if (pointeeType == nullptr) {
            markErrorNoTypeForPointer(parentExpression->getLocation());
            return nullptr; 
        }

        return wrappedValueForSourceValue(pointeeLoad, pointeeType, expression);
    } else if (parentWrappedValue->isPointer() && isVadr) {
        llvm::LoadInst *pointeeLoad = (llvm::LoadInst*)builder->CreateLoad(typePtr, parentWrappedValue->getPointerValue());
        pointeeLoad->setVolatile(true);
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreatePtrToInt(pointeeLoad, typeA),
            ValueType::A
        );
    } else if (isAdr) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreatePtrToInt(parentWrappedValue->getPointerValue(), typeA),
            ValueType::A
        );
    } else if (isSize) {
        int sizeInBytes = sizeInBitsForType(parentWrappedValue->getType()) / 8;
        if (sizeInBytes <= 0)
            return nullptr;
        return WrappedValue::wrappedUIntValue(typeUInt, sizeInBytes, ValueType::INT);
    }

    markErrorInvalidBuiltIn(expression->getLocation(), expressionValue->getIdentifier());
    return nullptr;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForCast(shared_ptr<WrappedValue> sourceWrappedValue, shared_ptr<ValueType> targetValueType) {
    // Figure out source type
    bool isSourceUInt = false;
    bool isSourceSInt = false;
    bool isSourceFloat = false;
    bool isSourceData = false;
    int sourceSize = 0;
    switch (sourceWrappedValue->getValueType()->getKind()) {
        case ValueTypeKind::INT: {
            isSourceUInt = true;
            //isSourceSInt = true;
            sourceSize = typeUInt->getBitWidth();
            break;
        }
        case ValueTypeKind::U8:
            isSourceUInt = true;
            sourceSize = 8;
            break;
        case ValueTypeKind::U32:
            isSourceUInt = true;
            sourceSize = 32;
            break;
        case ValueTypeKind::U64:
            isSourceUInt = true;
            sourceSize = 64;
            break;
        case ValueTypeKind::S8:
            isSourceSInt = true;
            sourceSize = 8;
            break;
        case ValueTypeKind::S32:
            isSourceSInt = true;
            sourceSize = 32;
            break;
        case ValueTypeKind::S64:
            isSourceSInt = true;
            sourceSize = 64;
            break;
        case ValueTypeKind::FLOAT: {
            isSourceFloat = true;
            sourceSize = 32;
            break;
        }
        case ValueTypeKind::F32:
            isSourceFloat = true;
            sourceSize = 32;
            break;
        case ValueTypeKind::F64:
            isSourceFloat = true;
            sourceSize = 64;
            break;
        case ValueTypeKind::DATA: {
            isSourceData = true;
            if (shared_ptr<ExpressionLiteral> expressionLiteral = dynamic_pointer_cast<ExpressionLiteral>(sourceWrappedValue->getValueType()->getCountExpression())) {
                sourceSize = expressionLiteral->getUIntValue();
            }
            break;
        }
        default:
            markErrorInvalidCast(nullptr);
            return nullptr;
    }

    // Figure out target type
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
        case ValueTypeKind::DATA: {
            isTargetData = true;
            if (shared_ptr<ExpressionLiteral> expressionLiteral = dynamic_pointer_cast<ExpressionLiteral>(targetValueType->getCountExpression())) {
                targetSize = expressionLiteral->getUIntValue();
            }
            break;
        }
        default:
            markErrorInvalidCast(nullptr);
            return nullptr;
    }

    llvm::Type *targetType = typeForValueType(targetValueType);
    // uint to int+
    if (isSourceUInt && (isTargetUInt || isTargetSInt) && targetSize >= sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateZExt(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // uint to int-
    } if (isSourceUInt && (isTargetUInt || isTargetSInt) && targetSize < sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateTrunc(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // sint to sint+
    } else if (isSourceSInt && isTargetSInt && targetSize >= sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateSExt(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // sint to sint-
    } else if (isSourceSInt && isTargetSInt && targetSize < sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateTrunc(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // sint to uint+
    } else if (isSourceSInt && isTargetUInt && targetSize >= sourceSize) {
        // clamp value to 0 if negative
        llvm::Constant *constantZero = llvm::ConstantInt::get(sourceWrappedValue->getType(), 0);
        llvm::Value *compareToZero = builder->CreateICmpSLT(sourceWrappedValue->getValue(), constantZero);
        llvm::Value *clampedValue = builder->CreateSelect(compareToZero, constantZero, sourceWrappedValue->getValue());
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateZExt(clampedValue, targetType),
            targetValueType
        );
    // sint to uint-
    } else if (isSourceSInt && isTargetUInt && targetSize < sourceSize) {
        // clamp value to 0 if negative
        llvm::Constant *constantZero = llvm::ConstantInt::get(sourceWrappedValue->getType(), 0);
        llvm::Value *compareToZero = builder->CreateICmpSLT(sourceWrappedValue->getValue(), constantZero);
        llvm::Value *clampedValue = builder->CreateSelect(compareToZero, constantZero, sourceWrappedValue->getValue());
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateTrunc(clampedValue, targetType),
            targetValueType
        );
    // uint to float
    } else if (isSourceUInt && isTargetFloat) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateUIToFP(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // sint to float
    } else if (isSourceSInt && isTargetFloat) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateSIToFP(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // float to float+
    } else if (isSourceFloat && isTargetFloat && targetSize >= sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateFPExt(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // float to float-
    } else if (isSourceFloat && isTargetFloat && targetSize < sourceSize) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateFPTrunc(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // float to uint
    } else if (isSourceFloat && isTargetUInt) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateFPToUI(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // float to sint
    } else if (isSourceFloat && isTargetSInt) {
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateFPToSI(sourceWrappedValue->getValue(), targetType),
            targetValueType
        );
    // data to data
    } else if (isSourceData && isTargetData) {
        llvm::AllocaInst *targetAlloca = builder->CreateAlloca(targetType);

        int elementsCount = min(sourceSize, targetSize);

        bool areElementTypesSame = sourceWrappedValue->getValueType()->getSubType()->isEqual(targetValueType->getSubType());
        // Do we just adjust the count or do we need to cast each of the element
        if (areElementTypesSame) {
            for (int i=0; i<elementsCount; i++) {
                llvm::Value *index[] = {
                    builder->getInt32(0),
                    builder->getInt32(i)
                };

                // get pointers for both source and target
                llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceWrappedValue->getArrayType(), sourceWrappedValue->getPointerValue(), index);
                llvm::Value *targetMemberPtr = builder->CreateGEP(targetType, targetAlloca, index);
                // load value from source pointer
                llvm::Value *sourceMemberValue = builder->CreateLoad(sourceWrappedValue->getArrayType()->getArrayElementType(), sourceMemberPtr);
                // and then store it at the target pointer
                builder->CreateStore(sourceMemberValue, targetMemberPtr);
            }
        } else {
            // iterate over each of the member
            for (int i=0; i<elementsCount; i++) {
                llvm::Value *index[] = {
                    builder->getInt32(0),
                    builder->getInt32(i)
                };

                // constant or non-constant copy?
                llvm::Value *sourceMemberValue;
                if (llvm::Constant *sourceConstantValue = sourceWrappedValue->getConstantValue()) {
                    sourceMemberValue = sourceConstantValue->getAggregateElement(i);
                } else {
                    llvm::Value *sourceMemberPtr = builder->CreateGEP(sourceWrappedValue->getArrayType(), sourceWrappedValue->getPointerValue(), index);
                    sourceMemberValue = builder->CreateLoad(sourceWrappedValue->getArrayType()->getArrayElementType(), sourceMemberPtr);
                }

                // cast the individual source member to target type
                shared_ptr<WrappedValue> castSourceMemberValue = wrappedValueForCast(
                    WrappedValue::wrappedValue(
                        module,
                        builder,
                        sourceMemberValue,
                        sourceWrappedValue->getValueType()->getSubType()
                    ),
                    targetValueType->getSubType()
                );
                if (castSourceMemberValue == nullptr)
                    return nullptr;

                // get the target member
                llvm::Value *targetMemberPtr = builder->CreateGEP(targetType, targetAlloca, index);

                // and finally store source member in the target member
                builder->CreateStore(castSourceMemberValue->getValue(), targetMemberPtr);
            }
        }

        return WrappedValue::wrappedValue(
            module,
            builder,
            targetAlloca,
            targetValueType
        );
    } else {
        markErrorInvalidCast(nullptr);
        return nullptr;
    }
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType, shared_ptr<Expression> expression) {
    if (builder->GetInsertBlock() == nullptr)
        return nullptr;

    if (shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(expression)) {
        switch (expressionValue->getValueKind()) {
            case ExpressionValueKind::FUN:
            case ExpressionValueKind::SIMPLE:
            case ExpressionValueKind::BUILT_IN_VAL_SIMPLE: {
                llvm::LoadInst *loadInst = builder->CreateLoad(sourceType, sourceValue, expressionValue->getIdentifier());
                loadInst->setVolatile(true);
                return WrappedValue::wrappedValue(
                    module,
                    builder,
                    loadInst,
                    expression->getValueType()
                );
            }
            case ExpressionValueKind::DATA: 
            case ExpressionValueKind::BUILT_IN_VAL_DATA: {
                llvm::Value *indexValue = wrappedValueForExpression(expressionValue->getIndexExpression())->getValue();
                if (indexValue == nullptr)
                    return nullptr;
                llvm::Value *index[] = {
                    builder->getInt32(0),
                    indexValue
                };
                llvm::Type *expType = llvm::ArrayType::get(typeForValueType(expression->getValueType()), 0); // TODO: this is hack and should be fixed
                llvm::ArrayType *sourceArrayType = llvm::dyn_cast<llvm::ArrayType>(expType);
                llvm::Value *elementPtr = builder->CreateGEP(sourceArrayType, sourceValue, index, format("{}[]", expressionValue->getIdentifier()));
                return WrappedValue::wrappedValue(
                    module,
                    builder,
                    builder->CreateLoad(sourceArrayType->getArrayElementType(), elementPtr),
                    expression->getValueType()
                );
            }
            default: {
                break;
            }
        }
    } else if (shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression)) {
        llvm::FunctionType *funType = llvm::dyn_cast<llvm::FunctionType>(sourceType);
        vector<llvm::Value*> argValues;
        for (shared_ptr<Expression> argumentExpression : expressionCall->getArgumentExpressions()) {
            shared_ptr<WrappedValue> wrappedvalue = wrappedValueForExpression(argumentExpression);
            argValues.push_back(wrappedvalue->getValue());
        }
        return WrappedValue::wrappedValue(
            module,
            builder,
            builder->CreateCall(funType, sourceValue, llvm::ArrayRef(argValues)),
            expression->getValueType()
        );
    }
    return nullptr;
}

shared_ptr<WrappedValue> ModuleBuilder::wrappedValueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionValue> expression) {
    bool isSize = expression->getIdentifier().compare("size") == 0;

    if (isSize) {
        int sizeInBytes = sizeInBitsForType(type) / 8;
        if (sizeInBytes <= 0)
            return nullptr;
        return WrappedValue::wrappedUIntValue(typeUInt, sizeInBytes, expression->getValueType());
    }
    
    markErrorInvalidBuiltIn(expression->getLocation(), expression->getIdentifier());
    return nullptr;
}

//
// Support
//
llvm::Type *ModuleBuilder::typeForValueType(shared_ptr<ValueType> valueType, shared_ptr<Location> location) {
    if (valueType == nullptr) {
        markErrorInvalidType(location);
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
        case ValueTypeKind::A:
            return typeA;
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
                markErrorNotDefined(nullptr, format("blob \"{}\"", *(valueType->getBlobName())));
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
        case ValueTypeKind::PTR: {
            return typePtr;
        }
        // If the type is composite then it is valid but has never beend promoted to an actual type,
        // because it's not used, so we just ignore it
        case ValueTypeKind::COMPOSITE: {
            return nullptr;
        }
        default:
            break;
    }

    markErrorInvalidType(location);
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
        return typeA->getBitWidth();
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

//
// Error Handling
//

void ModuleBuilder::markFunctionError(string functionName, string message) {
    errors.push_back(Error::builderFunctionError(functionName, message));
}

void ModuleBuilder::markModuleError(string message) {
    errors.push_back(Error::builderModuleError(moduleName, message));
}

void ModuleBuilder::markErrorAlreadyDefined(shared_ptr<Location> location, string name) {
    string message = format("{} has been already defined in scope", name);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markInvalidConstraints(shared_ptr<Location> location, string functionName, string constraints) {
    string message = format("Constraints \"{}\" for function \"{}\" are invalid", constraints, functionName);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidAssignment(shared_ptr<Location> location) {
    string message = "Invalid assignment";
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidBuiltIn(shared_ptr<Location> location, string name) {
    string message = format("Invalid built-in operation\"{}\"", name);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidCast(shared_ptr<Location> location) {
    string message = "Invalid cast";
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidConstant(shared_ptr<Location> location) {
    string message = "Invalid constant";
    errors.push_back(Error::error(location, message));   
}

void ModuleBuilder::markErrorInvalidGlobal(shared_ptr<Location> location) {
    string message = "Invalid global";
    errors.push_back(Error::error(location, message));   
}

void ModuleBuilder::markErrorInvalidImport(shared_ptr<Location> location, string moduleName) {
    string message = format("Invalid import, module \"{}\" doesn't exist", moduleName);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidLiteral(shared_ptr<Location> location, shared_ptr<ValueType> type) {
    string message = format("Invalid literal for type {}", Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidMember(shared_ptr<Location> location, string blobName, string memberName) {
    string message = format("Invalid member \"{}\" for \"blob<{}>\"", memberName, blobName);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidOperationBinary(shared_ptr<Location> location, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    string message = format(
        "Invalid binary operation {} for types {} and {}",
        Logger::toString(operation),
        Logger::toString(firstType),
        Logger::toString(secondType)
    );
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidOperationUnary(shared_ptr<Location> location, ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    string message = format(
        "Invalid unary operation {} for type {}",
        Logger::toString(operation),
        Logger::toString(type)
    );
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorInvalidType(shared_ptr<Location> location) {
    string message = "Invalid type";
    errors.push_back(Error::error(location, message));   
}

void ModuleBuilder::markErrorUnexpected(shared_ptr<Location> location, string name) {
    string message = format("Unexpected {}", name);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorNotDeclared(shared_ptr<Location> location, string name) {
    string message = format("{} is not declared in scope", name);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorNotDefined(shared_ptr<Location> location, string name) {
    string message = format("{} is not defined in scope", name);
    errors.push_back(Error::error(location, message));
}

void ModuleBuilder::markErrorNoTypeForPointer(shared_ptr<Location> location) {
    string message = "Cannot find type for pointer";
    errors.push_back(Error::error(location, message));
}