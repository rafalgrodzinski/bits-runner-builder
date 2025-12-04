#include "Parser.h"

#include "Error.h"
#include "Logger.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionValue.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCast.h"

#include "Parser/Statement/StatementModule.h"
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
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementRepeat.h"

#include "Parsee/Parsee.h"
#include "Parsee/ParseeResult.h"
#include "Parsee/ParseeResultsGroup.h"

Parser::Parser(string defaultModuleName, vector<shared_ptr<Token>> tokens) :
defaultModuleName(defaultModuleName), tokens(tokens) { }

shared_ptr<StatementModule> Parser::getStatementModule() {
    shared_ptr<Statement> statement = matchStatementModule();
    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1); 
    }
    return dynamic_pointer_cast<StatementModule>(statement);
}

//
// Statements
//
shared_ptr<Statement> Parser::nextStatement() {
    shared_ptr<Statement> statement;
    int errorsCount = errors.size();

    statement = matchStatementImport();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementFunction();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementRawFunction();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementVariable();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementMetaExternFunction();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementMetaExternVariable();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementBlob();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    if (tryMatchingTokenKinds({TokenKind::END}, true, false))
        return nullptr;

    markError({}, {}, {});

    return nullptr;
}

shared_ptr<Statement> Parser::nextInBlockStatement() {
    shared_ptr<Statement> statement;
    int errorsCount = errors.size();

    statement = matchStatementVariable();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementAssignment();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementReturn();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;
    
    statement = matchStatementRepeat();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    statement = matchStatementExpression();
    if (statement != nullptr || errors.size() > errorsCount)
        return statement;

    markError({}, {}, {});
    return nullptr;
}

shared_ptr<Statement> Parser::matchStatementModule() {
    enum {
        TAG_NAME,
        TAG_STATEMENT
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::M_MODULE, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_NAME),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::OPTIONAL, true
            ),
            Parsee::repeatedGroupParsee(
                {
                    Parsee::statementParsee(ParseeLevel::REQUIRED, true, TAG_STATEMENT),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::OPTIONAL, true
            ),
            Parsee::tokenParsee(TokenKind::END, ParseeLevel::CRITICAL, false)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string moduleName = defaultModuleName;
    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;
    vector<shared_ptr<Statement>> exportedHeaderStatements;

    vector<shared_ptr<Statement>> blobDeclarationStatements;
    vector<shared_ptr<Statement>> blobStatements;
    vector<shared_ptr<Statement>> variableDeclarationStatements;
    vector<shared_ptr<Statement>> functionDeclarationStatements;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_NAME:
                moduleName = parseeResult.getToken()->getLexme();
                break;
            case TAG_STATEMENT: {
                shared_ptr<Statement> statement = parseeResult.getStatement();

                switch (statement->getKind()) {
                    case StatementKind::FUNCTION: { // generate function declaration
                        shared_ptr<StatementFunction> statementFunction = dynamic_pointer_cast<StatementFunction>(statement);
                        shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                            statementFunction->getShouldExport(),
                            statementFunction->getName(),
                            statementFunction->getArguments(),
                            statementFunction->getReturnValueType(),
                            statement->getLine(),
                            statement->getColumn()
                        );
                        // body
                        statements.push_back(statement);
                        // local header
                        functionDeclarationStatements.push_back(statementFunctionDeclaration);
                        // exported header
                        if (statementFunction->getShouldExport()) {
                            // update argument types for exported statement
                            vector<pair<string, shared_ptr<ValueType>>> exportedArguments;
                            for (pair<string, shared_ptr<ValueType>> argument : statementFunctionDeclaration->getArguments())
                                exportedArguments.push_back(pair(argument.first, typeForExportedStatementFromType(argument.second, moduleName)));

                            // updated return type for exported statement
                            shared_ptr<ValueType> exportedReturnValueType = typeForExportedStatementFromType(statementFunctionDeclaration->getReturnValueType(), moduleName);

                            shared_ptr<StatementFunctionDeclaration> exportedStatementFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                                statementFunctionDeclaration->getShouldExport(),
                                statementFunctionDeclaration->getName(),
                                exportedArguments,
                                exportedReturnValueType,
                                statementFunctionDeclaration->getLine(),
                                statementFunctionDeclaration->getColumn()
                            );

                            // append updated statement
                            exportedHeaderStatements.push_back(exportedStatementFunctionDeclaration);
                        }
                        break;
                    }
                    case StatementKind::BLOB: { //generate blob declaration
                        shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                        shared_ptr<StatementBlobDeclaration> statementBlobDeclaration = make_shared<StatementBlobDeclaration>(
                            statementBlob->getShouldExport(),
                            statementBlob->getName(),
                            statement->getLine(),
                            statement->getColumn()
                        );                        
                        // local header
                        blobDeclarationStatements.push_back(statementBlobDeclaration);
                        blobStatements.push_back(statementBlob);
                        // exported header
                        if (statementBlob->getShouldExport()) {
                            // update member types for exported statement
                            vector<pair<string, shared_ptr<ValueType>>> exportedMembers;
                            for (pair<string, shared_ptr<ValueType>> member : statementBlob->getMembers())
                                exportedMembers.push_back(pair(member.first, typeForExportedStatementFromType(member.second, moduleName)));

                            shared_ptr<StatementBlob> exportedStatementBlob = make_shared<StatementBlob>(
                                statementBlob->getShouldExport(),
                                statementBlob->getName(),
                                exportedMembers,
                                statementBlob->getLine(),
                                statementBlob->getColumn()
                            );

                            // declaration doesn't contain any types, so it's fine like this
                            exportedHeaderStatements.push_back(statementBlobDeclaration);
                            // append updated statement
                            exportedHeaderStatements.push_back(exportedStatementBlob);
                        }
                        break;
                    }
                    case StatementKind::VARIABLE: {
                        shared_ptr<StatementVariable> statementVariable = dynamic_pointer_cast<StatementVariable>(statement);
                        shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = make_shared<StatementVariableDeclaration>(
                            statementVariable->getShouldExport(),
                            statementVariable->getIdentifier(),
                            statementVariable->getValueType(),
                            statement->getLine(),
                            statement->getColumn()
                        );
                        // body
                        statements.push_back(statementVariable);
                        // local header
                        variableDeclarationStatements.push_back(statementVariableDeclaration);
                        // exported header
                        if (statementVariable->getShouldExport()) {
                            // new declaration with updated type
                            shared_ptr<StatementVariableDeclaration> exportedStatementVariableDeclaration = make_shared<StatementVariableDeclaration>(
                                statementVariableDeclaration->getShouldExport(),
                                statementVariableDeclaration->getIdentifier(),
                                typeForExportedStatementFromType(statementVariableDeclaration->getValueType(), moduleName),
                                statementVariableDeclaration->getLine(),
                                statementVariableDeclaration->getColumn()
                            );
                            exportedHeaderStatements.push_back(statementVariableDeclaration);
                        }
                        break;
                    }
                    default:
                        statements.push_back(statement);
                        break;
                }
                break;
            }
        }
    }

    // arrange local header
    for (shared_ptr<Statement> &statement : blobDeclarationStatements)
        headerStatements.push_back(statement);

    for (shared_ptr<Statement> &statement : blobStatements)
        headerStatements.push_back(statement);

    for (shared_ptr<Statement> &statement : variableDeclarationStatements)
        headerStatements.push_back(statement);

    for (shared_ptr<Statement> &statement : functionDeclarationStatements)
        headerStatements.push_back(statement);

    return make_shared<StatementModule>(moduleName, statements, headerStatements, exportedHeaderStatements, line, column);
}

shared_ptr<Statement> Parser::matchStatementImport() {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::tokenParsee(TokenKind::M_IMPORT, ParseeLevel::REQUIRED, false),
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string name = resultsGroup.getResults().at(0).getToken()->getLexme();

    return make_shared<StatementMetaImport>(name, line, column);
}

shared_ptr<Statement> Parser::matchStatementMetaExternVariable() {
    enum {
        TAG_IDENTIFIER,
        TAG_VALUE_TYPE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // @extern
            Parsee::tokenParsee(TokenKind::M_EXTERN, ParseeLevel::REQUIRED, false),
            // identifier - module prefix
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER),
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER)
                }, ParseeLevel::OPTIONAL, true
            ),
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_IDENTIFIER),
            Parsee::valueTypeParsee(ParseeLevel::REQUIRED, true, TAG_VALUE_TYPE)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string identifier;
    shared_ptr<ValueType> valueType;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER: {
                identifier += parseeResult.getToken()->getLexme();
                break;
            }
            case TAG_VALUE_TYPE: {
                valueType = parseeResult.getValueType();
                break;
            }
        }
    }

    return make_shared<StatementMetaExternVariable>(identifier, valueType, line, column);
}

shared_ptr<Statement> Parser::matchStatementMetaExternFunction() {
    enum {
        TAG_NAME,
        TAG_ARGUMENT_IDENTIFIER,
        TAG_ARGUMENT_TYPE,
        TAG_RETURN_TYPE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // @extern
            Parsee::tokenParsee(TokenKind::M_EXTERN, ParseeLevel::REQUIRED, false),
            // identifier - module prefix
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_NAME),
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_NAME)
                }, ParseeLevel::OPTIONAL, true
            ),
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_NAME),
            Parsee::tokenParsee(TokenKind::FUNCTION, ParseeLevel::REQUIRED, false),
            // arguments
            Parsee::groupParsee(
                {
                    // first argument
                    Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE),
                    // additional arguments
                    Parsee::repeatedGroupParsee(
                        {
                            Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                            Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE)
                        }, ParseeLevel::OPTIONAL, true
                    )
                }, ParseeLevel::OPTIONAL, true
            ),
            // return type
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::RIGHT_ARROW, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_RETURN_TYPE)
                }, ParseeLevel::OPTIONAL, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_NAME:
                identifier += parseeResult.getToken()->getLexme();
                break;
            case TAG_ARGUMENT_IDENTIFIER: {
                pair<string, shared_ptr<ValueType>> argument;
                argument.first = parseeResult.getToken()->getLexme();
                argument.second = resultsGroup.getResults().at(++i).getValueType();
                arguments.push_back(argument);
                break;
            }
            case TAG_RETURN_TYPE:
                returnType = parseeResult.getValueType();
                break;
        }
    }

    return make_shared<StatementMetaExternFunction>(identifier, arguments, returnType, line, column);
}

shared_ptr<Statement> Parser::matchStatementVariable() {
    enum Tag {
        TAG_SHOULD_EXPORT,
        TAG_IDENTIFIER,
        TAG_VALUE_TYPE,
        TAG_EXPRESSION
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // export
            Parsee::tokenParsee(TokenKind::M_EXPORT, ParseeLevel::OPTIONAL, true, TAG_SHOULD_EXPORT),
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_IDENTIFIER),
            Parsee::valueTypeParsee(ParseeLevel::REQUIRED, true, TAG_VALUE_TYPE),
            // initializer
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::LEFT_ARROW, ParseeLevel::REQUIRED, false),
                    Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_EXPRESSION)
                }, ParseeLevel::OPTIONAL, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    bool shouldExport;
    string identifier;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_SHOULD_EXPORT: {
                shouldExport =  true;
                break;
            }
            case TAG_IDENTIFIER: {
                identifier = parseeResult.getToken()->getLexme();
                break;
            }
            case TAG_VALUE_TYPE: {
                valueType = parseeResult.getValueType();
                break;
            }
            case TAG_EXPRESSION: {
                expression = parseeResult.getExpression();
                break;
            }
        }
    }

    return make_shared<StatementVariable>(shouldExport, identifier, valueType, expression, line, column);
}

shared_ptr<Statement> Parser::matchStatementFunction() {
    enum {
        TAG_SHOULD_EXPORT,
        TAG_NAME,
        TAG_ARGUMENT_IDENTIFIER,
        TAG_ARGUMENT_TYPE,
        TAG_RETURN_TYPE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // export
            Parsee::tokenParsee(TokenKind::M_EXPORT, ParseeLevel::OPTIONAL, true, TAG_SHOULD_EXPORT),
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_NAME),
            Parsee::tokenParsee(TokenKind::FUNCTION, ParseeLevel::REQUIRED, false),
            // arguments
            Parsee::groupParsee(
                {
                    // first argument
                    Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE),
                    // additional arguments
                    Parsee::repeatedGroupParsee(
                        {
                            Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                            Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE)
                        }, ParseeLevel::OPTIONAL, true
                    )
                }, ParseeLevel::OPTIONAL, true
            ),
            // return type
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::tokenParsee(TokenKind::RIGHT_ARROW, ParseeLevel::REQUIRED, false),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_RETURN_TYPE)
                }, ParseeLevel::OPTIONAL, true
            ),
            // new line
            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    bool shouldExport = false;
    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    shared_ptr<Statement> statementBlock;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_SHOULD_EXPORT: {
                shouldExport = true;
                break;
            }
            case TAG_NAME: {
                name = parseeResult.getToken()->getLexme();
                break;
            }
            case TAG_ARGUMENT_IDENTIFIER: {
                pair<string, shared_ptr<ValueType>> argument;
                argument.first = parseeResult.getToken()->getLexme();
                argument.second = resultsGroup.getResults().at(++i).getValueType();
                arguments.push_back(argument);
                break;
            }
            case TAG_RETURN_TYPE: {
                returnType = parseeResult.getValueType();
                break;
            }
        }
    }

    // block
    statementBlock = matchStatementBlock({TokenKind::SEMICOLON, TokenKind::END});
    if (statementBlock == nullptr)
        return nullptr;

    // closing semicolon
    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true)) {
        markError(TokenKind::SEMICOLON, {}, {});
        return nullptr;
    }

    return make_shared<StatementFunction>(shouldExport, name, arguments, returnType, dynamic_pointer_cast<StatementBlock>(statementBlock), line, column);
}

shared_ptr<Statement> Parser::matchStatementRawFunction() {
    enum {
        TAG_NAME,
        TAG_CONSTRAINTS,
        TAG_ARGUMENT_IDENTIFIER,
        TAG_ARGUMENT_TYPE,
        TAG_RETURN_TYPE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_NAME),
            Parsee::tokenParsee(TokenKind::RAW_FUNCTION, ParseeLevel::REQUIRED, false),
            // constraints
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::LESS, ParseeLevel::CRITICAL, false),
                    Parsee::tokenParsee(TokenKind::STRING, ParseeLevel::CRITICAL, true, TAG_CONSTRAINTS),
                    Parsee::tokenParsee(TokenKind::GREATER, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::CRITICAL, true
            ),
            // arguments
            Parsee::groupParsee(
                {
                    // first argument
                    Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE),
                    // additional arguments
                    Parsee::repeatedGroupParsee(
                        {
                            Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_ARGUMENT_IDENTIFIER),
                            Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE)
                        }, ParseeLevel::OPTIONAL, true
                    )
                }, ParseeLevel::OPTIONAL, true
            ),
            // return type
        Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::RIGHT_ARROW, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_RETURN_TYPE)
                }, ParseeLevel::OPTIONAL, true
            ),
            // new line
            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false)
        }
    );

    string name;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    string rawSource;

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            for (int i=0; i<resultsGroup.getResults().size(); i++) {
                ParseeResult parseeResult = resultsGroup.getResults().at(i);
                switch (parseeResult.getTag()) {
                    case TAG_NAME:
                        name = parseeResult.getToken()->getLexme();
                        break;
                    case TAG_CONSTRAINTS:
                        constraints = parseeResult.getToken()->getLexme();
                        constraints = constraints.substr(1, constraints.length()-2);
                        break;
                    case TAG_ARGUMENT_IDENTIFIER: {
                        pair<string, shared_ptr<ValueType>> argument;
                        argument.first = parseeResult.getToken()->getLexme();
                        argument.second = resultsGroup.getResults().at(++i).getValueType();
                        arguments.push_back(argument);
                        break;
                    }
                    case TAG_RETURN_TYPE:
                        returnType = parseeResult.getValueType();
                        break;
                }
            }
            break;
        }
        case ParseeResultsGroupKind::NO_MATCH:
            return nullptr;
        case ParseeResultsGroupKind::FAILURE:
            break;
    }

    // source
    while (tryMatchingTokenKinds({TokenKind::RAW_SOURCE_LINE}, true, false)) {
        if (!rawSource.empty())
            rawSource += "\n";
        rawSource += tokens.at(currentIndex++)->getLexme();

        // Consume optional new line (for example because of a comment)
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);
    }

    // closing semicolon
    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true)) {
        markError(TokenKind::SEMICOLON, {}, {});
        return nullptr;
    }

    return make_shared<StatementRawFunction>(name, constraints, arguments, returnType, rawSource, line, column);
}

shared_ptr<Statement> Parser::matchStatementBlob() {
    enum Tag {
        TAG_SHOULD_EXPORT,
        TAG_NAME,
        TAG_MEMBER_IDENTIFIER,
        TAG_MEMBER_TYPE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // export
            Parsee::tokenParsee(TokenKind::M_EXPORT, ParseeLevel::OPTIONAL, true, TAG_SHOULD_EXPORT),
            // identifier
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_NAME),
            Parsee::tokenParsee(TokenKind::BLOB, ParseeLevel::REQUIRED, false),
            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::REQUIRED, false),
            // members
            Parsee::repeatedGroupParsee(
                {
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_MEMBER_IDENTIFIER),
                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_MEMBER_TYPE),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::OPTIONAL, true
            ),
            Parsee::tokenParsee(TokenKind::SEMICOLON, ParseeLevel::CRITICAL, false)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    bool shouldExport = false;
    string name;
    vector<pair<string, shared_ptr<ValueType>>> members;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_SHOULD_EXPORT: {
                shouldExport = true;
                break;
            }
            case TAG_NAME: {
                name = parseeResult.getToken()->getLexme();
                break;
            }
            case TAG_MEMBER_IDENTIFIER: {
                pair<string, shared_ptr<ValueType>> member;
                member.first = parseeResult.getToken()->getLexme();
                i++;
                member.second = resultsGroup.getResults().at(i).getValueType();
                members.push_back(member);
                break;
            }
        }
    }

    return make_shared<StatementBlob>(shouldExport, name, members, line, column);
}

shared_ptr<Statement> Parser::matchStatementBlock(vector<TokenKind> terminalTokenKinds) {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds(terminalTokenKinds, false, false)) {
        shared_ptr<Statement> statement = nextInBlockStatement();
        if (statement != nullptr)
            statements.push_back(statement);

        if (tryMatchingTokenKinds(terminalTokenKinds, false, false))
            break;

        // except new line
        if (statement != nullptr && !tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
            markError(TokenKind::NEW_LINE, {}, {});
    }

    return make_shared<StatementBlock>(statements, line, column);
}

shared_ptr<Statement> Parser::matchStatementAssignment() {
    enum {
        TAG_IDENTIFIER_PREFIX,
        TAG_IDENTIFIER,
        TAG_INDEX_EXPRESSION,
        TAG_VALUE_EXPRESSION
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // root chain
            // identifier - module prefix
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER_PREFIX),
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER_PREFIX)
                }, ParseeLevel::OPTIONAL, true
            ),
            // identifier - name
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_IDENTIFIER),
            // index expression
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, ParseeLevel::REQUIRED, false),
                    Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_INDEX_EXPRESSION),
                    Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::OPTIONAL, true
            ),
            // additional chains
            Parsee::repeatedGroupParsee(
                {
                    // dot separator in between
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::REQUIRED, false),
                    // identifier - name
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_IDENTIFIER),
                    // index expression
                    Parsee::groupParsee(
                        {
                            Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, ParseeLevel::REQUIRED, false),
                            Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_INDEX_EXPRESSION),
                            Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, ParseeLevel::CRITICAL, false)
                        }, ParseeLevel::OPTIONAL, true
                    ),
                }, ParseeLevel::OPTIONAL, true
            ),
            // value expression
            Parsee::tokenParsee(TokenKind::LEFT_ARROW, ParseeLevel::REQUIRED, false),
            Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_VALUE_EXPRESSION)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    vector<shared_ptr<Expression>> chainExpressions;
    shared_ptr<Expression> valueExpression;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        string identifier = "";
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER_PREFIX: {
                identifier += resultsGroup.getResults().at(i++).getToken()->getLexme(); // module
                identifier += resultsGroup.getResults().at(i++).getToken()->getLexme(); // dot
            }
            case TAG_IDENTIFIER: {
                identifier += resultsGroup.getResults().at(i).getToken()->getLexme(); // name
                // data
                if (i < resultsGroup.getResults().size() - 1 && resultsGroup.getResults().at(i+1).getTag() == TAG_INDEX_EXPRESSION) {
                    shared_ptr<Expression> indexExpression = resultsGroup.getResults().at(++i).getExpression();
                    shared_ptr<ExpressionValue> expression = ExpressionValue::data(identifier, indexExpression, line, column);
                    chainExpressions.push_back(expression);
                // simple
                } else {
                    shared_ptr<ExpressionValue> expression = ExpressionValue::simple(identifier, line, column);
                    chainExpressions.push_back(expression);
                }
                break;
            }
            case TAG_VALUE_EXPRESSION:
                valueExpression = parseeResult.getExpression();
                break;
        }
    }

    return make_shared<StatementAssignment>(
        make_shared<ExpressionChained>(chainExpressions, chainExpressions.front()->getLine(), chainExpressions.front()->getColumn()),
        valueExpression,
        line,
        column
    );
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::tokenParsee(TokenKind::RETURN, ParseeLevel::REQUIRED, false),
            Parsee::expressionParsee(ParseeLevel::OPTIONAL, true, false)
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    shared_ptr<Expression> expression = !resultsGroup.getResults().empty() ? resultsGroup.getResults().at(0).getExpression() : nullptr;

    return make_shared<StatementReturn>(expression, line, column);
}

shared_ptr<Statement> Parser::matchStatementRepeat() {
    enum Tag {
        TAG_STATEMENT_INIT,
        TAG_STATEMENT_POST,
        TAG_PRE_CONDITION,
        TAG_POST_CONDITION,
        TAG_STATEMENT_BLOCK
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::tokenParsee(TokenKind::REPEAT, ParseeLevel::REQUIRED, false),
            Parsee::oneOfParsee(
                {
                    // Has init
                    {
                        // init statement
                        Parsee::statementInBlockParsee(false, ParseeLevel::REQUIRED, true, TAG_STATEMENT_INIT),
                        // condition
                        Parsee::groupParsee(
                            {
                                // pre-condition
                                Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                                Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_PRE_CONDITION),
                                // post-condtion
                                Parsee::groupParsee(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                                        Parsee::expressionParsee(ParseeLevel::REQUIRED, true, false, TAG_POST_CONDITION),
                                    }, ParseeLevel::OPTIONAL, true
                                )
                            }, ParseeLevel::OPTIONAL, true
                        )
                    },
                    // No init
                    {
                        // pre-condition
                        Parsee::expressionParsee(ParseeLevel::REQUIRED, true, false, TAG_PRE_CONDITION),
                        // post-condtion
                        Parsee::groupParsee(
                            {
                                Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                                Parsee::expressionParsee(ParseeLevel::REQUIRED, true, false, TAG_POST_CONDITION),
                            }, ParseeLevel::OPTIONAL, true
                        )
                    },
                },ParseeLevel::OPTIONAL, true
            ),
            // post statement
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::statementInBlockParsee(true, ParseeLevel::CRITICAL, true, TAG_STATEMENT_POST),
                }, ParseeLevel::OPTIONAL, true
            ),
            // Statements
            Parsee::oneOfParsee(
                {
                    // single line
                    {
                        Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                        Parsee::statementBlockSingleLineParsee(ParseeLevel::CRITICAL, true, TAG_STATEMENT_BLOCK)
                    },
                    // multi line
                    {
                        Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::REQUIRED, false),
                        Parsee::statementBlockMultiLineParsee(ParseeLevel::CRITICAL, true, TAG_STATEMENT_BLOCK),
                        Parsee::tokenParsee(TokenKind::SEMICOLON, ParseeLevel::CRITICAL, false)
                    }
                }, ParseeLevel::CRITICAL, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    shared_ptr<Statement> initStatement;
    shared_ptr<Statement> postStatement;
    shared_ptr<Expression> preConditionExpression;
    shared_ptr<Expression> postConditionExpression;
    shared_ptr<Statement> bodyBlockStatement;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_STATEMENT_INIT:
                initStatement = parseeResult.getStatement();
                break;
            case TAG_STATEMENT_POST:
                postStatement = parseeResult.getStatement();
                break;
            case TAG_PRE_CONDITION:
                preConditionExpression = parseeResult.getExpression();
                break;
            case TAG_POST_CONDITION:
                postConditionExpression = parseeResult.getExpression();
                break;
            case TAG_STATEMENT_BLOCK:
                bodyBlockStatement = parseeResult.getStatement();
                break;
        }
    }

    return make_shared<StatementRepeat>(
        initStatement,
        postStatement,
        preConditionExpression,
        postConditionExpression,
        dynamic_pointer_cast<StatementBlock>(bodyBlockStatement),
        line,
        column
    );
}

shared_ptr<Statement> Parser::matchStatementExpression() {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    shared_ptr<Expression> expression = nextExpression();

    if (expression == nullptr)
        return nullptr;

    return make_shared<StatementExpression>(expression, line, column);
}

//
// Expressions
//
shared_ptr<Expression> Parser::nextExpression() {
    shared_ptr<Expression> expression;
    int errorsCount = errors.size();

    expression = matchLogicalOrXor();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;
    
    expression = matchExpressionIfElse();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;
    
    expression = matchExpressionVariable();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchLogicalOrXor() {
    shared_ptr<Expression> expression = matchLogicalAnd();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensLogicalOrXor, false, false))
        expression = matchExpressionBinary(expression);

    // Expression cannot be on left hand side of an assignment
    if (tokens.at(currentIndex)->isOfKind({TokenKind::LEFT_ARROW}))
        return nullptr;

    return expression;
}

shared_ptr<Expression> Parser::matchLogicalAnd() {
    shared_ptr<Expression> expression = matchLogicalNot();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensLogicalAnd, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchLogicalNot() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLogicalNot, false, true)) {
        shared_ptr<Expression> subExpression = matchLogicalNot();
        shared_ptr<ExpressionUnary> expression = ExpressionUnary::expression(token, subExpression);

        if (expression == nullptr)
            markError({}, {}, "Expected expression");
        
        return expression;
    }

    return matchEquality();
}

shared_ptr<Expression> Parser::matchEquality() {
    shared_ptr<Expression> expression = matchComparison();
    if (expression == nullptr)
        return nullptr;

    if (tryMatchingTokenKinds(Token::tokensEquality, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchComparison() {
    shared_ptr<Expression> expression = matchBitwiseOrXor();
    if (expression == nullptr)
        return nullptr;
    
    if (tryMatchingTokenKinds(Token::tokensComparison, false, false))
        expression = matchExpressionBinary(expression);
    
    return expression;
}

shared_ptr<Expression> Parser::matchBitwiseOrXor() {
    shared_ptr<Expression> expression = matchBitwiseAnd();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensBitwiseOrXor, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchBitwiseAnd() {
    shared_ptr<Expression> expression = matchBitwiseShift();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensBitwiseAnd, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchBitwiseShift() {
    shared_ptr<Expression> expression = matchBitwiseNot();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensBitwiseShift, false, false))
        expression = matchExpressionBinary(expression);

    return expression; 
}

shared_ptr<Expression> Parser::matchBitwiseNot() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensBitwiseNot, false, true)) {
        shared_ptr<Expression> subExpression = matchBitwiseNot();
        shared_ptr<ExpressionUnary> expression = ExpressionUnary::expression(token, subExpression);

        if (expression == nullptr)
            markError({}, {}, "Expected expression");
        
        return expression;
    }

    return matchTerm();
}

shared_ptr<Expression> Parser::matchTerm() {
    shared_ptr<Expression> expression = matchFactor();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensTerm, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchFactor() {
    shared_ptr<Expression> expression = matchUnary();
    if (expression == nullptr)
        return nullptr;

    while (tryMatchingTokenKinds(Token::tokensFactor, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchUnary() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensUnary, false, true)) {
        shared_ptr<Expression> subExpression = matchExpressionChained(nullptr);
        shared_ptr<ExpressionUnary> expression = ExpressionUnary::expression(token, subExpression);

        if (expression == nullptr)
            markError({}, {}, "Expected expression");
        
        return expression;
    }

    return matchExpressionChained(nullptr);
}

shared_ptr<Expression> Parser::matchExpressionChained(shared_ptr<ExpressionChained> parentExpression) {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    vector<shared_ptr<Expression>> chainExpressions;

    do {
        shared_ptr<Expression> expression = matchPrimary();
        if (expression != nullptr)
            chainExpressions.push_back(expression);
    } while (tryMatchingTokenKinds({TokenKind::DOT}, false, true));

    switch (chainExpressions.size()) {
        case 0:
            return nullptr;
        case 1:
            return chainExpressions.at(0);
        default:
            return make_shared<ExpressionChained>(chainExpressions, line, column);
    }
}

shared_ptr<Expression> Parser::matchPrimary() {
    shared_ptr<Expression> expression;
    int errorsCount = errors.size();

    expression = matchExpressionGrouping();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionCompositeLiteral();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionLiteral();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionCall();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionVariable();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionCast();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionGrouping() {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    if (tryMatchingTokenKinds({TokenKind::LEFT_ROUND_BRACKET}, true, true)) {
        shared_ptr<Expression> expression = matchLogicalOrXor();
        // has grouped expression failed?
        if (expression == nullptr) {
            return nullptr;
        } else if (tryMatchingTokenKinds({TokenKind::RIGHT_ROUND_BRACKET}, true, true)) {
            return make_shared<ExpressionGrouping>(expression, line, column);
        } else {
            markError(TokenKind::RIGHT_ROUND_BRACKET, {}, {});
        }
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionCompositeLiteral() {
    enum {
        TAG_EXPRESSION,
        TAG_STRING
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::oneOfParsee(
                {
                    {
                        Parsee::tokenParsee(TokenKind::LEFT_CURLY_BRACKET, ParseeLevel::REQUIRED, false),
                        // expressions
                        Parsee::groupParsee(
                            {
                                // first expression
                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                                Parsee::expressionParsee(ParseeLevel::REQUIRED, true, false, TAG_EXPRESSION),
                                // additional expressions
                                Parsee::repeatedGroupParsee(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                                        Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_EXPRESSION)
                                    }, ParseeLevel::OPTIONAL, true
                                ),
                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false)
                            }, ParseeLevel::OPTIONAL, true
                        ),
                        Parsee::tokenParsee(TokenKind::RIGHT_CURLY_BRACKET, ParseeLevel::CRITICAL, false)
                    },
                    {
                        Parsee::tokenParsee(TokenKind::STRING, ParseeLevel::REQUIRED, true, TAG_STRING)
                    }
                }, ParseeLevel::REQUIRED, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    vector<shared_ptr<Expression>> expressions;
    shared_ptr<Token> stringToken;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_EXPRESSION:
                expressions.push_back(parseeResult.getExpression());
                break;
            case TAG_STRING:
                stringToken = parseeResult.getToken();
                break;
        }
    }

    if (stringToken != nullptr)
        return ExpressionCompositeLiteral::expressionCompositeLiteralForTokenString(stringToken);
    else
        return ExpressionCompositeLiteral::expressionCompositeLiteralForExpressions(expressions, line, column);
}

shared_ptr<Expression> Parser::matchExpressionLiteral() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLiteral, false, true))
        return ExpressionLiteral::expressionLiteralForToken(token);

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionCall() {
    enum {
        TAG_NAME,
        TAG_ARGUMENT_EXPRESSION
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // identifier - module prefix
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_NAME),
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_NAME)
                }, ParseeLevel::OPTIONAL, true
            ),
            // identifier - name
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_NAME),
            // arguments
            Parsee::tokenParsee(TokenKind::LEFT_ROUND_BRACKET, ParseeLevel::REQUIRED, false),
            Parsee::groupParsee(
                {
                    // first argument
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                    Parsee::expressionParsee(ParseeLevel::REQUIRED, true, false, TAG_ARGUMENT_EXPRESSION),
                    // additional arguments
                    Parsee::repeatedGroupParsee(
                        {
                            Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false),
                            Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_ARGUMENT_EXPRESSION)
                        }, ParseeLevel::OPTIONAL, true
                    ),
                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::OPTIONAL, false)
                }, ParseeLevel::OPTIONAL, true
            ),
            Parsee::tokenParsee(TokenKind::RIGHT_ROUND_BRACKET, ParseeLevel::CRITICAL, false),
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_NAME:
                name += parseeResult.getToken()->getLexme();
                break;
            case TAG_ARGUMENT_EXPRESSION:
            argumentExpressions.push_back(parseeResult.getExpression());
                break;
        }
    }

    return make_shared<ExpressionCall>(name, argumentExpressions, line, column);
}

shared_ptr<Expression> Parser::matchExpressionVariable() {
    enum {
        TAG_IDENTIFIER,
        TAG_INDEX_EXPRESSION
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            // identifier - module prefix
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                    Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER),
                    Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_IDENTIFIER)
                }, ParseeLevel::OPTIONAL, true
            ),
            // identifier - name
            Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::REQUIRED, true, TAG_IDENTIFIER),
            // index expression
            Parsee::groupParsee(
                {
                    Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, ParseeLevel::REQUIRED, false),
                    Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_INDEX_EXPRESSION),
                    Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, ParseeLevel::CRITICAL, false)
                }, ParseeLevel::OPTIONAL, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string identifier;
    shared_ptr<Expression> indexExpression;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER:
                identifier += parseeResult.getToken()->getLexme();
                break;
            case TAG_INDEX_EXPRESSION:
                indexExpression = parseeResult.getExpression();
                break;
        }
    }

    if (indexExpression != nullptr)
        return ExpressionValue::data(identifier, indexExpression, line, column);
    else
        return ExpressionValue::simple(identifier, line, column);
}

shared_ptr<Expression> Parser::matchExpressionCast() {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup parseeResults = parseeResultsGroupForParsees(
        {
            Parsee::valueTypeParsee(ParseeLevel::REQUIRED, true)
        }
    );

    if (parseeResults.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    shared_ptr<ValueType> valueType = parseeResults.getResults().at(0).getValueType();

    return make_shared<ExpressionCast>(valueType, line, column);
}

shared_ptr<Expression> Parser::matchExpressionIfElse() {
    enum Tag {
        TAG_CONDITION,
        TAG_THEN,
        TAG_ELSE
    };

    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::tokenParsee(TokenKind::IF, ParseeLevel::REQUIRED, false),
            Parsee::expressionParsee(ParseeLevel::CRITICAL, true, false, TAG_CONDITION),
            Parsee::oneOfParsee(
                {
                    // Single line
                    {
                        Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                        Parsee::expressionBlockSingleLineParsee(ParseeLevel::CRITICAL, true, TAG_THEN),
                        Parsee::oneOfParsee(
                            {
                                // multi line else or else-if
                                {
                                    Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::REQUIRED, false),
                                    Parsee::tokenParsee(TokenKind::ELSE, ParseeLevel::REQUIRED, false),
                                    Parsee::oneOfParsee(
                                        {
                                            // else if
                                            {
                                                Parsee::ifElseParsee(ParseeLevel::REQUIRED, true, TAG_ELSE)
                                            },
                                            // multi-line else
                                            {
                                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::REQUIRED, false),
                                                Parsee::expressionBlockMultiLineParsee(ParseeLevel::CRITICAL, true, TAG_ELSE),
                                                Parsee::tokenParsee(TokenKind::SEMICOLON, ParseeLevel::CRITICAL, false)
                                            }
                                        }, ParseeLevel::CRITICAL, true
                                    )
                                },
                                // single-line else
                                {
                                    Parsee::tokenParsee(TokenKind::ELSE, ParseeLevel::REQUIRED, false),
                                    Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::CRITICAL, false),
                                    Parsee::expressionBlockSingleLineParsee(ParseeLevel::CRITICAL, true, TAG_ELSE)
                                }
                            }, ParseeLevel::OPTIONAL, true
                        )
                    },
                    // Multi line
                    {
                        Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false),
                        Parsee::expressionBlockMultiLineParsee(ParseeLevel::CRITICAL, true, TAG_THEN),
                        Parsee::oneOfParsee(
                            {
                                {
                                    Parsee::tokenParsee(TokenKind::ELSE, ParseeLevel::REQUIRED, false),
                                    Parsee::oneOfParsee(
                                        {
                                            // else if
                                            {
                                                Parsee::ifElseParsee(ParseeLevel::REQUIRED, true, TAG_ELSE)
                                            },
                                            // multi-line else
                                            {
                                                Parsee::tokenParsee(TokenKind::NEW_LINE, ParseeLevel::CRITICAL, false),
                                                Parsee::expressionBlockMultiLineParsee(ParseeLevel::CRITICAL, true, TAG_ELSE),
                                                Parsee::tokenParsee(TokenKind::SEMICOLON, ParseeLevel::CRITICAL, false)
                                            },
                                            // single-line else
                                            {
                                                Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::CRITICAL, false),
                                                Parsee::expressionBlockSingleLineParsee(ParseeLevel::CRITICAL, true, TAG_ELSE)
                                            }
                                        }, ParseeLevel::CRITICAL, true
                                    )
                                },
                                // no else
                                {
                                    Parsee::tokenParsee(TokenKind::SEMICOLON, ParseeLevel::CRITICAL, false)
                                }
                            }, ParseeLevel::CRITICAL, true
                        )                            
                    }
                }, ParseeLevel::CRITICAL, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    shared_ptr<Expression> condition;
    shared_ptr<ExpressionBlock> thenBlock;
    shared_ptr<Expression> elseBlock;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_CONDITION:
                condition = parseeResult.getExpression();
                break;
            case TAG_THEN:
                thenBlock = dynamic_pointer_cast<ExpressionBlock>(parseeResult.getExpression());
                break;
            case TAG_ELSE:
                elseBlock = parseeResult.getExpression();
                break;
        }
    }

    return make_shared<ExpressionIfElse>(condition, thenBlock, elseBlock, line, column);
}

shared_ptr<Expression> Parser::matchExpressionBinary(shared_ptr<Expression> left) {
    shared_ptr<Token> token = tokens.at(currentIndex);
    shared_ptr<Expression> right;
    // What level of binary expression are we having?
    if (tryMatchingTokenKinds(Token::tokensLogicalOrXor, false, true)) {
        right = matchLogicalAnd();
    } else if (tryMatchingTokenKinds(Token::tokensLogicalAnd, false, true)) {
        right = matchLogicalNot();
    } else if (tryMatchingTokenKinds(Token::tokensEquality, false, true)) {
        right = matchComparison();
    } else if (tryMatchingTokenKinds(Token::tokensComparison, false, true)) {
        right = matchBitwiseOrXor();
    } else if (tryMatchingTokenKinds(Token::tokensBitwiseOrXor, false, true)) {
        right = matchBitwiseAnd();
    } else if (tryMatchingTokenKinds(Token::tokensBitwiseAnd, false, true)) {
        right = matchBitwiseShift();
    } else if (tryMatchingTokenKinds(Token::tokensBitwiseShift, false, true)) {
        right = matchBitwiseNot();
    } else if (tryMatchingTokenKinds(Token::tokensTerm, false, true)) {
        right = matchFactor();
    } else if (tryMatchingTokenKinds(Token::tokensFactor, false, true)) {
        right = matchUnary();
    }

    shared_ptr<ExpressionBinary> expression = ExpressionBinary::expression(token, left, right);

    if (expression == nullptr)
        markError({}, {}, "Expected expression");

    return expression;
}

shared_ptr<Expression> Parser::matchExpressionBlock(vector<TokenKind> terminalTokenKinds) {
    int line = tokens.at(currentIndex)->getLine();
    int column = tokens.at(currentIndex)->getColumn();

    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds(terminalTokenKinds, false, false)) {
        shared_ptr<Statement> statement = nextInBlockStatement();

        if (statement != nullptr)
            statements.push_back(statement);

        if (tryMatchingTokenKinds(terminalTokenKinds, false, false))
            break;

        // except new line
        if (statement != nullptr && !tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true)) {
            markError(TokenKind::NEW_LINE, {}, {});
            return nullptr;
        }
    }

    return make_shared<ExpressionBlock>(statements, line, column);
}

shared_ptr<ValueType> Parser::matchValueType() {
    enum TAG {
        TAG_DATA,
        TAG_BLOB,
        TAG_PTR_FUN,
        TAG_PTR,
        TAG_ARGUMENT_TYPE,
        TAG_RETURN_TYPE,
        TAG_TYPE,
        TAG_SUBTYPE,
        TAG_SIZE_EXPRESSION,
        TAG_BLOB_NAME
    };

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(
        {
            Parsee::oneOfParsee(
                {
                    // PTR
                    {
                        Parsee::tokenParsee(TokenKind::PTR, ParseeLevel::REQUIRED, true, TAG_PTR),
                        Parsee::tokenParsee(TokenKind::LESS, ParseeLevel::CRITICAL, false),
                        Parsee::oneOfParsee(
                            {
                                // function pointer
                                {
                                    Parsee::tokenParsee(TokenKind::FUNCTION, ParseeLevel::REQUIRED, true, TAG_PTR_FUN),
                                    // arguments
                                    Parsee::groupParsee(
                                        {
                                            // colon
                                            Parsee::tokenParsee(TokenKind::COLON, ParseeLevel::REQUIRED, false),
                                            // first argument
                                            Parsee::valueTypeParsee(ParseeLevel::REQUIRED, true, TAG_ARGUMENT_TYPE),
                                            // addditional arguments
                                            Parsee::repeatedGroupParsee(
                                                {
                                                    Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                                    Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_ARGUMENT_TYPE)
                                                }, ParseeLevel::OPTIONAL, true
                                            )
                                        }, ParseeLevel::OPTIONAL, true
                                    ),
                                    // return type
                                    Parsee::groupParsee(
                                        {
                                            Parsee::tokenParsee(TokenKind::RIGHT_ARROW, ParseeLevel::REQUIRED, false),
                                            Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_RETURN_TYPE)
                                        }, ParseeLevel::OPTIONAL, true
                                    )
                                },
                                // other pointer
                                {
                                    Parsee::valueTypeParsee(ParseeLevel::REQUIRED, true, TAG_SUBTYPE)
                                }
                            }, ParseeLevel::CRITICAL, true
                        ),
                        Parsee::tokenParsee(TokenKind::GREATER, ParseeLevel::CRITICAL, false)
                    },
                    // DATA
                    {
                        Parsee::tokenParsee(TokenKind::DATA, ParseeLevel::REQUIRED, true, TAG_DATA),
                        Parsee::tokenParsee(TokenKind::LESS, ParseeLevel::CRITICAL, false),
                        Parsee::valueTypeParsee(ParseeLevel::CRITICAL, true, TAG_SUBTYPE),
                        Parsee::groupParsee(
                            {
                                Parsee::tokenParsee(TokenKind::COMMA, ParseeLevel::REQUIRED, false),
                                Parsee::expressionParsee(ParseeLevel::CRITICAL, true, true, TAG_SIZE_EXPRESSION)
                            }, ParseeLevel::OPTIONAL, true
                        ),
                        Parsee::tokenParsee(TokenKind::GREATER, ParseeLevel::CRITICAL, false)
                    },
                    // BLOB
                    {
                        Parsee::tokenParsee(TokenKind::BLOB, ParseeLevel::REQUIRED, true, TAG_BLOB),
                        Parsee::tokenParsee(TokenKind::LESS, ParseeLevel::REQUIRED, false),
                        // identifier - module prefix
                        Parsee::groupParsee(
                            {
                                Parsee::tokenParsee(TokenKind::META, ParseeLevel::REQUIRED, false),
                                Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_BLOB_NAME),
                                Parsee::tokenParsee(TokenKind::DOT, ParseeLevel::CRITICAL, true, TAG_BLOB_NAME)
                            }, ParseeLevel::OPTIONAL, true
                        ),
                        // identifier
                        Parsee::tokenParsee(TokenKind::IDENTIFIER, ParseeLevel::CRITICAL, true, TAG_BLOB_NAME),
                        Parsee::tokenParsee(TokenKind::GREATER, ParseeLevel::CRITICAL, false)
                    },
                    // SIMPLE
                    {
                        Parsee::tokenParsee(TokenKind::TYPE, ParseeLevel::REQUIRED, true, TAG_TYPE)
                    }
                }, ParseeLevel::REQUIRED, true
            )
        }
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    bool isData = false;
    bool isBlob = false;
    bool isPtrFun = false;
    bool isPtr = false;

    vector<shared_ptr<ValueType>> argTypes;
    shared_ptr<ValueType> retType;

    shared_ptr<Token> typeToken;
    shared_ptr<ValueType> subType;
    shared_ptr<Expression> countExpression;
    string blobName;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_DATA:
                isData = true;
                break;
            case TAG_BLOB:
                isBlob = true;
                break;
            case TAG_PTR_FUN:
                isPtrFun = true;
                break;
            case TAG_PTR:
                isPtr = true;
                break;
            case TAG_ARGUMENT_TYPE:
                argTypes.push_back(parseeResult.getValueType());
                break;
            case TAG_RETURN_TYPE:
                retType = parseeResult.getValueType();
                break;
            case TAG_TYPE:
                typeToken = parseeResult.getToken();
                break;
            case TAG_SUBTYPE:
                subType = parseeResult.getValueType();
                break;
            case TAG_SIZE_EXPRESSION:
                countExpression = parseeResult.getExpression();
                break;
            case TAG_BLOB_NAME:
                blobName += parseeResult.getToken()->getLexme();
                break;
        }
    }

    if (isData)
        return ValueType::data(subType, countExpression);
    else if (isBlob)
        return ValueType::blob(blobName);
    else if (isPtrFun)
        return ValueType::ptr(ValueType::fun(argTypes, retType));
    else if (isPtr)
        return ValueType::ptr(subType);
    else
        return ValueType::simpleForToken(typeToken);
}

shared_ptr<ValueType> Parser::typeForExportedStatementFromType(shared_ptr<ValueType> valueType, string moduleName) {
    switch (valueType->getKind()) {
        case ValueTypeKind::BLOB: {
            string name = *(valueType->getBlobName());
            if (name.find('.', 0) == string::npos && defaultModuleName.compare(moduleName) != 0) {
                name = moduleName + "." + name;
            }
            return ValueType::blob(name);
        }
        case ValueTypeKind::DATA:
            return ValueType::data(typeForExportedStatementFromType(valueType->getSubType(), moduleName), valueType->getCountExpression());
        case ValueTypeKind::PTR:
            return ValueType::ptr(typeForExportedStatementFromType(valueType->getSubType(), moduleName));
        default:
            return valueType;
    }
}

//
// Parsee
//
ParseeResultsGroup Parser::parseeResultsGroupForParsees(vector<Parsee> parsees) {
    int errorsCount = errors.size();
    int startIndex = currentIndex;
    vector<ParseeResult> parseeResults;

    for (Parsee &parsee : parsees) {
        optional<pair<vector<ParseeResult>, int>> subResults;
        switch (parsee.getKind()) {
            case ParseeKind::GROUP:
                subResults = groupParseeResults(*parsee.getGroupParsees());
                break;
            case ParseeKind::REPEATED_GROUP:
                subResults = repeatedGroupParseeResults(*parsee.getRepeatedParsees());
                break;
            case ParseeKind::TOKEN:
                subResults = tokenParseeResults(parsee.getTokenKind(), parsee.getTag());
                break;
            case ParseeKind::VALUE_TYPE:
                subResults = valueTypeParseeResults(currentIndex, parsee.getTag());
                break;
            case ParseeKind::STATEMENT:
                subResults = statementParseeResults(parsee.getTag());
                break;
            case ParseeKind::STATEMENT_IN_BLOCK:
                subResults = statementInBlockParseeResults(parsee.getShouldIncludeExpressionStatement(), parsee.getTag());
                break;
            case ParseeKind::EXPRESSION:
                subResults = expressionParseeResults(parsee.getIsNumericExpression(), parsee.getTag());
                break;
            case ParseeKind::ONE_OF:
                subResults = oneOfParseeResults(*parsee.getParsees());
                break;
            case ParseeKind::STATEMENT_BLOCK_SINGLE_LINE:
                subResults = statementBlockParseeResults(false, parsee.getTag());
                break;
            case ParseeKind::STATEMENT_BLOCK_MULTI_LINE:
                subResults = statementBlockParseeResults(true, parsee.getTag());
                break;
            case ParseeKind::EXPRESSION_BLOCK_SINGLE_LINE:
                subResults = expressionBlockSingleLineParseeResults(parsee.getTag());
                break;
            case ParseeKind::EXPRESSION_BLOCK_MULTI_LINE:
                subResults = expressionBlockMultiLineParseeResults(parsee.getTag());
                break;
            case ParseeKind::IF_ELSE:
                subResults = ifElseParseeResults(parsee.getTag());
                break;
        }

        // generated an error?
        if (errors.size() > errorsCount)
            return ParseeResultsGroup::failure();

        // if doesn't match a required but non-failing parsee
        if (!subResults && parsee.getLevel() == ParseeLevel::REQUIRED) {
            currentIndex = startIndex;
            return ParseeResultsGroup::noMatch();
        }

        // should return a matching result?
        if (subResults && parsee.getShouldReturn()) {
            for (ParseeResult &subResult : (*subResults).first)
                parseeResults.push_back(subResult);
        }

        // invalid sequence detected?
        if (!subResults && parsee.getLevel() == ParseeLevel::CRITICAL) {
            markError({}, parsee, {});
            return ParseeResultsGroup::failure();
        }

        // got to the next token if we got a match
        if (subResults)
            currentIndex += (*subResults).second;
    }

    return ParseeResultsGroup::success(parseeResults);
}

optional<pair<vector<ParseeResult>, int>> Parser::groupParseeResults(vector<Parsee> groupParsees) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    
    ParseeResultsGroup resultsGroup = parseeResultsGroupForParsees(groupParsees);
    if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE)
        return {};

    for (ParseeResult &result : resultsGroup.getResults())
        results.push_back(result);
    
    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(results, tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::repeatedGroupParseeResults(vector<Parsee> repeatedParsees) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    
    ParseeResultsGroup resultsGroup;
    do {
        resultsGroup = parseeResultsGroupForParsees(repeatedParsees);
        if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE)
            return {};

        for (ParseeResult &result : resultsGroup.getResults())
            results.push_back(result);
    } while (resultsGroup.getKind() == ParseeResultsGroupKind::SUCCESS);
    
    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(results, tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::oneOfParseeResults(vector<vector<Parsee>> parseeGroups) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    ParseeResultsGroup resultsGroup;

    int groupIndex = 0;
    while (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS && groupIndex < parseeGroups.size()) {
        vector<Parsee> parseeGroup = parseeGroups.at(groupIndex);
        resultsGroup = parseeResultsGroupForParsees(parseeGroup);
        if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE) {
            return {};
        } else if (resultsGroup.getKind() == ParseeResultsGroupKind::NO_MATCH) {
            currentIndex = startIndex;
        }
        groupIndex++;
    }

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return {};

    for (ParseeResult &result : resultsGroup.getResults())
        results.push_back(result);

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(results, tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::tokenParseeResults(TokenKind tokenKind, int tag) {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (token->isOfKind({tokenKind}))
        return pair(vector<ParseeResult>({ParseeResult::tokenResult(token, tag)}), 1);
    return {};
}

optional<pair<vector<ParseeResult>, int>> Parser::valueTypeParseeResults(int index, int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();

    shared_ptr<ValueType> valueType = matchValueType();
    if (errors.size() > errorsCount || valueType == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::valueTypeResult(valueType, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::statementParseeResults(int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Statement> statement = nextStatement();
    if (errors.size() > errorsCount || statement == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::statementResult(statement, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::statementInBlockParseeResults(bool shouldIncludeExpressionStatement, int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Statement> statement;
    if (shouldIncludeExpressionStatement) {
        statement = nextInBlockStatement();
    } else {
        statement = matchStatementVariable() ?: matchStatementAssignment();
    }
    if (errors.size() > errorsCount || statement == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::statementInBlockResult(statement, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::expressionParseeResults(bool isNumeric, int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression;
    if (isNumeric)
        expression = matchBitwiseOrXor();
    else
        expression = nextExpression();
    if (errors.size() > errorsCount || expression == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::statementBlockParseeResults(bool isMultiline, int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Statement> statement;
    if (isMultiline)
        statement = matchStatementBlock({TokenKind::SEMICOLON, TokenKind::END});
    else
        statement = matchStatementBlock({TokenKind::NEW_LINE, TokenKind::COMMA, TokenKind::END});

    if (errors.size() > errorsCount || statement == nullptr)
        return {};
    
    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::statementInBlockResult(statement, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::expressionBlockSingleLineParseeResults(int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression = matchExpressionBlock({TokenKind::ELSE, TokenKind::NEW_LINE});
    if (errors.size() > errorsCount || expression == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::expressionBlockMultiLineParseeResults(int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression = matchExpressionBlock({TokenKind::ELSE, TokenKind::SEMICOLON});
    if (errors.size() > errorsCount || expression == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::ifElseParseeResults(int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression = matchExpressionIfElse();
    if (errors.size() > errorsCount || expression == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount, tag)}), tokensCount);
}

//
// Support
//
bool Parser::tryMatchingTokenKinds(vector<TokenKind> kinds, bool shouldMatchAll, bool shouldAdvance) {
    int requiredCount = shouldMatchAll ? kinds.size() : 1;
    if (currentIndex + requiredCount > tokens.size())
        return false;
    
    if (shouldMatchAll) {
        for (int i=0; i<kinds.size(); i++) {
            if (kinds.at(i) != tokens.at(currentIndex + i)->getKind())
                return false;
        }

        if (shouldAdvance)
            currentIndex += kinds.size();
        
        return true;
    } else {
        for (int i=0; i<kinds.size(); i++) {
            if (kinds.at(i) == tokens.at(currentIndex)->getKind()) {
                if (shouldAdvance)
                    currentIndex++;
                return true;
            }
        }

        return false;
    }
}

void Parser::markError(optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message) {
    shared_ptr<Token> actualToken = tokens.at(currentIndex);

    // Try reaching the next safe token
    vector<TokenKind> safeKinds = {TokenKind::END};
    if (!actualToken->isOfKind({TokenKind::NEW_LINE}))
        safeKinds.push_back(TokenKind::NEW_LINE);
    if (!actualToken->isOfKind({TokenKind::SEMICOLON}))
        safeKinds.push_back(TokenKind::SEMICOLON);

    while (!tryMatchingTokenKinds(safeKinds, false, true))
        currentIndex++;

    // Last END should not be consumed
    if (currentIndex > tokens.size() - 1)
        currentIndex = tokens.size() - 1;

    errors.push_back(Error::parserError(actualToken, expectedTokenKind, expectedParsee, message));
}
