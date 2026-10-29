#include "Analyzer.h"

#include "Analyzer/AnalyzerScope/AnalyzerScope.h"
#include "Analyzer/AnalyzerScope/AnalyzerScopeBlob.h"
#include "Error.h"
#include "Logger.h"
#include "Module/Module.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionCast.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionMatch.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionValue.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementEnum.h"
#include "Parser/Statement/StatementExchange.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementMetaImport.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementProto.h"
#include "Parser/Statement/StatementProtoDeclaration.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"

#include "Parser/ValueType/ValueType.h"
#include "Parser/ValueType/ValueTypeBoxed.h"
#include "Parser/ValueType/ValueTypeBlob.h"
#include "Parser/ValueType/ValueTypeComposite.h"
#include "Parser/ValueType/ValueTypeData.h"
#include "Parser/ValueType/ValueTypeEnum.h"
#include "Parser/ValueType/ValueTypeEnumField.h"
#include "Parser/ValueType/ValueTypeFun.h"
#include "Parser/ValueType/ValueTypeProto.h"
#include "Parser/ValueType/ValueTypePtr.h"
#include "Parser/ValueType/ValueTypeSimple.h"

#include "Parser/Pattern.h"

using namespace std;

static vector<string> illegalBlobFieldNames = {"adr", "size"};

// MARK: - Public

Analyzer::Analyzer(
    const string &defaultModuleName,
    shared_ptr<Module> module,
    const map<string, vector<shared_ptr<Statement>>> &importableHeaderStatementsMap
):
defaultModuleName(defaultModuleName),
module(module),
importableHeaderStatementsMap(importableHeaderStatementsMap) { }

void Analyzer::checkModule() {
    scope = make_shared<AnalyzerScope>();

    // check header
    for (shared_ptr<Statement> statement : module->getHeaderStatements())
        checkStatement(statement, nullptr);

    // check blob member functions
    for (shared_ptr<Statement> headerStatement : module->getHeaderStatements()) {
        if (shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(headerStatement)) {
            for (shared_ptr<StatementFunction> statementFunction : statementBlob->getStatementFunctions()) {
                checkStatement(statementFunction);
            }
        }
    }

    // check body
    for (shared_ptr<Statement> statement : module->getBodyStatements()) {
            checkStatement(statement, nullptr);
    }

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }
}

// MARK: - Private

// Statements

void Analyzer::checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType, bool isImported, ImportLevel importLevel) {
    switch (statement->getKind()) {
        case StatementKind::ASSIGNMENT: {
            checkStatement(dynamic_pointer_cast<StatementAssignment>(statement));
            return;
        }
        case StatementKind::BLOB: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementBlob>(statement), isImported);
            return;
        }
        case StatementKind::BLOB_DECLARATION: {
            checkStatement(dynamic_pointer_cast<StatementBlobDeclaration>(statement));
            return;
        }
        case StatementKind::BLOCK: {
            checkStatement(dynamic_pointer_cast<StatementBlock>(statement), returnType);
            return;
        }
        case StatementKind::ENUM: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementEnum>(statement));
            return;
        }
        case StatementKind::EXCHANGE: {
            checkStatement(dynamic_pointer_cast<StatementExchange>(statement));
            return;
        }
        case StatementKind::EXPRESSION: {
            checkStatement(dynamic_pointer_cast<StatementExpression>(statement), returnType);
            return;
        }
        case StatementKind::FUNCTION: {
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            return;
        }
        case StatementKind::FUNCTION_DECLARATION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            return;
        }
        case StatementKind::META_EXTERN_FUNCTION: {
            checkStatement(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
            return;
        }
        case StatementKind::META_EXTERN_VARIABLE: {
            checkStatement(dynamic_pointer_cast<StatementMetaExternVariable>(statement));
            return;
        }
        case StatementKind::META_IMPORT: {
            ImportLevel newImportLevel = ImportLevel::IMPLICIT;
            if (importLevel == ImportLevel::NONE)
                newImportLevel = ImportLevel::EXPLICIT;
            checkStatement(dynamic_pointer_cast<StatementMetaImport>(statement), newImportLevel);
            return;
        }
        case StatementKind::PROTO: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementProto>(statement));
            return;
        }
        case StatementKind::PROTO_DECLARATION: {
            checkStatement(dynamic_pointer_cast<StatementProtoDeclaration>(statement));
            return;
        }
        case StatementKind::RAW_FUNCTION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementRawFunction>(statement));
            return;
        }
        case StatementKind::REPEAT: {
            checkStatement(dynamic_pointer_cast<StatementRepeat>(statement), returnType);
            return;
        }
        case StatementKind::RETURN: {
            checkStatement(dynamic_pointer_cast<StatementReturn>(statement), returnType);
            return;
        }
        case StatementKind::VARIABLE: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementVariable>(statement));
            return;
        }
        case StatementKind::VARIABLE_DECLARATION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            return;
        }
        default:
            break;
    }

    markErrorUnexpected(statement->getLocation(), "statement");
}

void Analyzer::checkStatement(shared_ptr<StatementAssignment> statementAssignment) {
    shared_ptr<ValueType> targetType = typeForExpression(statementAssignment->getExpressionChained());
    if (targetType == nullptr)
        return;

    targetType = typeForCheckedValueType(targetType, false);
    if (targetType == nullptr)
        return;

    statementAssignment->valueExpression = checkAndTryCasting(statementAssignment->getValueExpression(), targetType, nullptr);
    if (statementAssignment->getValueExpression() == nullptr)
        return;

    shared_ptr<ValueType> sourceType = statementAssignment->getValueExpression()->getValueType();
    if (sourceType != nullptr && !sourceType->isEqual(targetType)) {
        markErrorInvalidType(
            statementAssignment->getValueExpression()->getLocation(),
            sourceType,
            targetType
        );
    }
}

void Analyzer::checkStatement(shared_ptr<StatementBlob> statementBlob, bool isImported) {
    // first register named value types for the blob
    scope->blobScope->registerNamedValueTypeKeys(statementBlob->getSymbolName(), statementBlob->getNamedTypeKeys());

    // then check and verify blob field variables
    if (!scope->level([&]() -> bool {
        for (shared_ptr<StatementVariableDeclaration> statementVariableDeclaration : statementBlob->getStatementVariableDeclarations()) {
            // check for invalid field names
            for (string &illegalName : illegalBlobFieldNames) {
                if (statementVariableDeclaration->getIdentifier() == illegalName) {
                    markErrorNameNotAllowed(statementVariableDeclaration->getLocation(), statementVariableDeclaration->getIdentifier());
                    return false;
                }
            }

            // fields should not have @export
            if (statementVariableDeclaration->getShouldExport()) {
                markErrorInvalidAttribute(statementVariableDeclaration->getLocation(), "@export");
                return false;
            }

            checkStatement(statementVariableDeclaration);
            if (statementVariableDeclaration->getValueType() == nullptr)
                return false;
        }

        return true;
    })) { return; }

    // verify field functions
    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getStatementFunctions()) {
        // fields should not have export
        if (statementFunction->getShouldExport()) {
            markErrorInvalidAttribute(statementFunction->getLocation(), "@export");
            return;
        }
    }

    // verify proto compliance (but only if it's not an import statement)
    if (!isImported) {
        for (shared_ptr<SymbolName> protoSymbolName : statementBlob->getProtoSymbolNames()) {
            optional<vector<pair<string, shared_ptr<ValueType>>>> oProtoFields = scope->protoScope->getFields(protoSymbolName);
            if (!oProtoFields) {
                markErrorNotDefined(statementBlob->getLocation(), format("proto {}", protoSymbolName->getGlobalName()));
                return;
            }

            // for each proto field
            for (pair<string, shared_ptr<ValueType>> protoField : *oProtoFields) {
                bool isImplemented = false;

                if (protoField.second->isFun()) {
                    string funName = format("{}.{}", statementBlob->getSymbolName()->getName(), protoField.first);
                    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getStatementFunctions()) {
                        // check function name
                        if (funName != statementFunction->getName())
                            continue;

                        // check function arguments
                        int argsCount = (protoField.second->toFun()->getArgumentValueTypes()).size();

                        // count
                        if (argsCount != statementFunction->getArguments().size()) {
                            isImplemented = false;
                            goto not_implemented;
                        }

                        // types
                        for (int i=1; i<argsCount; i++) {
                            if (!protoField.second->toFun()->getArgumentValueTypes().at(i)->isEqual(statementFunction->getArguments().at(i).second)) {
                                isImplemented = false;
                                goto not_implemented;
                            }
                        }

                        // return type
                        if (!protoField.second->toFun()->getReturnValueType()->isEqual(statementFunction->getReturnValueType())) {
                            isImplemented = false;
                            goto not_implemented;
                        }

                        isImplemented = true;
                    }
                } else {
                    for (shared_ptr<StatementVariableDeclaration> statementVariableDeclaration : statementBlob->getStatementVariableDeclarations()) {
                        if (protoField.first == statementVariableDeclaration->getIdentifier() && protoField.second->isEqual(statementVariableDeclaration->getValueType())) {
                            isImplemented = true;
                            break;
                        }
                    }
                }

                not_implemented:
                if (!isImplemented) {
                    markErrorNotImplemented(statementBlob->getLocation(), protoSymbolName->getGlobalName(), protoField.first);
                    return;
                }
            }
        }
    }

    // register blob fields in scope
    vector<pair<string, shared_ptr<ValueType>>> fields;

    // extract variable fields
    for (shared_ptr<StatementVariableDeclaration> statementVariableDeclaration : statementBlob->getStatementVariableDeclarations())
        fields.push_back(pair(statementVariableDeclaration->getIdentifier(), statementVariableDeclaration->getValueType()));

    // then function fields
    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getStatementFunctions())
        fields.push_back(pair(statementFunction->getName(), statementFunction->getValueType()));

    // check each of the extracted fields's type
    for (auto &field : fields) {
        if (typeForCheckedValueType(field.second, true) == nullptr)
            return;
    }

    // and the register
    scope->blobScope->registerFields(statementBlob->getSymbolName(), fields);
    scope->blobScope->registerConformingProtoSymbolNames(statementBlob->getSymbolName(), statementBlob->getProtoSymbolNames());
}

void Analyzer::checkStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration) {
    scope->blobScope->registerDeclaration(statementBlobDeclaration->getSymbolName());
}

void Analyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementEnum> statementEnum) {
    // check fields
    shared_ptr<Expression> currentTagExpression = nullptr;
    for (EnumField &field : statementEnum->fields) {
        // Only none or boxed types are valid
        if (!field.payloadValueType->isBoxed() && field.payloadValueType->getKind() != ValueTypeKind::NONE) {
            markErrorInvalidType(field.payloadValueType->getLocation(), field.payloadValueType, nullptr);
            return;
        }

        // Make sure each field has a value expression
        if (field.tagExpression == nullptr) {
            // if none present, initialize from zero
            if (currentTagExpression == nullptr) {
                currentTagExpression = ExpressionLiteral::expressionLiteralForUInt(0, statementEnum->getLocation());
            // otherwise just add 1 to the current value
            } else {
                currentTagExpression = ExpressionBinary::expression(
                    ExpressionBinaryOperation::ADD,
                    currentTagExpression,
                    ExpressionLiteral::expressionLiteralForUInt(1, statementEnum->getLocation()),
                    statementEnum->getLocation()
                );
            }

            field.tagExpression = currentTagExpression;
        } else {
            currentTagExpression = field.tagExpression;
        }
        currentTagExpression->valueType = typeForExpression(currentTagExpression, nullptr, nullptr);

        // Register enum field type
        AnalyzerScopeRegisterResult keysRegisterResult = scope->enumScope->registerNamedValueTypeKeys(field.symbolName, statementEnum->getNamedValueTypeKeys());
        if (keysRegisterResult != AnalyzerScopeRegisterResult::SUCCES) {
            markErrorInvalidRegistration(
                field.payloadValueType->getLocation(),
                field.symbolName->getGlobalName(),
                keysRegisterResult
            );
            return;
        }
        AnalyzerScopeRegisterResult payloadRegisterResult = scope->enumScope->registerPayloadValueType(field.symbolName, field.payloadValueType);
        if (payloadRegisterResult != AnalyzerScopeRegisterResult::SUCCES) {
            markErrorInvalidRegistration(
                field.payloadValueType->getLocation(),
                field.symbolName->getGlobalName(),
                payloadRegisterResult
            );
            return;
        }
    }

   // register enum type
   AnalyzerScopeRegisterResult registerResult = scope->enumScope->registerNamedValueTypeKeys(statementEnum->getSymbolName(), statementEnum->getNamedValueTypeKeys());
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(
            statementEnum->getLocation(),
            statementEnum->getSymbolName()->getGlobalName(),
            registerResult
        );
        return;
    }
}

void Analyzer::checkStatement(std::shared_ptr<StatementExchange> statementExchange) {
    statementExchange->getLeftExpression()->valueType = typeForExpression(statementExchange->getLeftExpression(), nullptr, nullptr);
    statementExchange->getRightExpression()->valueType = typeForExpression(statementExchange->getRightExpression(), nullptr, nullptr);

    if (statementExchange->getLeftExpression()->getValueType() == nullptr || statementExchange->getRightExpression()->getValueType() == nullptr)
        return;

    // Value types have to be the same
    if (!statementExchange->getLeftExpression()->getValueType()->isEqual(statementExchange->getRightExpression()->getValueType()))
        markErrorInvalidType(statementExchange->getLocation(), statementExchange->getLeftExpression()->getValueType(), nullptr);
}

void Analyzer::checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType) {
    // returned value type is ignored
    statementExpression->getExpression()->valueType = typeForExpression(statementExpression->getExpression(), nullptr, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    // check argument types
    for (pair<string, shared_ptr<ValueType>> &argument : statementFunction->getArguments()) {
        if (typeForCheckedValueType(argument.second, true) == nullptr)
            return;
    }

    // check return type
    if (typeForCheckedValueType(statementFunction->getReturnValueType(), true) == nullptr)
        return;

    // register function definition
    AnalyzerScopeRegisterResult registerResult = scope->functionScope->registerValueType(
        statementFunction->getGlobalName(),
        statementFunction->getValueType(),
        true
    );
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(
            statementFunction->getLocation(),
            statementFunction->getGlobalName(),
            registerResult
        );
        return;
    }

    scope->level([&]() -> bool {
        // register arguments as variables
        for (auto &argument : statementFunction->getArguments()) {
            AnalyzerScopeRegisterResult registerResult = scope->variableScope->registerValueType(argument.first, argument.second, true);
            if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
                markErrorInvalidRegistration(argument.second->getLocation(), argument.first, registerResult);
                return false;
            }
        }

        checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());

        return true;
    });
}

void Analyzer::checkStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration) {
    // check argument types
    for (auto &argument : statementFunctionDeclaration->getArguments()) {
        if (typeForCheckedValueType(argument.second, true) == nullptr)
            return;
    }

    // check return type
    if (typeForCheckedValueType(statementFunctionDeclaration->getReturnValueType(), true) == nullptr)
        return;

    // register function declaration
    AnalyzerScopeRegisterResult registerResult = scope->functionScope->registerValueType(
        statementFunctionDeclaration->getGlobalName(),
        statementFunctionDeclaration->getValueType(),
        false
    );
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(
            statementFunctionDeclaration->getLocation(),
            statementFunctionDeclaration->getGlobalName(),
            registerResult
        );
        return;
    }
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction) {
    // check argument types
    for (auto &argument : statementMetaExternFunction->getArguments()) {
        if (typeForCheckedValueType(argument.second, true) == nullptr)
            return;
    }

    // check return type
    if (typeForCheckedValueType(statementMetaExternFunction->getReturnValueType(), true) == nullptr)
        return;

    // register function declaration
    AnalyzerScopeRegisterResult registerResult = scope->functionScope->registerValueType(
        statementMetaExternFunction->getGlobalName(),
        statementMetaExternFunction->getValueType(),
        false
    );
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(
            statementMetaExternFunction->getLocation(),
            statementMetaExternFunction->getGlobalName(),
            registerResult
        );
        return;
    }
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable) {
   string identifier = statementMetaExternVariable->getGlobalIdentifier();

    AnalyzerScopeRegisterResult registerResult = scope->variableScope->registerValueType(identifier, statementMetaExternVariable->getValueType(), false);
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(statementMetaExternVariable->getLocation(), identifier, registerResult);
        return;
    }
}

void Analyzer::checkStatement(shared_ptr<StatementMetaImport> statementMetaImport, ImportLevel importLevel) {
    // Check if import exits
    auto it = importableHeaderStatementsMap.find(statementMetaImport->getName());
    if (it == importableHeaderStatementsMap.end()) {
        markErrorInvalidImport(statementMetaImport->getLocation(), statementMetaImport->getName());
        return;
    }

    // Skip if import circles back
    if (statementMetaImport->getName() == module->getName())
        return;
    
    // Check already imported levels
    ImportLevel currentImport = importedModuleLevelsMap[statementMetaImport->getName()];
    if (
        currentImport == ImportLevel::EXPLICIT ||
        currentImport == ImportLevel::IMPLICIT && importLevel != ImportLevel::EXPLICIT
    ) {
        return;
    }
    importedModuleLevelsMap[statementMetaImport->getName()] = importLevel;

    for (shared_ptr<Statement> &importedStatement : it->second) {
        checkStatement(importedStatement, nullptr, true, importLevel);
    }
}

void Analyzer::checkStatement(shared_ptr<StatementProto> statement) {
    // check and verify proto field variables
    if (!scope->level([&]() -> bool {
        for (shared_ptr<StatementVariableDeclaration> statementVariableDeclaration : statement->getStatementVariableDeclarations()) {
            // fields should not have @export
            if (statementVariableDeclaration->getShouldExport()) {
                markErrorInvalidAttribute(statementVariableDeclaration->getLocation(), "@export");
                return false;
            }
            
            // proto fields have to have an identifier
            if (statementVariableDeclaration->getIdentifier().empty()) {
                markErrorInvalidName(statementVariableDeclaration->getLocation(), statementVariableDeclaration->getIdentifier());
                return false;
            }

            checkStatement(statementVariableDeclaration);
            if (statementVariableDeclaration->getValueType() == nullptr)
                return false;
        }

        return true;
    })) { return; }

    // verify field function declarations
    for (shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration : statement->getStatementFunctionDeclarations()) {
        // members should not have export
        if (statementFunctionDeclaration->getShouldExport()) {
            markErrorInvalidAttribute(statementFunctionDeclaration->getLocation(), "@export");
            return;
        }

        checkStatement(statementFunctionDeclaration);
    }

    // register proto fields in scope
    vector<pair<string, shared_ptr<ValueType>>> members;

    // extract variable fields
    for (shared_ptr<StatementVariableDeclaration> statementVariableDeclaration : statement->getStatementVariableDeclarations())
        members.push_back(pair(statementVariableDeclaration->getIdentifier(), statementVariableDeclaration->getValueType()));

    // then function fields
    for (shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration : statement->getStatementFunctionDeclarations())
        members.push_back(pair(statementFunctionDeclaration->getName(), statementFunctionDeclaration->getValueType()));

    // check each of the extracted type
    for (auto &member : members) {
        if (typeForCheckedValueType(member.second, true) == nullptr)
            return;
    }

    // and the register
    scope->protoScope->registerFields(statement->getSymbolName(), members);
}

void Analyzer::checkStatement(shared_ptr<StatementProtoDeclaration> statementProtoDeclaration) {
    scope->protoScope->registerDeclaration(statementProtoDeclaration->getSymbolName());
}

void Analyzer::checkStatement(shared_ptr<StatementRawFunction> statementRawFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementRawFunction->getArguments())
        argumentTypes.push_back(argument.second);

    // register raw function definition
    AnalyzerScopeRegisterResult registerResult = scope->functionScope->registerValueType(
        statementRawFunction->getGlobalName(),
        statementRawFunction->getValueType(),
        true
    );
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(
            statementRawFunction->getLocation(),
            statementRawFunction->getGlobalName(),
            registerResult
        );
        return;
    }
}

void Analyzer::checkStatement(shared_ptr<StatementRepeat> statementRepeat, shared_ptr<ValueType> returnType) {
    if (!scope->level([&]() -> bool {
        if (statementRepeat->getInitStatement() != nullptr)
            checkStatement(statementRepeat->getInitStatement(), returnType);

        if (statementRepeat->getPostStatement() != nullptr)
            checkStatement(statementRepeat->getPostStatement(), returnType);

        if (shared_ptr<Expression> preConditionExpression = statementRepeat->getPreConditionExpression()) {
            preConditionExpression->valueType = typeForExpression(preConditionExpression, nullptr, nullptr);
            if (preConditionExpression->getValueType() != nullptr && !preConditionExpression->getValueType()->isEqual(ValueTypeSimple::BOOL)) {
                markErrorInvalidType(preConditionExpression->getLocation(), preConditionExpression->getValueType(), ValueTypeSimple::BOOL);
                return false;
            }
        }

        if (shared_ptr<Expression> postConditionExpression = statementRepeat->getPostConditionExpression()) {
            postConditionExpression->valueType = typeForExpression(postConditionExpression, nullptr, nullptr);
            if (postConditionExpression->getValueType() != nullptr && !postConditionExpression->getValueType()->isEqual(ValueTypeSimple::BOOL)) {
                markErrorInvalidType(postConditionExpression->getLocation(), postConditionExpression->getValueType(), ValueTypeSimple::BOOL);
                return false;
            }
        }

        // body
        checkStatement(statementRepeat->getBodyBlockStatement(), returnType);

        return true;
    })) { return; }
}

void Analyzer::checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType) {
    statementReturn->expression = checkAndTryCasting(
        statementReturn->getExpression(),
        returnType,
        returnType
    );
    if (statementReturn->getExpression() == nullptr)
        return;

    shared_ptr<ValueType> expressionType = statementReturn->getExpression()->getValueType();
    if (expressionType == nullptr || !expressionType->isEqual(returnType)) {
        markErrorInvalidType(
            statementReturn->getExpression()->getLocation(),
            expressionType,
            returnType
        );
    }
}

void Analyzer::checkStatement(shared_ptr<StatementVariable> statementVariable) {
    statementVariable->valueType = typeForCheckedValueType(statementVariable->getValueType(), false);
    if (statementVariable->getValueType() == nullptr)
        return;

    // check initial value expression
    if (statementVariable->getExpression() != nullptr) {
        // cast expression into target
        statementVariable->expression = checkAndTryCasting(
            statementVariable->getExpression(),
            statementVariable->getValueType(),
            nullptr
        );
        if (statementVariable->getExpression() == nullptr)
            return;

        // if target has no count expression defined, use the one from source
        if (statementVariable->getValueType()->isData() && dynamic_pointer_cast<ValueTypeData>(statementVariable->getValueType())->getCountExpression() == nullptr) {
            statementVariable->valueType = make_shared<ValueTypeData>(
                dynamic_pointer_cast<ValueTypeData>(statementVariable->getValueType())->getElementValueType(),
                dynamic_pointer_cast<ValueTypeData>(statementVariable->getExpression()->getValueType())->getCountExpression(),
                statementVariable->getLocation()
            );
        }

        if (!statementVariable->getValueType()->isEqual(statementVariable->getExpression()->getValueType())) {
            markErrorInvalidType(
                statementVariable->getExpression()->getValueType()->getLocation(),
                statementVariable->getExpression()->getValueType(),
                statementVariable->getValueType()
            );
            return;
        }
    }

    // data types should have count expression
    if (statementVariable->getValueType()->isData() && dynamic_pointer_cast<ValueTypeData>(statementVariable->getValueType())->getCountExpression() == nullptr) {
        markErrorInvalidType(statementVariable->getValueType()->getLocation(), statementVariable->getValueType(), nullptr);
        return;
    }

    // try registering in scope*/
    AnalyzerScopeRegisterResult registerResult = scope->variableScope->registerValueType(statementVariable->getGlobalIdentifier(), statementVariable->getValueType(), true);
    if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
        markErrorInvalidRegistration(statementVariable->getLocation(), statementVariable->getIdentifier(), registerResult);
        return;
    }

    // updated corresponding variable declaration
    for (shared_ptr<Statement> headerStatement : this->module->getHeaderStatements()) {
        // find matching declaration
        shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(headerStatement);
        if (statementVariableDeclaration != nullptr && statementVariableDeclaration->getIdentifier().compare(statementVariable->getIdentifier()) == 0) {
            statementVariableDeclaration->valueType = statementVariable->getValueType();
        }
    }
}

void Analyzer::checkStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    statementVariableDeclaration->valueType = typeForCheckedValueType(statementVariableDeclaration->getValueType(), false);
    if (statementVariableDeclaration->getValueType() == nullptr)
        return;

    // data types should have count expression
    if (statementVariableDeclaration->getValueType()->isData() && statementVariableDeclaration->getValueType()->toData()->getCountExpression() == nullptr) {
        markErrorInvalidType(statementVariableDeclaration->getValueType()->getLocation(), statementVariableDeclaration->getValueType(), nullptr);
        return;
    }

    // try registering in scope
    string identifier = statementVariableDeclaration->getGlobalIdentifier();
    // variable declarations are allowed (in blobs)
    if (!identifier.empty()) {
        AnalyzerScopeRegisterResult registerResult = scope->variableScope->registerValueType(identifier, statementVariableDeclaration->getValueType(), false);
        if (registerResult != AnalyzerScopeRegisterResult::SUCCES) {
            markErrorInvalidRegistration(statementVariableDeclaration->getLocation(), identifier, registerResult);
            return;
        }
    }
}

// Expressions

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<Expression> expression, shared_ptr<Expression> parentExpression, shared_ptr<ValueType> returnType) {
    if (expression == nullptr)
        return nullptr;

    if (expression->getValueType() != nullptr && expression->getKind() != ExpressionKind::CAST)
        return expression->getValueType();

    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::BLOCK:
            return typeForExpression(dynamic_pointer_cast<ExpressionBlock>(expression), returnType);
        case ExpressionKind::CALL:
            return typeForExpression(dynamic_pointer_cast<ExpressionCall>(expression), parentExpression);
        case ExpressionKind::CAST:
            return typeForExpression(dynamic_pointer_cast<ExpressionCast>(expression), parentExpression);
        case ExpressionKind::CHAINED:
            return typeForExpression(dynamic_pointer_cast<ExpressionChained>(expression));
        case ExpressionKind::COMPOSITE_LITERAL:
            return typeForExpression(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression));
        case ExpressionKind::GROUPING:
            return typeForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::IF_ELSE:
            return typeForExpression(dynamic_pointer_cast<ExpressionIfElse>(expression), returnType);
        case ExpressionKind::LITERAL:
            return typeForExpression(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::MATCH:
            return typeForExpression(dynamic_pointer_cast<ExpressionMatch>(expression), returnType);
        case ExpressionKind::NONE:
            return ValueTypeSimple::NONE;
        case ExpressionKind::UNARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::VALUE:
            return typeForExpression(dynamic_pointer_cast<ExpressionValue>(expression), parentExpression);
        default:
            break;
    }

    markErrorUnexpected(expression->getLocation(), "expression");
    return nullptr;
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    expressionBinary->getLeft()->valueType = typeForExpression(expressionBinary->getLeft(), nullptr, nullptr);
    expressionBinary->getRight()->valueType = typeForExpression(expressionBinary->getRight(), nullptr, nullptr);

    if (expressionBinary->getLeft()->getValueType() == nullptr || expressionBinary->getRight()->getValueType() == nullptr)
        return nullptr;

    // We only want to adjust arithmetic or comparison operations
    // Logical must already evaluate to bool
    // bitwise we want to leave as they are and leave it to the module builder
    if (expressionBinary->isArithmetic() || expressionBinary->isComparison()) {
        shared_ptr<ValueType> commonType = commonImplicitCastValueType({
            expressionBinary->getLeft()->getValueType(),
            expressionBinary->getRight()->getValueType()
        });

        expressionBinary->left = checkAndTryCasting(
            expressionBinary->getLeft(),
            commonType,
            nullptr
        );

        expressionBinary->right = checkAndTryCasting(
            expressionBinary->getRight(),
            commonType,
            nullptr
        );
    }

    expressionBinary->valueType = typeForBinaryOperation(
        expressionBinary->getOperation(),
        expressionBinary->getLeft()->getValueType(),
        expressionBinary->getRight()->getValueType()
    );
    return expressionBinary->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionBlock> expressionBlock, shared_ptr<ValueType> returnType) {
    checkStatement(expressionBlock->getStatementBlock(), returnType);
    checkStatement(expressionBlock->getResultStatementExpression(), returnType);
    expressionBlock->valueType = expressionBlock->getResultStatementExpression()->getExpression()->getValueType();
    return expressionBlock->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCall> expressionCall, shared_ptr<Expression> parentExpression) {
    shared_ptr<ValueType> valueType;

    int extraArguments = 0;

    if (!scope->level([&]() -> bool {
        // check for built-in
        if (parentExpression != nullptr) {
            bool isParentPointer = parentExpression->getValueType()->isPtr();
            bool isParentBlob = parentExpression->getValueType()->isBlob();
            bool isParentProto = parentExpression->getValueType()->isProto();
            bool isVal = expressionCall->getName().compare("val") == 0;

            if (isParentPointer && isVal && dynamic_pointer_cast<ValueTypePtr>(parentExpression->getValueType())->getPointeeValueType()->isFun()) {
                valueType = dynamic_pointer_cast<ValueTypePtr>(parentExpression->getValueType())->getPointeeValueType();
            } else if (isParentBlob) {
                shared_ptr<ValueTypeBlob> parentBlobValueType = dynamic_pointer_cast<ValueTypeBlob>(parentExpression->getValueType());
                string functionName = format("{}.{}", parentBlobValueType->getSymbolName()->getGlobalName(), expressionCall->getName());
                valueType = scope->functionScope->getValueType(functionName);
                if (valueType == nullptr) {
                    markErrorNotDefined(expressionCall->getLocation(), functionName);
                    return false;
                }
                extraArguments = 1; // for the implicit "it"
                scope->boxedScope->registerNamedValueTypesMap(*parentBlobValueType->getNamedValueTypeKeys(), parentBlobValueType->getNamedValueTypes());
            } else if (isParentProto) {
                auto oMembers = scope->protoScope->getFields(parentExpression->getValueType()->toProto()->getSymbolName());
                if (!oMembers) {
                    markErrorNotDefined(parentExpression->getLocation(), parentExpression->getValueType()->toProto()->getSymbolName()->getGlobalName());
                    return false;
                }
                for (pair<string, shared_ptr<ValueType>> &member : *oMembers) {
                    if (expressionCall->getName().compare(member.first) == 0) {
                        valueType = member.second;
                    }
                }
                extraArguments = 1; // for the implicit "it"
            } else {
                if (isParentPointer) {
                    markErrorInvalidType(expressionCall->getLocation(), parentExpression->getValueType()->toPtr()->getPointeeValueType(), nullptr);
                } else {
                    markErrorInvalidType(expressionCall->getLocation(), parentExpression->getValueType(), nullptr);
                }
                return false;
            }
        } else {
            expressionCall->setModuleName(module->getName());
            valueType = scope->functionScope->getValueType(expressionCall->getGlobalName());
        }

        // check if defined
        if (valueType == nullptr) {
            markErrorNotDefined(expressionCall->getLocation(), expressionCall->getGlobalName());
            return false;
        }

        // check arguments count
        vector<shared_ptr<ValueType>> argumentTypes = dynamic_pointer_cast<ValueTypeFun>(valueType)->getArgumentValueTypes();
        if (argumentTypes.size() != expressionCall->getArgumentExpressions().size() + extraArguments) {
            markErrorInvalidArgumentsCount(
                expressionCall->getLocation(),
                expressionCall->getArgumentExpressions().size(),
                argumentTypes.size() - extraArguments
            );
            return false;
        }
        // check argument types
        // we want to skip the implicit argumnets hence startring from "extraArguments"
        for (int i=extraArguments; i<argumentTypes.size(); i++) {
            shared_ptr<ValueType> targetType = typeForCheckedValueType(argumentTypes.at(i), false);

            // ignore the implicit arguments
            int argumentExpressionIndex = i - extraArguments;

            expressionCall->argumentExpressions[argumentExpressionIndex] = checkAndTryCasting(
                expressionCall->getArgumentExpressions().at(argumentExpressionIndex),
                targetType,
                dynamic_pointer_cast<ValueTypeFun>(valueType)->getReturnValueType()
            );
            if (expressionCall->getArgumentExpressions().at(argumentExpressionIndex) == nullptr)
                return false;

            shared_ptr<ValueType> sourceType = expressionCall->getArgumentExpressions().at(argumentExpressionIndex)->getValueType();
            if (sourceType == nullptr)
                return false;

            if (!sourceType->isEqual(targetType)) {
                markErrorInvalidType(
                    expressionCall->getLocation(),
                    sourceType,
                    targetType
                );
                expressionCall->valueType = nullptr;
                return false;
            }
        }

        expressionCall->valueType = typeForCheckedValueType(valueType->toFun()->getReturnValueType(), false);
        if (expressionCall->getValueType() == nullptr) {
            markErrorInvalidType(valueType->toFun()->getReturnValueType()->getLocation(), valueType->toFun()->getReturnValueType(), nullptr);
            return false;
        }

        return true;
    })) { return nullptr; }

    return expressionCall->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCast> expressionCast, shared_ptr<Expression> parentExpression) {
    // update count expression type
    if (expressionCast->getValueType()->isData() && dynamic_pointer_cast<ValueTypeData>(expressionCast->getValueType())->getCountExpression() != nullptr) {
        dynamic_pointer_cast<ValueTypeData>(expressionCast->getValueType())->getCountExpression()->valueType = typeForExpression(
            dynamic_pointer_cast<ValueTypeData>(expressionCast->getValueType())->getCountExpression(),
            nullptr,
            nullptr
        );
    }

    expressionCast->valueType = typeForCheckedValueType(expressionCast->getValueType(), false);
    if (expressionCast->getValueType() == nullptr)
        return nullptr;

    // if the first expression in a chain is a cast, we may want to do a built-in operation on it
    if (parentExpression == nullptr)
        return expressionCast->getValueType();

    // If the types are identical, we ignore the cast
    if (parentExpression->getValueType()->isEqual(expressionCast->getValueType()))
        return parentExpression->getValueType();

    bool isSourceNumeric = parentExpression->getValueType()->isNumeric();

    bool areNumeric = parentExpression->getValueType()->isNumeric() && expressionCast->getValueType()->isNumeric();
    bool areBool = parentExpression->getValueType()->isBool() && expressionCast->getValueType()->isBool();
    bool areDataNumeric = parentExpression->getValueType()->isDataNumeric() && expressionCast->getValueType()->isDataNumeric();
    bool areDataBool = parentExpression->getValueType()->isDataBool() && expressionCast->getValueType()->isDataBool();
    bool isAddressToPointer = parentExpression->getValueType()->isAddress() && expressionCast->getValueType()->isPtr();

    bool isSourceComposite = parentExpression->getValueType()->isComposite();
    bool isSourceBoxed = parentExpression->getValueType()->isBoxed();
    bool isSourceEnum = parentExpression->getValueType()->isEnum();
    bool isSourceBlob = parentExpression->getValueType()->isBlob();

    bool isTargetBlob = expressionCast->getValueType()->isBlob();
    bool isTargetData = expressionCast->getValueType()->isData();
    bool isTargetEnumField = expressionCast->getValueType()->getKind() == ValueTypeKind::ENUM_FIELD;
    bool isTargetPointer = expressionCast->getValueType()->isPtr();
    bool isTargetProto = expressionCast->getValueType()->isProto();
    bool isTargetNumeric = expressionCast->getValueType()->isNumeric();

    if (areNumeric || areBool || areDataNumeric || areDataBool | isAddressToPointer) {
        // if cast does not have a count expression, use one from the parent expression
        if (expressionCast->getValueType()->isData() && dynamic_pointer_cast<ValueTypeData>(expressionCast->getValueType())->getCountExpression() == nullptr) {
            expressionCast->valueType = make_shared<ValueTypeData>(
                dynamic_pointer_cast<ValueTypeData>(expressionCast->getValueType())->getElementValueType(),
                dynamic_pointer_cast<ValueTypeData>(parentExpression->getValueType())->getCountExpression(),
                expressionCast->getLocation()
            );
        }
        return expressionCast->getValueType();
    // cast composite to complex type
    } else if (isSourceComposite && (isTargetBlob || isTargetData || isTargetEnumField || isTargetPointer || isTargetProto)) {
        if (canImplicitCast(parentExpression->getValueType(), expressionCast->getValueType())) {
            // we don't want to cast the whole composite, just the individual expression
            // so we ignore the result (we don't replace the composite expression itself)
            checkAndTryCasting(parentExpression, expressionCast->getValueType(), nullptr);
            return expressionCast->getValueType();
        }
    // from boxed
    } else if (isSourceBoxed) {
        if (parentExpression->getValueType()->toBoxed()->getBoxedValueType()->isEqual(expressionCast->getValueType()))
            return expressionCast->getValueType();
    // from enum
    } else if (isSourceEnum && isTargetNumeric) {
        return expressionCast->getValueType();
    // from blob to blob
    } else if (isSourceBlob && isTargetBlob) {
        if (parentExpression->getValueType()->toBlob()->getSymbolName()->isEqual(expressionCast->getValueType()->toBlob()->getSymbolName()))
            return expressionCast->getValueType();
    }  else if (isTargetPointer && !isSourceComposite) {
        shared_ptr<ValueTypePtr> ptrValueType =  expressionCast->getValueType()->toPtr();
        if (ptrValueType->getPointeeValueType() == nullptr) {
            ptrValueType->pointeeValueType = parentExpression->getValueType()->clone();
            return ptrValueType;
        } else if (ptrValueType->getPointeeValueType()->isEqual(parentExpression->getValueType())) {
            return ptrValueType;
        }
    }

    markErrorInvalidCast(expressionCast->getLocation(), parentExpression->getValueType(), expressionCast->getValueType());
    return nullptr;
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionChained> expressionChained) {
    shared_ptr<Expression> parentExpression = nullptr;

    if (!scope->level([&]() -> bool {
        for (shared_ptr<Expression> chainExpression : expressionChained->getChainExpressions()) {
            shared_ptr<ValueType> chainType = typeForExpression(chainExpression, parentExpression, nullptr);
            chainExpression->valueType = chainType;
            parentExpression = chainExpression;
            if (chainType == nullptr)
                return false;
            if (shared_ptr<ValueTypeBlob> valueTypeBlob = dynamic_pointer_cast<ValueTypeBlob>(chainType)) {
                scope->boxedScope->registerNamedValueTypesMap(*valueTypeBlob->getNamedValueTypeKeys(), valueTypeBlob->getNamedValueTypes());
            }
        }

        expressionChained->valueType = parentExpression->getValueType();

        return true;
    })) { return nullptr; }

    return expressionChained->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    vector<shared_ptr<ValueType>> elementTypes;
    for (shared_ptr<Expression> expression : expressionCompositeLiteral->getExpressions()) {
        if (expression == nullptr)
            return nullptr;
        shared_ptr<ValueType> elementType = typeForExpression(expression, nullptr, nullptr);
        if (elementType == nullptr)
            return nullptr;
        elementTypes.push_back(elementType);
    }
    shared_ptr<Expression> countExpression = ExpressionLiteral::expressionLiteralForUInt(elementTypes.size(), expressionCompositeLiteral->getLocation());
    countExpression->valueType = typeForExpression(countExpression, nullptr, nullptr);
    expressionCompositeLiteral->valueType = make_shared<ValueTypeComposite>(elementTypes, countExpression, expressionCompositeLiteral->getLocation());
    return expressionCompositeLiteral->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionGrouping> expressionGrouping) {
    expressionGrouping->valueType = typeForExpression(expressionGrouping->getSubExpression(), nullptr, nullptr);
    return expressionGrouping->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionIfElse> expressionIfElse, shared_ptr<ValueType> returnValueType) {
    // first check that condition is as BOOL
    expressionIfElse->conditionExpression = checkAndTryCasting(expressionIfElse->getConditionExpression(), ValueTypeSimple::BOOL, returnValueType);
    if (expressionIfElse->getConditionExpression() == nullptr)
        return nullptr;
    shared_ptr<ValueType> conditionType = expressionIfElse->getConditionExpression()->getValueType();
    if (conditionType == nullptr) {
        return nullptr;
    } else if (!conditionType->isEqual(ValueTypeSimple::BOOL)) {
        markErrorInvalidType(
            conditionType->getLocation(),
            conditionType,
            ValueTypeSimple::BOOL
        );
    }

    vector<shared_ptr<ValueType>> conditionValueTypes;

    // Then expression
    scope->pushLevel();
    expressionIfElse->getThenExpression()->valueType = typeForExpression(expressionIfElse->getThenExpression(), nullptr, returnValueType);
    scope->popLevel();
    if (expressionIfElse->getThenExpression()->getValueType() == nullptr)
        return nullptr;
    conditionValueTypes.push_back(expressionIfElse->getThenExpression()->getValueType());

    // Else expression
    if (expressionIfElse->getElseExpression() != nullptr) {
        scope->pushLevel();
        expressionIfElse->getElseExpression()->valueType = typeForExpression(expressionIfElse->getElseExpression(), nullptr, returnValueType);
        scope->popLevel();
        if (expressionIfElse->getElseExpression()->getValueType() == nullptr)
            return nullptr;
        conditionValueTypes.push_back(expressionIfElse->getElseExpression()->getValueType());
    }

    // Check to what common type can we cast the conditions (only if there is an else expression)
    if (expressionIfElse->getElseExpression() != nullptr) {
        expressionIfElse->valueType = commonImplicitCastValueType(conditionValueTypes);

        expressionIfElse->thenExpression = checkAndTryCasting(
            expressionIfElse->getThenExpression(),
            expressionIfElse->getValueType(),
            returnValueType
        );
        expressionIfElse->elseExpression = checkAndTryCasting(
            expressionIfElse->getElseExpression(),
            expressionIfElse->getValueType(),
            returnValueType
        );
    // Otherwise the whole thing is just none
    } else {
        expressionIfElse->valueType = make_shared<ValueTypeSimple>(ValueTypeKind::NONE, expressionIfElse->getLocation());
    }

    return expressionIfElse->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    // if it's already set, return it
    if (expressionLiteral->getValueType() != nullptr)
        return expressionLiteral->getValueType();

    // otherwise get a default one
    switch (expressionLiteral->getLiteralKind()) {
        case ExpressionLiteralKind::BOOL:
            expressionLiteral->valueType = ValueTypeSimple::BOOL;
            break;
        case ExpressionLiteralKind::UINT:
            expressionLiteral->valueType = ValueTypeSimple::UINT;
            break;
        case ExpressionLiteralKind::FLOAT:
            expressionLiteral->valueType = ValueTypeSimple::FLOAT;
            break;
        default:
            markErrorInvalidType(expressionLiteral->getLocation(), nullptr, nullptr);
            return nullptr;
    }

    return expressionLiteral->getValueType();
}

std::shared_ptr<ValueType> Analyzer::typeForExpression(std::shared_ptr<ExpressionMatch> expressionMatch, shared_ptr<ValueType> returnValueType) {
    expressionMatch->getSwitchExpression()->valueType = typeForExpression(expressionMatch->getSwitchExpression(), nullptr, returnValueType);
    if (expressionMatch->getSwitchExpression()->getValueType() == nullptr)
        return nullptr;

    vector<shared_ptr<ValueType>> caseValueTypes;

    // Evaluate each of the case expressions
    if (!scope->level([&]() -> bool {
        scope->boxedScope->registerNamedValueTypesMap(
            *scope->enumScope->getNamedValueTypeKeys(expressionMatch->getSwitchExpression()->getValueType()->toEnum()->getSymbolName()),
            expressionMatch->getSwitchExpression()->getValueType()->toEnum()->getNamedValueTypes()
        );
        for (pair<shared_ptr<Pattern>, shared_ptr<Expression>> &casePair : expressionMatch->getCasePairs()) {
            casePair.first->valueType = typeForCheckedValueType(casePair.first->getValueType(), false);
            if (casePair.first->getValueType() == nullptr)
                return false;
            checkPattern(casePair.first, expressionMatch->getSwitchExpression()->getValueType());
            if (!scope->level([&]() -> bool {
                scope->variableScope->registerAnonymousValueType(casePair.first->getValueType());
                casePair.second->valueType = typeForExpression(casePair.second, nullptr, returnValueType);
                if (casePair.second->getValueType() == nullptr)
                    return false;

                return true;
            })) { return false; }

            caseValueTypes.push_back(casePair.second->getValueType());
        }

        return true;
    })) { return nullptr; };

    // Else expression
    if (expressionMatch->getElseExpression() != nullptr) {
        expressionMatch->getElseExpression()->valueType = typeForExpression(expressionMatch->getElseExpression(), nullptr, returnValueType);
        caseValueTypes.push_back(expressionMatch->getElseExpression()->getValueType());
    }

    // Check to what commont type can we cast the cases (only if there is an else expression)
    if (expressionMatch->getElseExpression() != nullptr) {
        expressionMatch->valueType = commonImplicitCastValueType(caseValueTypes);

        for (pair<shared_ptr<Pattern>, shared_ptr<Expression>> &casePair : expressionMatch->getCasePairs())
            casePair.second = checkAndTryCasting(casePair.second, expressionMatch->getValueType(), returnValueType);
        expressionMatch->elseExpression = checkAndTryCasting(expressionMatch->getElseExpression(), expressionMatch->getValueType(), returnValueType);
    // Otherwise the whole thing is just none
    } else {
        expressionMatch->valueType = make_shared<ValueTypeSimple>(ValueTypeKind::NONE, expressionMatch->getLocation());
    }

    return expressionMatch->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    ExpressionUnaryOperation operation = expressionUnary->getOperation();
    shared_ptr<ValueType> subType = typeForExpression(expressionUnary->getSubExpression(), nullptr, nullptr);
    if (subType == nullptr)
        return nullptr;

    if (!isUnaryOperationValidForType(expressionUnary->getOperation(), subType)) {
        markErrorInvalidOperationUnary(expressionUnary->getLocation(), operation, subType);
        return nullptr;
    }
    
    expressionUnary->valueType = typeForUnaryOperation(operation, subType);

    return expressionUnary->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionValue> expressionValue, shared_ptr<Expression> parentExpression) {
    if (parentExpression != nullptr) {
        // check built-in
        bool isParentData = parentExpression->getValueType()->isData();
        bool isParentPointer = parentExpression->getValueType()->isPtr();
        bool isParentBlob = parentExpression->getValueType()->isBlob();
        bool isParentProto = parentExpression->getValueType()->isProto();
        bool isParentEnum = parentExpression->getValueType()->isEnum();
        bool isParentEnumField = parentExpression->getValueType()->isEnumField();
        bool isParentCast = parentExpression->getKind() == ExpressionKind::CAST;

        bool isCount = expressionValue->getIdentifier() == "count";
        bool isVal = expressionValue->getIdentifier() == "val";
        bool isVadr = expressionValue->getIdentifier() == "vadr";
        bool isAdr = expressionValue->getIdentifier() == "adr";
        bool isSize = expressionValue->getIdentifier() == "size";
        bool isTag = expressionValue->getIdentifier() == "tag";

        // parent data
        if (isParentData && isCount) {
            expressionValue->valueType = ValueTypeSimple::UINT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_COUNT;
            return expressionValue->getValueType();
        // parent pointer
        } else if (isParentPointer && isVal) {
            switch (expressionValue->getValueKind()) {
                case ExpressionValueKind::SIMPLE:
                case ExpressionValueKind::BUILT_IN_VAL_SIMPLE:
                    expressionValue->valueType = dynamic_pointer_cast<ValueTypePtr>(parentExpression->getValueType())->getPointeeValueType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_SIMPLE;
                    break;
                case ExpressionValueKind::DATA:
                case ExpressionValueKind::BUILT_IN_VAL_DATA:
                    // make sure we're referencing a pointer to data
                    if (!dynamic_pointer_cast<ValueTypePtr>(parentExpression->getValueType())->getPointeeValueType()->isData()) {
                        expressionValue->valueType = nullptr;
                        markErrorInvalidBuiltIn(
                            expressionValue->getLocation(),
                            expressionValue->getIdentifier(),
                            parentExpression->getValueType()
                        );
                        break;
                    }
                    expressionValue->valueType = dynamic_pointer_cast<ValueTypeData>(dynamic_pointer_cast<ValueTypePtr>(parentExpression->getValueType())->getPointeeValueType())->getElementValueType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_DATA;
                    expressionValue->indexExpression = checkAndTryCasting(expressionValue->getIndexExpression(), ValueTypeSimple::UINT, nullptr);
                    break;
                default:
                    expressionValue->valueType = nullptr;
                    markErrorInvalidBuiltIn(
                        expressionValue->getLocation(),
                        expressionValue->getIdentifier(),
                        parentExpression->getValueType()
                    );
                    break;
            }
            return expressionValue->getValueType();
        } else if ((isParentPointer || isParentProto) && isVadr) {
            expressionValue->valueType = ValueTypeSimple::A;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VADR;
            return expressionValue->getValueType();
        // parent enum/enum field
        } else if ((isParentEnum || isParentEnumField) && isTag) {
            expressionValue->valueType = ValueTypeSimple::UINT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_TAG;
            return expressionValue->getValueType();
        // parent enum field
        } else if (isParentEnumField && isVal) {
            expressionValue->valueType = parentExpression->getValueType()->toEnumField()->getPayloadValueType();
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_SIMPLE;
            return expressionValue->getValueType();
        // parent cast
        } else if (!isParentCast && isAdr) {
            expressionValue->valueType = ValueTypeSimple::A;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_ADR;
            return expressionValue->getValueType();
        // size
        } else if (isSize) {
            expressionValue->valueType = ValueTypeSimple::UINT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_SIZE;
            return expressionValue->getValueType();
        // parent blob
        } else if (isParentBlob) {
            shared_ptr<ValueTypeBlob> blobValueType = dynamic_pointer_cast<ValueTypeBlob>(parentExpression->getValueType());
            string blobName = blobValueType->getSymbolName()->getGlobalName();
            optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = scope->blobScope->getFields(blobValueType->getSymbolName());
            if (blobMembers) {
                if (!scope->level([&]() -> bool {
                    scope->boxedScope->registerNamedValueTypesMap(*blobValueType->getNamedValueTypeKeys(), blobValueType->getNamedValueTypes());

                    string nameVariable = expressionValue->getIdentifier();
                    string nameFunction = format("{}.{}", blobName, expressionValue->getIdentifier());
                    for (pair<string, shared_ptr<ValueType>> &blobMember : *blobMembers) {
                        if (nameVariable == blobMember.first || nameFunction == blobMember.first) {
                            // found corresponding blob, decide if it's a simple or data access
                            switch (expressionValue->getValueKind()) {
                                case ExpressionValueKind::SIMPLE: {
                                    // resolve type of named type if required
                                    expressionValue->valueType = typeForCheckedValueType(blobMember.second, false);
                                    return true;
                                }
                                case ExpressionValueKind::DATA: {
                                    // make sure that the indexed value is an array
                                    shared_ptr<ValueType> valueType = blobMember.second;
                                    if (valueType->getKind() != ValueTypeKind::DATA) {
                                        markErrorInvalidType(expressionValue->getLocation(), valueType, nullptr);
                                        return false;
                                    }
                                    expressionValue->valueType = dynamic_pointer_cast<ValueTypeData>(blobMember.second)->getElementValueType();
                                    expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
                                    // make sure that the index expression evaluates to an uint
                                    shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
                                    if (!indexExpression->getValueType()->isUnsignedInteger()) {
                                        markErrorInvalidType(indexExpression->getLocation(), indexExpression->getValueType(), ValueTypeSimple::UINT);
                                        return false;
                                    }

                                    return true;
                                }
                                default:
                                    break;
                            }
                        }
                    }

                    markErrorNotDefined(expressionValue->getLocation(), expressionValue->getIdentifier());
                    return false;
                })) { return nullptr; }

                return expressionValue->getValueType();
            }
            markErrorNotDefined(
                expressionValue->getLocation(),
                format("{}.{}", blobName, expressionValue->getIdentifier())
            );
            return nullptr;
        // parent proto
        } else if (isParentProto) {
            auto members = *scope->protoScope->getFields(parentExpression->getValueType()->toProto()->getSymbolName());
            for (pair<string, shared_ptr<ValueType>> &member : members) {
                if (expressionValue->getIdentifier() == member.first) {
                    // found corresponding member, decide if it's a simple or data access
                    switch (expressionValue->getValueKind()) {
                        case ExpressionValueKind::SIMPLE:
                            expressionValue->valueType = member.second;
                            return expressionValue->getValueType();
                        case ExpressionValueKind::DATA: {
                            // make sure that the indexed value is an array
                            shared_ptr<ValueType> valueType = member.second;
                            if (valueType->getKind() != ValueTypeKind::DATA) {
                                markErrorInvalidType(valueType->getLocation(), valueType, nullptr);
                                return nullptr;
                            }
                            expressionValue->valueType = dynamic_pointer_cast<ValueTypeData>(member.second)->getElementValueType();
                            expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
                            // make sure that the index expression evaluates to an uint
                            shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
                            if (!indexExpression->getValueType()->isUnsignedInteger()) {
                                markErrorInvalidType(indexExpression->getValueType()->getLocation(), indexExpression->getValueType(), ValueTypeSimple::UINT);
                                return nullptr;
                            }
                            return expressionValue->getValueType();
                        }
                        default:
                            break;
                    }
                }
            }
            markErrorNotDefined(
                expressionValue->getLocation(),
                format("{}.{}", parentExpression->getValueType()->toProto()->getSymbolName()->getGlobalName(), expressionValue->getIdentifier())
            );
            return nullptr;
        }
    }

    // first assume it's just simple
    //shared_ptr<ValueType> type = scope->getVariableType(expressionValue->getIdentifier());
    shared_ptr<ValueType> type = scope->variableScope->getValueType(expressionValue->getIdentifier());
    // try with global identifier
    if (type == nullptr && expressionValue->getModuleName().empty()) {
        string globalIdentifier = format("{}.{}", module->getName(), expressionValue->getIdentifier());
        //type = scope->getVariableType(globalIdentifier);
        type = scope->variableScope->getValueType(globalIdentifier);
        if (type != nullptr)
            expressionValue->setModuleName(module->getName());
    }
    if (type != nullptr) {
        expressionValue->valueKind = ExpressionValueKind::SIMPLE;
    }

    // then check if it's data
    if (type != nullptr && expressionValue->getIndexExpression() != nullptr) {
        // make sure that the indexed value is an array
        if (type->getKind() != ValueTypeKind::DATA) {
            markErrorInvalidType(type->getLocation(), type, nullptr);
            return nullptr;
        }
        expressionValue->indexExpression = checkAndTryCasting(
            expressionValue->getIndexExpression(),
            ValueTypeSimple::UINT,
            nullptr
        );
        shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
        // make sure that the index expression evaluates to an uint
        if (!indexExpression->getValueType()->isUnsignedInteger()) {
            markErrorInvalidType(indexExpression->getValueType()->getLocation(), indexExpression->getValueType(), ValueTypeSimple::UINT);
            return nullptr;
        }
        type = dynamic_pointer_cast<ValueTypeData>(type)->getElementValueType();
        expressionValue->valueKind = ExpressionValueKind::DATA;
    // check if it's blob's `it`
    } else if (type == nullptr && expressionValue->getIdentifier() == "it") {
        //shared_ptr<ValueType> blobPtrType = scope->getVariableType(".pit");
        shared_ptr<ValueType> blobPtrType = scope->variableScope->getValueType(".pit");
        if (blobPtrType == nullptr) {
            markErrorNotDefined(expressionValue->getLocation(), expressionValue->getIdentifier());
            return nullptr;
        }
        type = dynamic_pointer_cast<ValueTypePtr>(blobPtrType)->getPointeeValueType();
        expressionValue->valueKind = ExpressionValueKind::SIMPLE;
    // check if it's a function
    } else if (type == nullptr) {
        type = scope->functionScope->getValueType(expressionValue->getIdentifier());
        if (type == nullptr && expressionValue->getModuleName().empty()) {
            string globalIdentifier = format("{}.{}", module->getName(), expressionValue->getIdentifier());
            type = scope->functionScope->getValueType(globalIdentifier);
            if (type != nullptr)
                expressionValue->setModuleName(module->getName());
        }
        if (type != nullptr)
            expressionValue->valueKind = ExpressionValueKind::FUN;
    }
    
    if (type == nullptr && parentExpression != nullptr) {
        markErrorNotDefined(expressionValue->getLocation(), format("member \".{}\"", expressionValue->getIdentifier()));
    } else if (type == nullptr) {
        markErrorNotDefined(expressionValue->getLocation(), format("\"{}\"", expressionValue->getIdentifier()));
    }

    expressionValue->valueType = type;
    return expressionValue->getValueType();
}

//
// Support
//
bool Analyzer::isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) const {
    switch (type->getKind()) {
        // bool
        case ValueTypeKind::BOOL:
            switch (operation) {
                case ExpressionUnaryOperation::NOT: {
                    return true;
                }
                default:
                    break;
            break;
        }
        // numeric
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64: {
            switch (operation) {
                case ExpressionUnaryOperation::BIT_NOT:
                case ExpressionUnaryOperation::PLUS:
                case ExpressionUnaryOperation::MINUS: {
                    return true;
                }
                default:
                    break;
            }
            break;
        }

        // address
        case ValueTypeKind::A: {
            switch (operation) {
                case ExpressionUnaryOperation::BIT_NOT:
                case ExpressionUnaryOperation::PLUS: {
                    return true;
                }
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    // all other combinations are invalid
    return false;
}

bool Analyzer::isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) const{
    // Unbox types if required
    if (firstType->isBoxed())
        firstType = dynamic_pointer_cast<ValueTypeBoxed>(firstType)->getBoxedValueType();

    if (secondType->isBoxed())
        secondType = dynamic_pointer_cast<ValueTypeBoxed>(secondType)->getBoxedValueType();

    switch (firstType->getKind()) {
        // Valid operations for boolean types
        case ValueTypeKind::BOOL: {
            switch (operation) {
                case ExpressionBinaryOperation::EQUAL:
                case ExpressionBinaryOperation::NOT_EQUAL:

                case ExpressionBinaryOperation::OR:
                case ExpressionBinaryOperation::XOR:
                case ExpressionBinaryOperation::AND:
                    return firstType->isEqual(secondType);
                default:
                    break;
            }
            break;
        }
        // Valid operations for numeric types
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:
        
        case ValueTypeKind::A: {
            switch (operation) {
                // shift operations requires second type to be an unsigned integer
                case ExpressionBinaryOperation::BIT_SHL:
                case ExpressionBinaryOperation::BIT_SHR: {
                    switch (secondType->getKind()) {
                        case ValueTypeKind::UINT:
                        case ValueTypeKind::U8:
                        case ValueTypeKind::U16:
                        case ValueTypeKind::U32:
                        case ValueTypeKind::U64: {
                            return true;
                        }
                        default:
                            break;
                    }
                    break;
                }

                // other operations have to match
                case ExpressionBinaryOperation::BIT_TEST:
                case ExpressionBinaryOperation::BIT_OR:
                case ExpressionBinaryOperation::BIT_XOR:
                case ExpressionBinaryOperation::BIT_AND:

                case ExpressionBinaryOperation::EQUAL:
                case ExpressionBinaryOperation::NOT_EQUAL:
                case ExpressionBinaryOperation::LESS:
                case ExpressionBinaryOperation::LESS_EQUAL:
                case ExpressionBinaryOperation::GREATER:
                case ExpressionBinaryOperation::GREATER_EQUAL:

                case ExpressionBinaryOperation::ADD:
                case ExpressionBinaryOperation::SUB:
                case ExpressionBinaryOperation::MUL:
                case ExpressionBinaryOperation::DIV:
                case ExpressionBinaryOperation::MOD: {
                    return firstType->isEqual(secondType);
                }
                default:
                    break;
            }
            break;
        }
        case ValueTypeKind::ENUM: {
            return firstType->isEqual(secondType);
        }
        default:
            break;
    }

    // all other combinations are invalid
    return false;
}

shared_ptr<ValueType> Analyzer::typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) const {
    switch (operation) {
        case ExpressionUnaryOperation::MINUS:
            switch (type->getKind()) {
                case ValueTypeKind::UINT:
                    return ValueTypeSimple::SINT;
                case ValueTypeKind::U8:
                    return ValueTypeSimple::S8;
                case ValueTypeKind::U16:
                    return ValueTypeSimple::S16;
                case ValueTypeKind::U32:
                    return ValueTypeSimple::S32;
                case ValueTypeKind::U64:
                    return ValueTypeSimple::S64;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    // no change
    return type;
}

 shared_ptr<ValueType> Analyzer::typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) const {
    switch (operation) {
        // Comparisions change into BOOL
        case ExpressionBinaryOperation::EQUAL:
        case ExpressionBinaryOperation::NOT_EQUAL:
        case ExpressionBinaryOperation::LESS:
        case ExpressionBinaryOperation::LESS_EQUAL:
        case ExpressionBinaryOperation::GREATER:
        case ExpressionBinaryOperation::GREATER_EQUAL:
        case ExpressionBinaryOperation::BIT_TEST: {
            return ValueTypeSimple::BOOL;
        }

        // We want to promote arithmetic integer operations to full ints
        case ExpressionBinaryOperation::ADD:
        case ExpressionBinaryOperation::SUB:
        case ExpressionBinaryOperation::MUL:
        case ExpressionBinaryOperation::DIV:
        case ExpressionBinaryOperation::MOD:
        case ExpressionBinaryOperation::BIT_OR:
        case ExpressionBinaryOperation::BIT_XOR:
        case ExpressionBinaryOperation::BIT_AND:
        case ExpressionBinaryOperation::BIT_SHL:
        case ExpressionBinaryOperation::BIT_SHR: {
            if (!firstType->isAddress() && !secondType->isAddress() && (firstType->isUnsignedInteger() || secondType->isUnsignedInteger())) {
                return ValueTypeSimple::UINT;
            } else if (firstType->isSignedInteger() || secondType->isSignedInteger()) {
                return ValueTypeSimple::SINT;
            }
            break;
        }
        default:
            break;
    }

    // not change
    return firstType;
}

shared_ptr<Expression> Analyzer::checkAndTryCasting(shared_ptr<Expression> sourceExpression, shared_ptr<ValueType> targetType, shared_ptr<ValueType> returnType) {
    if (sourceExpression == nullptr)
        return nullptr;

    shared_ptr<ValueType> sourceType = typeForExpression(sourceExpression, nullptr, returnType);
    if (sourceType == nullptr)
        return nullptr;
    sourceExpression->valueType = sourceType;
    if (sourceType->isEqual(targetType))
        return sourceExpression;

    if (!canImplicitCast(sourceType, targetType))
        return sourceExpression;

    // single literal just needs to set the type
    if (sourceExpression->getKind() == ExpressionKind::LITERAL && !targetType->isBoxed()) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    // composite to blob
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isBlob()) {
        scope->level([&]() -> bool {
            shared_ptr<ValueTypeBlob> targetValueTypeBlob = targetType->toBlob();
            scope->boxedScope->registerNamedValueTypesMap(*targetValueTypeBlob->getNamedValueTypeKeys(), targetValueTypeBlob->getNamedValueTypes());

            sourceExpression->valueType = targetType;
            vector<shared_ptr<ValueType>> blobMemberTypes = *scope->blobScope->getVariableFieldValueTypes(dynamic_pointer_cast<ValueTypeBlob>(targetType)->getSymbolName());
            shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
            for (int i=0; i<blobMemberTypes.size(); i++) {
                shared_ptr<ValueType> memberType = blobMemberTypes.at(i);
                expressionCompositeLiteral->expressions[i] = checkAndTryCasting(expressionCompositeLiteral->getExpressions().at(i), memberType, returnType);
            }

            return true;
        });

        return sourceExpression;
    // composite to proto
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isProto()) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    // composite to data
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isData()) {
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        // first update the type
        sourceExpression->valueType = make_shared<ValueTypeData>(
            dynamic_pointer_cast<ValueTypeData>(targetType)->getElementValueType(),
            ExpressionLiteral::expressionLiteralForUInt(
                expressionCompositeLiteral->getExpressions().size(),
                sourceExpression->getLocation()
            ),
            sourceExpression->getLocation()
        );
        dynamic_pointer_cast<ValueTypeData>(sourceExpression->getValueType())->getCountExpression()->valueType = typeForExpression(dynamic_pointer_cast<ValueTypeData>(sourceExpression->getValueType())->getCountExpression(), nullptr, returnType);
        // and then cast (if necessary) each of the element expressions
        for (int i=0; i<expressionCompositeLiteral->getExpressions().size(); i++) {
            shared_ptr<Expression> sourceElementExpression = expressionCompositeLiteral->getExpressions().at(i);
            sourceElementExpression = checkAndTryCasting(sourceElementExpression, dynamic_pointer_cast<ValueTypeData>(targetType)->getElementValueType(), returnType);
        }
        // check if types are already equal or we need additional cast
        if (dynamic_pointer_cast<ValueTypeData>(targetType)->getCountExpression() == nullptr || expressionCompositeLiteral->getValueType()->isEqual(targetType))
            return sourceExpression;
    // composite to pointer
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isPtr()) {
        sourceExpression->valueType = targetType;
        // make sure the composite element expression is of type a
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        shared_ptr<Expression> sourceElementExpression = expressionCompositeLiteral->getExpressions().at(0);
        sourceElementExpression = checkAndTryCasting(sourceElementExpression, ValueTypeSimple::A, nullptr);
        return sourceExpression;
    // data to data
    } else if (sourceExpression->getValueType()->isData() && targetType->isData()) {
        if (dynamic_pointer_cast<ValueTypeData>(sourceType)->getCountExpression() != nullptr)
            dynamic_pointer_cast<ValueTypeData>(sourceType)->getCountExpression()->valueType = typeForExpression(dynamic_pointer_cast<ValueTypeData>(sourceType)->getCountExpression(), nullptr, returnType);

        if (dynamic_pointer_cast<ValueTypeData>(targetType)->getCountExpression() == nullptr)
            return sourceExpression;
    } else if (sourceExpression->getKind() == ExpressionKind::IF_ELSE) {
        sourceExpression->valueType = targetType;
        shared_ptr<ExpressionIfElse> expressionIfElse = dynamic_pointer_cast<ExpressionIfElse>(sourceExpression);
        expressionIfElse->thenExpression = checkAndTryCasting(expressionIfElse->getThenExpression(), targetType, returnType);
        expressionIfElse->elseExpression = checkAndTryCasting(expressionIfElse->getElseExpression(), targetType, returnType);
        return sourceExpression;
    } else if (sourceExpression->getKind() == ExpressionKind::BLOCK) {
        sourceExpression->valueType = targetType;
        shared_ptr<ExpressionBlock> expressionBlock = dynamic_pointer_cast<ExpressionBlock>(sourceExpression);
        expressionBlock->getResultStatementExpression()->expression = checkAndTryCasting(expressionBlock->getResultStatementExpression()->getExpression(), targetType, returnType);
        return expressionBlock;
    }

    // if target has no count expression defined, use the one from source
    if (targetType->isData() && dynamic_pointer_cast<ValueTypeData>(targetType)->getCountExpression() == nullptr) {
        targetType = make_shared<ValueTypeData>(
            dynamic_pointer_cast<ValueTypeData>(targetType)->getElementValueType(),
            dynamic_pointer_cast<ValueTypeData>(sourceExpression->getValueType())->getCountExpression(),
            sourceExpression->getLocation()
        );
    }

    // create target cast
    shared_ptr<ExpressionChained> targetExpression;

    if (targetType->isBoxed()) {
        targetType = typeForCheckedValueType(targetType, false);
        if (targetType == nullptr)
            return nullptr;

        shared_ptr<ValueTypeBoxed> valueTypeBoxed = dynamic_pointer_cast<ValueTypeBoxed>(targetType);

        sourceExpression = checkAndTryCasting(sourceExpression, valueTypeBoxed->getBoxedValueType(), returnType);

        targetExpression = make_shared<ExpressionChained>(
            vector<shared_ptr<Expression>>(
                {
                    sourceExpression,
                    make_shared<ExpressionCast>(valueTypeBoxed, false, sourceExpression->getLocation())
                }
            ),
            sourceExpression->getLocation()
        );

        targetExpression->valueType = valueTypeBoxed;
    } else {
        targetExpression = make_shared<ExpressionChained>(
            vector<shared_ptr<Expression>>(
                {
                    sourceExpression,
                    make_shared<ExpressionCast>(targetType, false, sourceExpression->getLocation())
                }
            ),
            sourceExpression->getLocation()
        );

        targetExpression->valueType = targetType;
    }
    return targetExpression;
}

bool Analyzer::canImplicitCast(shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    targetType = typeForCheckedValueType(targetType, false);
    if (targetType == nullptr)
        return false;

    switch (sourceType->getKind()) {
        // From UINT
        case ValueTypeKind::UINT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;   
            }
            break;
        }

        // From SINT
        case ValueTypeKind::SINT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;   
            }
            break;
        }

        // From FLOAT
        case ValueTypeKind::FLOAT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;   
            }
            break;
        }

        // From U8
        case ValueTypeKind::U8: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From U16
        case ValueTypeKind::U16: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From U32
        case ValueTypeKind::U32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;
                
                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From U64
        case ValueTypeKind::U64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From S8
        case ValueTypeKind::S8: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From S16
        case ValueTypeKind::S16: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From S32
        case ValueTypeKind::S32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From S64
        case ValueTypeKind::S64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From F32
        case ValueTypeKind::F32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From F64
        case ValueTypeKind::F64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From A
        case ValueTypeKind::A: {
            switch (targetType->getKind()) {
                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From PTR
        case ValueTypeKind::PTR: {
            switch (targetType->getKind()) {
                case ValueTypeKind::PTR:
                    return sourceType->isEqual(targetType);

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, targetType->toBoxed()->getBoxedValueType());

                default:
                    return false;
            }
            break;
        }

        // From DATA
        case ValueTypeKind::DATA: {
            switch (targetType->getKind()) {
                case ValueTypeKind::DATA:
                    return canImplicitCast(sourceType->toData()->getElementValueType(), targetType->toData()->getElementValueType());

                default:
                    return false;
            }
            break;
        }

        // From BLOB
        case ValueTypeKind::BLOB: {
            return sourceType->isEqual(targetType);
        }

        // From COMPOSITE
        case ValueTypeKind::COMPOSITE: {
            switch (targetType->getKind()) {
                // From COMPOSITE to PTR
                case ValueTypeKind::PTR: {
                    vector<shared_ptr<ValueType>> sourceElementValueTypes = sourceType->toComposite()->getElementValueTypes();
                    return sourceElementValueTypes.size() == 1 && sourceElementValueTypes.at(0)->isInteger();
                }

                // From COMPOSITE to DATA
                case ValueTypeKind::DATA: {
                    vector<shared_ptr<ValueType>> sourceElementValueTypes = sourceType->toComposite()->getElementValueTypes();
                    for (shared_ptr<ValueType> sourceElementValueType : sourceElementValueTypes) {
                        if (!canImplicitCast(sourceElementValueType, targetType->toData()->getElementValueType()))
                            return false;
                    }
                    return true;
                }

                // From COMPOSITE to BLOB
                case ValueTypeKind::BLOB: {
                    vector<shared_ptr<ValueType>> sourceElementValueTypes = sourceType->toComposite()->getElementValueTypes();
                    shared_ptr<ValueTypeBlob> targetValueTypeBlob = targetType->toBlob();

                    // get target non-function field types
                    optional<vector<shared_ptr<ValueType>>> oTargetFieldValueTypes = scope->blobScope->getVariableFieldValueTypes(targetValueTypeBlob->getSymbolName());
                    if (!oTargetFieldValueTypes)
                        return false;

                    // check that number of fields match
                    if (sourceElementValueTypes.size() != (*oTargetFieldValueTypes).size())
                        return false;

                    // check that each entry in composite can be cast to member in blob
                    if (!scope->level([&]() -> bool {
                        scope->boxedScope->registerNamedValueTypesMap(*targetValueTypeBlob->getNamedValueTypeKeys(), targetValueTypeBlob->getNamedValueTypes());
                        for (int i=0; i<(*oTargetFieldValueTypes).size(); i++) {
                            if (!canImplicitCast(sourceElementValueTypes.at(i), (*oTargetFieldValueTypes).at(i)))
                                return false;
                        }
                        return true;
                    })) { return false; }

                    return true;
                }

                // From COMPOSITE to ENUM_FIELD
                case ValueTypeKind::ENUM_FIELD: {
                    vector<shared_ptr<ValueType>> sourceElementValueTypes = sourceType->toComposite()->getElementValueTypes();
                    shared_ptr<ValueTypeEnumField> targetValueTypeEnumField = targetType->toEnumField();

                    // first option, no source types and target is none
                    if (sourceElementValueTypes.size() == 0) {
                        return targetValueTypeEnumField->getPayloadValueType()->getKind() == ValueTypeKind::NONE;
                    }

                    // otherwise source has to have one element
                    if (sourceElementValueTypes.size() != 1)
                        return false;

                    // and check if it can be cast
                    return canImplicitCast(sourceElementValueTypes.front(), targetValueTypeEnumField->getPayloadValueType());
                }

                // From COMPOSITE to PROTO
                case ValueTypeKind::PROTO: {
                    vector<shared_ptr<ValueType>> sourceElementValueTypes = sourceType->toComposite()->getElementValueTypes();
                    shared_ptr<SymbolName> targetProtoSymbolName = targetType->toProto()->getSymbolName();

                    // There should be only a single pointer to blob element
                    if (sourceElementValueTypes.size() != 1 || !sourceElementValueTypes.front()->isPtr())
                        return false;

                    shared_ptr<ValueType> pointeeValueType = sourceElementValueTypes.front()->toPtr()->getPointeeValueType();
                    if (pointeeValueType == nullptr || !pointeeValueType->isBlob())
                        return false;

                    // Check if the blob type conforms to the target proto
                    optional<vector<shared_ptr<SymbolName>>> oConformingProtoSymbolNames = scope->blobScope->getConformingProtoSymbolNames(pointeeValueType->toBlob()->getSymbolName());
                    if (!oConformingProtoSymbolNames)
                        return false;
                    
                    for (shared_ptr<SymbolName> conformingProtoSymbolName : *oConformingProtoSymbolNames) {
                        if(targetProtoSymbolName->isEqual(conformingProtoSymbolName))
                            return true;
                    }

                    return false;
                }

                default:
                    return false;
            }
            break;
        }

        // From BOXED
        case ValueTypeKind::BOXED: {
            switch (targetType->getKind()) {
                // TODO: Having automatic unboxing cast would be nice
                /*
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                case ValueTypeKind::PTR:
                    return canCast(sourceType->getSubType(), targetType);
                */

                default:
                    return false;
            }
        }

        // From ENUM_FIELD
        case ValueTypeKind::ENUM_FIELD: {
            switch (targetType->getKind()) {
                // From ENUM_FIELD to ENUM_FIELD
                case ValueTypeKind::ENUM_FIELD:
                    return sourceType->isEqual(targetType);

                // From ENUM_FIELD to ENUM
                case ValueTypeKind::ENUM: {                    
                    shared_ptr<ValueTypeEnumField> sourceValueTypeEnumField = sourceType->toEnumField();
                    shared_ptr<ValueTypeEnum> targetValueTypeEnum = targetType->toEnum();

                    // Is this a field of a parent enum?
                    if (!sourceValueTypeEnumField->getSymbolName()->getGlobalName().starts_with(targetValueTypeEnum->getSymbolName()->getGlobalName()))
                        return false;

                    // Does number of named types match?
                    if (sourceValueTypeEnumField->getNamedValueTypes().size() != targetValueTypeEnum->getNamedValueTypes().size())
                        return false;

                    // Are the named value types identical?
                    for (int i=0; i<sourceValueTypeEnumField->getNamedValueTypes().size(); i++) {
                        shared_ptr<ValueType> sourceNamedValueType = sourceValueTypeEnumField->getNamedValueTypes().at(i);
                        shared_ptr<ValueType> targetNamedValueType = targetValueTypeEnum->getNamedValueTypes().at(i);
                        if (!sourceNamedValueType->isEqual(targetNamedValueType))
                            return false;
                    }

                    return true;
                }

                default:
                    return false;
            }
        }

        // From any other type
        default:
            return false;
    }
}

std::shared_ptr<ValueType> Analyzer::commonImplicitCastValueType(std::vector<std::shared_ptr<ValueType>> valueTypes) {
    if (valueTypes.empty())
        return ValueTypeSimple::NONE;

    shared_ptr<ValueType> targetValueType = valueTypes.at(0);
    for (int i=1; i<valueTypes.size(); i++) {
        // are they the same?
        if (targetValueType->isEqual(valueTypes.at(i))) {
            // don't do anything for identical types
        // can we cast one way?
        } else if (canImplicitCast(valueTypes.at(i), targetValueType)) {
            // keep the current target
        // can we cast the other way?
        } else if (canImplicitCast(targetValueType, valueTypes.at(i))) {
            // switch to a new target type
            targetValueType = valueTypes.at(i);
        // panic
        } else {
            return ValueTypeSimple::NONE;
        }
    }

    // We want to promote integers to their native sizes
    if (!targetValueType->isPtr() && targetValueType->isUnsignedInteger()) {
        targetValueType = ValueTypeSimple::UINT;
    } else if (targetValueType->isSignedInteger()) {
        targetValueType = ValueTypeSimple::SINT;
    }

    return targetValueType;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueType> valueType, bool isCountExperssionRequired) {
    if (valueType == nullptr)
        return nullptr;

    switch (valueType->getKind()) {
        case ValueTypeKind::BLOB:
           return typeForCheckedValueType(valueType->toBlob());
        case ValueTypeKind::BOXED:
            return typeForCheckedValueType(valueType->toBoxed());
        case ValueTypeKind::DATA:
            return typeForCheckedValueType(valueType->toData(), isCountExperssionRequired);
        case ValueTypeKind::ENUM:
            return typeForCheckedValueType(valueType->toEnum());
        case ValueTypeKind::ENUM_FIELD:
            return typeForCheckedValueType(valueType->toEnumField());
        case ValueTypeKind::FUN:
            return typeForCheckedValueType(valueType->toFun());
        case ValueTypeKind::PTR:
            return typeForCheckedValueType(valueType->toPtr());
        default:
            break;
    }

    return valueType;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeBlob> valueTypeBlob) {
    // TODO: This gets messed up because of imported sub-sub-modules
    /*
    if (!scope->isBlobDeclared(*valueType->getBlobName())) {
        markErrorInvalidType(location, valueType, nullptr);
        return false;
    } else
    */

    // Check if blob is registered
    if (scope->blobScope->getState(valueTypeBlob->getSymbolName()) == AnalyzerScopeState::NOT_REGISTERED) {
        markErrorNotDefined(valueTypeBlob->getLocation(), valueTypeBlob->getSymbolName()->getGlobalName());
        return nullptr;
    }

    // Leave the original alone
    shared_ptr<ValueTypeBlob> clonedValueTypeBlob = valueTypeBlob->clone()->toBlob();

    // Make sure module names are set properly
    clonedValueTypeBlob->setModuleName(module->getName());

    optional<vector<string>> oNamedValueTypeKeys = scope->blobScope->getNamedValueTypeKeys(valueTypeBlob->getSymbolName());
    if (oNamedValueTypeKeys)
        clonedValueTypeBlob->namedValueTypeKeys = *oNamedValueTypeKeys;

    return clonedValueTypeBlob;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeBoxed> valueTypeBoxed) {
    // Skip if already resolved
    if (valueTypeBoxed->getBoxedValueType() != nullptr)
        return valueTypeBoxed;
    
    // Otherwise try getting value type from the provided scope
    if (!valueTypeBoxed->getNamedValueTypeKey()) {
        markErrorInvalidType(valueTypeBoxed->getLocation(), valueTypeBoxed, nullptr);
        return nullptr;
    }
    
    // Try resolving named value type key (ignore failures, since it may be a blob field)
    shared_ptr<ValueType> valueType = scope->boxedScope->getNamedValueType(*valueTypeBoxed->getNamedValueTypeKey());
    shared_ptr<ValueTypeBoxed> clonedValueTypeBoxed = dynamic_pointer_cast<ValueTypeBoxed>(valueTypeBoxed->clone());
    if (valueType != nullptr) {
        clonedValueTypeBoxed->boxedValueType = valueType;
    }

    return clonedValueTypeBoxed;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeData> valueTypeData, bool isCountExperssionRequired) {
    if (valueTypeData->getCountExpression() != nullptr) {
        valueTypeData->getCountExpression()->valueType = typeForExpression(valueTypeData->getCountExpression(), nullptr, nullptr);
        return valueTypeData;
    } else if (isCountExperssionRequired) {
        markErrorInvalidType(valueTypeData->getLocation(), valueTypeData, nullptr);
        return nullptr;
    }
    return valueTypeData;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeEnum> valueTypeEnum) {
    optional<vector<string>> oNamedValueTypeKeys = scope->enumScope->getNamedValueTypeKeys(valueTypeEnum->getSymbolName());

    // Check if enum is registered
    if (!oNamedValueTypeKeys) {
        markErrorNotDefined(valueTypeEnum->getLocation(), valueTypeEnum->getSymbolName()->getGlobalName());
        return nullptr;
    }

    // Check number of named types match
    if ((*oNamedValueTypeKeys).size() != valueTypeEnum->getNamedValueTypes().size()) {
        markErrorInvalidType(valueTypeEnum->getLocation(), valueTypeEnum, nullptr);
        return nullptr;
    }

    valueTypeEnum->namedValueTypeKeys = *oNamedValueTypeKeys;

    return valueTypeEnum;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeEnumField> valueTypeEnumField) {
    optional<vector<string>> oNamedValueTypeKeys = scope->enumScope->getNamedValueTypeKeys(valueTypeEnumField->getSymbolName());
    shared_ptr<ValueType> payloadValueType = scope->enumScope->getPayloadValueType(valueTypeEnumField->getSymbolName());

    // Check if enum field is registered
    if (!oNamedValueTypeKeys || payloadValueType == nullptr) {
        markErrorNotDefined(valueTypeEnumField->getLocation(), valueTypeEnumField->getSymbolName()->getGlobalName());
        return nullptr;
    }

    // Check number of named types match
    if ((*oNamedValueTypeKeys).size() != valueTypeEnumField->getNamedValueTypes().size()) {
        markErrorInvalidType(valueTypeEnumField->getLocation(), valueTypeEnumField, nullptr);
        return nullptr;
    }

    // Check payload type, first make sure that potential named types in boxed have access to the current context
    scope->level([&]() -> bool {
        scope->boxedScope->registerNamedValueTypesMap(*oNamedValueTypeKeys, valueTypeEnumField->getNamedValueTypes());
        payloadValueType = typeForCheckedValueType(payloadValueType, false);
        return true;
    });

    if (payloadValueType == nullptr) {
        markErrorInvalidType(valueTypeEnumField->getLocation(), valueTypeEnumField, nullptr);
        return nullptr;
    }

    valueTypeEnumField->namedValueTypeKeys = *oNamedValueTypeKeys;
    valueTypeEnumField->payloadValueType = payloadValueType;

    return valueTypeEnumField;
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypeFun> valueTypeFun) {
    vector<shared_ptr<ValueType>> checkedArgumentValueTypes;
    for (shared_ptr<ValueType> argumentValueType : valueTypeFun->getArgumentValueTypes()) {
        shared_ptr<ValueType> checkedArgumentValueType = typeForCheckedValueType(argumentValueType, true);
        if (checkedArgumentValueType == nullptr)
        return nullptr;
        checkedArgumentValueTypes.push_back(checkedArgumentValueType);
    }
    
    shared_ptr<ValueType> checkedReturnValueType = typeForCheckedValueType(valueTypeFun->getReturnValueType(), true);
    if (checkedReturnValueType == nullptr)
        return nullptr;

    return make_shared<ValueTypeFun>(checkedArgumentValueTypes, checkedReturnValueType, valueTypeFun->getLocation());
}

shared_ptr<ValueType> Analyzer::typeForCheckedValueType(shared_ptr<ValueTypePtr> valueTypePtr) {
    shared_ptr<ValueType> pointeeValueType = typeForCheckedValueType(valueTypePtr->getPointeeValueType(), false);
    return make_shared<ValueTypePtr>(pointeeValueType, valueTypePtr->getIsVolatile(), valueTypePtr->getLocation());
}

// Pattern

void Analyzer::checkPattern(std::shared_ptr<Pattern> pattern, std::shared_ptr<ValueType> switchValueType) {
    pattern->getValueType()->setModuleName(module->getName());
    shared_ptr<ValueTypeEnum> switchEnumValueType = switchValueType->toEnum();
    shared_ptr<ValueTypeEnumField> patternEnumFieldValueType =  pattern->getValueType()->toEnumField();
    if (switchEnumValueType == nullptr || patternEnumFieldValueType == nullptr || !canImplicitCast(patternEnumFieldValueType, switchValueType)) {
        markErrorInvalidPattern(pattern->getLocation(), pattern);
        return;
    }
}

void Analyzer::markErrorInvalidAttribute(shared_ptr<Location> location, const string &name) {
    string message = format("Invalid attribute {}", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidArgumentsCount(shared_ptr<Location> location, int actualCount, int expectedCount) {
    string message = format("Invalid arguments count {}, expected {}", actualCount, expectedCount);
    errors.push_back(Error::error(location, message));
}

 void Analyzer::markErrorInvalidBuiltIn(shared_ptr<Location> location, const string &builtInName, shared_ptr<ValueType> type) {
    string message = format("Invalid built-in \"{}\" on type {}", builtInName, Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidCast(shared_ptr<Location> location, shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    string message = format("Invalid cast from {} to {}", Logger::toString(sourceType), Logger::toString(targetType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidImport(shared_ptr<Location> location, const string &moduleName) {
    string message = format("Invalid import, module \"{}\" doesn't exist", moduleName);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidName(shared_ptr<Location> location, const string &name) {
    string message;
    if (name.empty())
        message = format("Empty name is not allowed");
    else
        message = format("Name `{}` is not allowed", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidOperationBinary(shared_ptr<Location> location, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    string message = format("Invalid binary operation {} for types {} and {}", Logger::toString(operation), Logger::toString(firstType), Logger::toString(secondType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidOperationUnary(shared_ptr<Location> location, ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    string message = format("Invalid unary operation {} for type {}", Logger::toString(operation), Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidPattern(std::shared_ptr<Location> location, std::shared_ptr<Pattern> pattern) {
    string message = format("Invalid pattern {}", Logger::toString(pattern));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidRegistration(std::shared_ptr<Location> location, const std::string &identifier, AnalyzerScopeRegisterResult registerResult) {
    string message = "{INVALID STATE}";

    if (registerResult == AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED) {
        message = format("\"{}\" is already defined", identifier);
    } else if (registerResult == AnalyzerScopeRegisterResult::FAILURE_ALREADY_DECLARED) {
        message = format("Declaration of \"{}\" doesn't match existing type", identifier);
    }

    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidType(shared_ptr<Location> location, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType) {
    string message;
    if (expectedType != nullptr)
        message = format("Invalid type {}, expected {}", Logger::toString(actualType), Logger::toString(expectedType));
    else
        message = format("Invalid type {}", Logger::toString(actualType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNameNotAllowed(std::shared_ptr<Location> location, const std::string &name) {
    string message = format("Name \"{}\" is not allowed in this context", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNotDefined(shared_ptr<Location> location, const string &name) {
    string message = format("{} is not defined in scope", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNotImplemented(shared_ptr<Location> location, const string &protoName, const string &memberName) {
    string message = format("member `{}` of proto `{}` not implemented", memberName, protoName);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorUnexpected(shared_ptr<Location> location, const string &name) {
    string message = format("Unexpected {}", name);
    errors.push_back(Error::error(location, message));
}

