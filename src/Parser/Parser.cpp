#include "Parser.h"

#include "Error.h"
#include "Logger.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionChained.h"

#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementImport.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementRepeat.h"

#include "Parsee/Parsee.h"
#include "Parsee/ParseeGroup.h"
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
// Statement
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

    string name = defaultModuleName;
    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;
    vector<shared_ptr<Statement>> exportedHeaderStatements;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::M_MODULE, true, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_NAME),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
                        }
                    ), false, true, false
                ),
                Parsee::repeatedGroupParsee(
                    ParseeGroup(
                        {
                            Parsee::statementParsee(true, true, false, TAG_STATEMENT),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
                        }
                    ), false, true, false
                ),
                Parsee::tokenParsee(TokenKind::END, true, false, true)
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    vector<shared_ptr<Statement>> blobDeclarationStatements;
    vector<shared_ptr<Statement>> blobStatements;
    vector<shared_ptr<Statement>> functionDeclarationStatements;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_NAME:
                name = parseeResult.getToken()->getLexme();
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
                            statementFunction->getReturnValueType()
                        );
                        functionDeclarationStatements.push_back(statementFunctionDeclaration);
                        statements.push_back(statement);

                        if (statementFunction->getShouldExport())
                            exportedHeaderStatements.push_back(statementFunctionDeclaration);
                        break;
                    }
                    case StatementKind::BLOB: { //generate blob declaration
                        shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                        shared_ptr<StatementBlobDeclaration> statementBlobDeclaration = make_shared<StatementBlobDeclaration>(statementBlob->getIdentifier());
                        blobStatements.push_back(statementBlob);
                        blobDeclarationStatements.push_back(statementBlobDeclaration);
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

    // arrange header
    for (shared_ptr<Statement> &statement : blobDeclarationStatements)
        headerStatements.push_back(statement);

    for (shared_ptr<Statement> &statement : blobStatements)
        headerStatements.push_back(statement);

    for (shared_ptr<Statement> &statement : functionDeclarationStatements)
        headerStatements.push_back(statement);

    return make_shared<StatementModule>(name, statements, headerStatements, exportedHeaderStatements);
}

shared_ptr<Statement> Parser::matchStatementImport() {
    string name;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::M_IMPORT, true, false, false),
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true)
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    name = resultsGroup.getResults().at(0).getToken()->getLexme();

    return make_shared<StatementImport>(name);
}

shared_ptr<Statement> Parser::matchStatementMetaExternFunction() {
    enum {
        TAG_ID,
        TAG_ARG_ID,
        TAG_ARG_TYPE,
        TAG_RET_TYPE
    };

    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::M_EXTERN, true, false, false),
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_ID),
                Parsee::tokenParsee(TokenKind::FUNCTION, true, false, false),
                // arguments
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ARG_ID),
                            Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ID),
                                        Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE)
                                    }
                                ), false, true, false
                            )
                        }
                    ), false, true, false
                ),
                // return type
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::RIGHT_ARROW, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::valueTypeParsee(true, true, true, TAG_RET_TYPE)
                        }
                    ), false, true, false
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_ID:
                identifier = parseeResult.getToken()->getLexme();
                break;
            case TAG_ARG_ID: {
                pair<string, shared_ptr<ValueType>> argument;
                argument.first = parseeResult.getToken()->getLexme();
                argument.second = resultsGroup.getResults().at(++i).getValueType();
                arguments.push_back(argument);
                break;
            }
            case TAG_RET_TYPE:
                returnType = parseeResult.getValueType();
                break;
        }
    }

    return make_shared<StatementMetaExternFunction>(identifier, arguments, returnType);
}

shared_ptr<Statement> Parser::matchStatementVariable() {
    enum Tag {
        TAG_IDENTIFIER,
        TAG_TYPE,
        TAG_EXPRESSION
    };

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER),
                Parsee::valueTypeParsee(true, true, false, TAG_TYPE),
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                            Parsee::expressionParsee(true, true, true, TAG_EXPRESSION)
                        }
                    ), false, true, false
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string identifier;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression = nullptr;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER:
                identifier = parseeResult.getToken()->getLexme();
                break;
            case TAG_TYPE:
                valueType = parseeResult.getValueType();
                break;
            case TAG_EXPRESSION:
                expression = parseeResult.getExpression();
                break;
        }
    }

    return make_shared<StatementVariable>(identifier, valueType, expression);
}

shared_ptr<Statement> Parser::matchStatementFunction() {
    enum {
        TAG_SHOULD_EXPORT,
        TAG_ID,
        TAG_ARG_ID,
        TAG_ARG_TYPE,
        TAG_RET_TYPE
    };

    bool shouldExport = false;
    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    shared_ptr<Statement> statementBlock;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // export
                Parsee::tokenParsee(TokenKind::M_EXPORT, false, true, false, TAG_SHOULD_EXPORT),
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_ID),
                Parsee::tokenParsee(TokenKind::FUNCTION, true, false, false),
                // arguments
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ARG_ID),
                            Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ARG_ID),
                                        Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE)
                                    }
                                ), false, true, false
                            )
                        }
                    ), false, true, false
                ),
                // return type
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::RIGHT_ARROW, true, false, false),
                            Parsee::valueTypeParsee(true, true, true, TAG_RET_TYPE)
                        }
                    ), false, true, false
                ),
                // new line
                Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            for (int i=0; i<resultsGroup.getResults().size(); i++) {
                ParseeResult parseeResult = resultsGroup.getResults().at(i);
                switch (parseeResult.getTag()) {
                    case TAG_SHOULD_EXPORT:
                        shouldExport = true;
                        break;
                    case TAG_ID:
                        identifier = parseeResult.getToken()->getLexme();
                        break;
                    case TAG_ARG_ID: {
                        pair<string, shared_ptr<ValueType>> argument;
                        argument.first = parseeResult.getToken()->getLexme();
                        argument.second = resultsGroup.getResults().at(++i).getValueType();
                        arguments.push_back(argument);
                        break;
                    }
                    case TAG_RET_TYPE:
                        returnType = parseeResult.getValueType();
                        break;
                }
            }
            break;
        }
        case ParseeResultsGroupKind::NO_MATCH:
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
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

    return make_shared<StatementFunction>(shouldExport, identifier, arguments, returnType, dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementRawFunction() {
    enum {
        TAG_ID,
        TAG_CONSTRAINTS,
        TAG_ARG_ID,
        TAG_ARG_TYPE,
        TAG_RET_TYPE
    };

    string identifier;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    string rawSource;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_ID),
                Parsee::tokenParsee(TokenKind::RAW_FUNCTION, true, false, false),
                // constraints
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::LESS, true, false, true),
                            Parsee::tokenParsee(TokenKind::STRING, true, true, true, TAG_CONSTRAINTS),
                            Parsee::tokenParsee(TokenKind::GREATER, true, false, true)
                        }
                    ), true, true, true
                ),
                // arguments
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ARG_ID),
                            Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true, TAG_ARG_ID),
                                        Parsee::valueTypeParsee(true, true, true, TAG_ARG_TYPE)
                                    }
                                ), false, true, false
                            )
                        }
                    ), false, true, false
                ),
                // return type
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::RIGHT_ARROW, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::valueTypeParsee(true, true, true, TAG_RET_TYPE)
                        }
                    ), false, true, false
                ),
                // new line
                Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            for (int i=0; i<resultsGroup.getResults().size(); i++) {
                ParseeResult parseeResult = resultsGroup.getResults().at(i);
                switch (parseeResult.getTag()) {
                    case TAG_ID:
                        identifier = parseeResult.getToken()->getLexme();
                        break;
                    case TAG_CONSTRAINTS:
                        constraints = parseeResult.getToken()->getLexme();
                        break;
                    case TAG_ARG_ID: {
                        pair<string, shared_ptr<ValueType>> argument;
                        argument.first = parseeResult.getToken()->getLexme();
                        argument.second = resultsGroup.getResults().at(++i).getValueType();
                        arguments.push_back(argument);
                        break;
                    }
                    case TAG_RET_TYPE:
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

    return make_shared<StatementRawFunction>(identifier, constraints, arguments, returnType, rawSource);
}

shared_ptr<Statement> Parser::matchStatementBlob() {
    enum Tag {
        TAG_IDENTIFIER,
        TAG_MEMBER_IDENTIFIER,
        TAG_MEMBER_TYPE
    };

    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> members;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER),
                Parsee::tokenParsee(TokenKind::BLOB, true, false, false),
                Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, false),
                Parsee::repeatedGroupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_MEMBER_IDENTIFIER),
                            Parsee::valueTypeParsee(true, true, true, TAG_MEMBER_TYPE),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
                        }
                    ), false, true, false
                ),
                Parsee::tokenParsee(TokenKind::SEMICOLON, true, false, true)
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER:
                identifier = parseeResult.getToken()->getLexme();
                break;
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

    return make_shared<StatementBlob>(identifier, members);
}

shared_ptr<Statement> Parser::matchStatementBlock(vector<TokenKind> terminalTokenKinds) {
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

    return make_shared<StatementBlock>(statements);
}

shared_ptr<Statement> Parser::matchStatementAssignment() {
    enum {
        TAG_IDENTIFIER_SIMPLE,
        TAG_IDENTIFIER_DATA,
        TAG_INDEX_EXPRESSION,
        TAG_VALUE_EXPRESSION
    };

    vector<shared_ptr<Expression>> chainExpressions;
    shared_ptr<Expression> valueExpression;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // root chain
                Parsee::orParsee(
                    // data
                    ParseeGroup(
                        {
                            // identifier
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER_DATA),
                            // index expression
                            Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, true, false, false),
                            Parsee::expressionParsee(true, true, true, TAG_INDEX_EXPRESSION),
                            Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, true, false, true),
                        }
                    ),
                    // simple
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER_SIMPLE)
                        }
                    ), true, true, false
                ),
                // additional chains
                Parsee::repeatedGroupParsee(
                    ParseeGroup(
                        {
                            // dot separator in between
                            Parsee::tokenParsee(TokenKind::DOT, true, false, false),
                            Parsee::orParsee(
                                // .data[]
                                ParseeGroup(
                                    {
                                        // identifier
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER_DATA),
                                        // index expression
                                        Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, true, false, false),
                                        Parsee::expressionParsee(true, true, true, TAG_INDEX_EXPRESSION),
                                        Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, true, false, true),
                                    }
                                ),
                                // .simple
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false, TAG_IDENTIFIER_SIMPLE)
                                    }
                                ), true, true, true
                            )
                        }
                    ), false, true, false
                ),
                // value expression
                Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                Parsee::expressionParsee(true, true, true, TAG_VALUE_EXPRESSION)
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    for (int i=0; i<resultsGroup.getResults().size(); i++) {
        ParseeResult parseeResult = resultsGroup.getResults().at(i);
        switch (parseeResult.getTag()) {
            case TAG_IDENTIFIER_SIMPLE: {
                shared_ptr<ExpressionVariable> expression = ExpressionVariable::simple(parseeResult.getToken()->getLexme());
                chainExpressions.push_back(expression);
                break;
            }
            case TAG_IDENTIFIER_DATA: {
                string identifier = parseeResult.getToken()->getLexme();
                shared_ptr<Expression> indexExpression = resultsGroup.getResults().at(++i).getExpression();
                shared_ptr<ExpressionVariable> expression = ExpressionVariable::data(identifier, indexExpression);
                chainExpressions.push_back(expression);
                break;
            }
            case TAG_VALUE_EXPRESSION: {
                valueExpression = parseeResult.getExpression();
                break;
            }
        }
    }

    return make_shared<StatementAssignment>(chainExpressions, valueExpression);
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::RETURN, true, false, false),
                Parsee::expressionParsee(false, true, false)
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    shared_ptr<Expression> expression = !resultsGroup.getResults().empty() ? resultsGroup.getResults().at(0).getExpression() : nullptr;

    return make_shared<StatementReturn>(expression);
}

shared_ptr<Statement> Parser::matchStatementRepeat() {
    enum Tag {
        TAG_STATEMENT_INIT,
        TAG_STATEMENT_POST,
        TAG_PRE_CONDITION,
        TAG_POST_CONDITION,
        TAG_STATEMENT_BLOCK
    };

    shared_ptr<Statement> initStatement;
    shared_ptr<Statement> postStatement;
    shared_ptr<Expression> preConditionExpression;
    shared_ptr<Expression> postConditionExpression;
    shared_ptr<Statement> bodyBlockStatement;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::REPEAT, true, false, false),
                Parsee::orParsee(
                    // Has init
                    ParseeGroup(
                        {
                            // init statement
                            Parsee::statementInBlockParsee(false, true, true, false, TAG_STATEMENT_INIT),
                            // condition
                            Parsee::groupParsee(
                                ParseeGroup(
                                    {
                                        // pre-condition
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::expressionParsee(true, true, true, TAG_PRE_CONDITION),
                                        // post-condtion
                                        Parsee::groupParsee(
                                            ParseeGroup(
                                                {
                                                    Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                                    Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                                    Parsee::expressionParsee(true, true, false, TAG_POST_CONDITION),
                                                }
                                            ), false, true, false
                                        )
                                    }
                                ), false, true, false
                            )
                        }
                    ),
                    // No init
                    ParseeGroup(
                        {
                            // pre-condition
                            Parsee::expressionParsee(true, true, false, TAG_PRE_CONDITION),
                            // post-condtion
                            Parsee::groupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::expressionParsee(true, true, false, TAG_POST_CONDITION),
                                    }
                                ), false, true, false
                            )
                        }
                    ),
                    false, true, false
                ),
                // post statement
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::statementInBlockParsee(true, true, true, true, TAG_STATEMENT_POST),
                        }
                    ), false, true, false
                ),
                // Statements
                Parsee::orParsee(
                    // single line
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::statementBlockSingleLineParsee(true, true, true, TAG_STATEMENT_BLOCK)
                        }
                    ),
                    // multi line
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, false),
                            Parsee::statementBlockMultiLineParsee(true, true, true, TAG_STATEMENT_BLOCK),
                            Parsee::tokenParsee(TokenKind::SEMICOLON, true, false, true)
                        }
                    ), true, true, true
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

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
        dynamic_pointer_cast<StatementBlock>(bodyBlockStatement)
    );
}

shared_ptr<Statement> Parser::matchStatementExpression() {
    shared_ptr<Expression> expression = nextExpression();

    if (expression == nullptr)
        return nullptr;

    return make_shared<StatementExpression>(expression);
}

//
// Expression
//
shared_ptr<Expression> Parser::nextExpression() {
    shared_ptr<Expression> expression;
    int errorsCount = errors.size();

    expression = matchLogicalSecond();
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

shared_ptr<Expression> Parser::matchLogicalSecond() {
    shared_ptr<Expression> expression = matchLogicalFirst();
    if (expression == nullptr)
        return nullptr;

    if (tryMatchingTokenKinds(Token::tokensLogicalSecond, false, false))
        expression = matchExpressionBinary(expression);

    // Expression cannot be on left hand side of an assignment
    if (tokens.at(currentIndex)->isOfKind({TokenKind::LEFT_ARROW}))
        return nullptr;

    return expression;
}

shared_ptr<Expression> Parser::matchLogicalFirst() {
    shared_ptr<Expression> expression = matchLogicalUnary();
    if (expression == nullptr)
        return nullptr;

    if (tryMatchingTokenKinds(Token::tokensLogicalFirst, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchLogicalUnary() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLogicalUnary, false, true)) {
        shared_ptr<Expression> expression = matchLogicalUnary();
        if (expression == nullptr)
            return nullptr;
        return make_shared<ExpressionUnary>(token, expression);
    }

    return matchEquality();
}

shared_ptr<Expression> Parser::matchEquality() {
    shared_ptr<Expression> expression = matchComparison();
    if (expression == nullptr)
        return nullptr;

    if (tryMatchingTokenKinds({Token::tokensEquality}, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchComparison() {
    shared_ptr<Expression> expression = matchTerm();
    if (expression == nullptr)
        return nullptr;
    
    if (tryMatchingTokenKinds({Token::tokensComparison}, false, false))
        expression = matchExpressionBinary(expression);
    
    return expression;
}

shared_ptr<Expression> Parser::matchTerm() {
    shared_ptr<Expression> expression = matchFactor();
    if (expression == nullptr)
        return nullptr;

    if (tryMatchingTokenKinds({Token::tokensTerm}, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchFactor() {
    shared_ptr<Expression> expression = matchUnary();
    if (expression == nullptr)
        return nullptr;

    if (tokens.at(currentIndex)->isOfKind(Token::tokensFactor))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchUnary() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensUnary, false, true)) {
        shared_ptr<Expression> expression = matchExpressionChained(nullptr);
        if (expression == nullptr)
            return nullptr;
        return make_shared<ExpressionUnary>(token, expression);
    }

    return matchExpressionChained(nullptr);
}

shared_ptr<Expression> Parser::matchExpressionChained(shared_ptr<ExpressionChained> parentExpression) {
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
            return make_shared<ExpressionChained>(chainExpressions);
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

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionGrouping() {
    if (tryMatchingTokenKinds({TokenKind::LEFT_ROUND_BRACKET}, true, true)) {
        shared_ptr<Expression> expression = matchLogicalSecond();
        // has grouped expression failed?
        if (expression == nullptr) {
            return nullptr;
        } else if (tryMatchingTokenKinds({TokenKind::RIGHT_ROUND_BRACKET}, true, true)) {
            return make_shared<ExpressionGrouping>(expression);
        } else {
            markError(TokenKind::RIGHT_ROUND_BRACKET, {}, {});
        }
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionLiteral() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLiteral, false, true))
        return ExpressionLiteral::expressionLiteralForToken(token);

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionCompositeLiteral() {
    enum {
        TAG_EXPRESSION,
        TAG_STRING
    };

    vector<shared_ptr<Expression>> expressions;
    shared_ptr<Token> stringToken;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::orParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::LEFT_CURLY_BRACKET, true, false, false),
                            // expressions
                            Parsee::groupParsee(
                                ParseeGroup(
                                    {
                                        // first expression
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::expressionParsee(true, true, false, TAG_EXPRESSION),
                                        // additional expressions
                                        Parsee::repeatedGroupParsee(
                                            ParseeGroup(
                                                {
                                                    Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                                    Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                                    Parsee::expressionParsee(true, true, true, TAG_EXPRESSION)
                                                }
                                            ), false, true, false
                                        ),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false)
                                    }
                                ), false, true, false
                            ),
                            Parsee::tokenParsee(TokenKind::RIGHT_CURLY_BRACKET, true, false, true)
                        }
                    ),
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::STRING, true, true, false, TAG_STRING)
                        }
                    ), true, true, false
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

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
        return ExpressionCompositeLiteral::expressionCompositeLiteralForExpressions(expressions);
}

shared_ptr<Expression> Parser::matchExpressionVariable() {
    ParseeResultsGroup resultsGroup;

    // data
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // index expression
                Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, true, false, false),
                Parsee::expressionParsee(true, true, true),
                Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, true, false, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            shared_ptr<Expression> indexExpression = resultsGroup.getResults().at(1).getExpression();
            return ExpressionVariable::data(identifier, indexExpression);
        }
        case ParseeResultsGroupKind::NO_MATCH:
            break;
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
    }

    // simple
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            return ExpressionVariable::simple(identifier);
        }
        case ParseeResultsGroupKind::NO_MATCH:
            break;
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionCall() {
    string identifier;
    vector<shared_ptr<Expression>> argumentExpressions;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier - module prefix
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::META, true, true, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                            Parsee::tokenParsee(TokenKind::DOT, true, false, true)
                        }
                    ), false, true, false
                ),
                // identifier - name
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // arguments
                Parsee::tokenParsee(TokenKind::LEFT_ROUND_BRACKET, true, false, false),
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::expressionParsee(true, true, false),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::expressionParsee(true, true, true)
                                    }
                                ), false, true, false
                            ),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false)
                        }
                    ), false, true, false
                ),
                Parsee::tokenParsee(TokenKind::RIGHT_ROUND_BRACKET, true, false, true),
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    int i = 0;
    // identifier
    identifier = "";
    if (resultsGroup.getResults().at(i).getToken()->isOfKind({TokenKind::META})) {
        i++;
        identifier += resultsGroup.getResults().at(i++).getToken()->getLexme();
        identifier += ".";
    }
    identifier += resultsGroup.getResults().at(i++).getToken()->getLexme();
    // arguments
    while (i < resultsGroup.getResults().size()) {
        argumentExpressions.push_back(resultsGroup.getResults().at(i).getExpression());
        i++;
    }

    return make_shared<ExpressionCall>(identifier, argumentExpressions);
}

shared_ptr<Expression> Parser::matchExpressionIfElse() {
    enum Tag {
        TAG_CONDITION,
        TAG_THEN,
        TAG_ELSE
    };

    shared_ptr<Expression> condition;
    shared_ptr<Expression> thenBlock;
    shared_ptr<Expression> elseBlock;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::IF, true, false, false),
                Parsee::expressionParsee(true, true, true, TAG_CONDITION),
                Parsee::orParsee(
                    // Single line
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::expressionBlockSingleLineParsee(true, true, true, TAG_THEN),
                            // Else block
                            Parsee::groupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::ELSE, true, false, false),
                                        Parsee::tokenParsee(TokenKind::COLON, true, false, true),
                                        Parsee::expressionBlockSingleLineParsee(true, true, true, TAG_ELSE)
                                    }
                                ), false, true, false
                            )
                        }
                    ),
                    // Multi line
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true),
                            Parsee::expressionBlockMultiLineParsee(true, true, true, TAG_THEN),
                            // Else blcok
                            Parsee::groupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::ELSE, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true),
                                        Parsee::expressionBlockMultiLineParsee(true, true, true, TAG_ELSE)
                                    }
                                ), false, true, false
                            ),
                            Parsee::tokenParsee(TokenKind::SEMICOLON, true, false, true)
                        }
                    ), true, true, true
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    for (ParseeResult &parseeResult : resultsGroup.getResults()) {
        switch (parseeResult.getTag()) {
            case TAG_CONDITION:
                condition = parseeResult.getExpression();
                break;
            case TAG_THEN:
                thenBlock = parseeResult.getExpression();
                break;
            case TAG_ELSE:
                elseBlock = parseeResult.getExpression();
                break;
        }
    }

    return make_shared<ExpressionIfElse>(condition, dynamic_pointer_cast<ExpressionBlock>(thenBlock), dynamic_pointer_cast<ExpressionBlock>(elseBlock));
}

shared_ptr<Expression> Parser::matchExpressionBinary(shared_ptr<Expression> left) {
    shared_ptr<Token> token = tokens.at(currentIndex);
    shared_ptr<Expression> right;
    // What level of binary expression are we having?
    if (tryMatchingTokenKinds(Token::tokensLogicalSecond, false, true)) {
        right = matchLogicalSecond();
    } else if (tryMatchingTokenKinds(Token::tokensLogicalFirst, false, true)) {
        right = matchLogicalFirst();
    } else if (tryMatchingTokenKinds(Token::tokensEquality, false, true)) {
        right = matchComparison();
    } else if (tryMatchingTokenKinds(Token::tokensComparison, false, true)) {
        right = matchTerm();
    } else if (tryMatchingTokenKinds(Token::tokensTerm, false, true)) {
        right = matchTerm();
    } else if (tryMatchingTokenKinds(Token::tokensFactor, false, true)) {
        right = matchFactor();
    }

    if (right == nullptr) {
        markError({}, {}, "Expected expression");
        return nullptr;
    } else {
        return make_shared<ExpressionBinary>(token, left, right);
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionBlock(vector<TokenKind> terminalTokenKinds) {
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

    return make_shared<ExpressionBlock>(statements);
}

ParseeResultsGroup Parser::parseeResultsGroupForParseeGroup(ParseeGroup group) {
    int errorsCount = errors.size();
    int startIndex = currentIndex;
    vector<ParseeResult> parseeResults;

    for (Parsee &parsee : group.getParsees()) {
        optional<pair<vector<ParseeResult>, int>> subResults;
        switch (parsee.getKind()) {
            case ParseeKind::GROUP:
                subResults = groupParseeResults(*parsee.getGroup());
                break;
            case ParseeKind::REPEATED_GROUP:
                subResults = repeatedGroupParseeResults(*parsee.getRepeatedGroup());
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
                subResults = expressionParseeResults(parsee.getTag());
                break;
            case ParseeKind::OR:
                subResults = orParseeResults(*parsee.getFirstGroup(), *parsee.getSecondGroup());
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
        }

        // generated an error?
        if (errors.size() > errorsCount)
            return ParseeResultsGroup::failure();

        // if doesn't match a required but non-failing parsee
        if (!subResults && parsee.getIsRequired() && !parsee.getShouldFailOnNoMatch()) {
            currentIndex = startIndex;
            return ParseeResultsGroup::noMatch();
        }

        // should return a matching result?
        if (subResults && parsee.getShouldReturn()) {
            for (ParseeResult &subResult : (*subResults).first)
                parseeResults.push_back(subResult);
        }

        // invalid sequence detected?
        if (!subResults && parsee.getShouldFailOnNoMatch()) {
            markError({}, parsee, {});
            return ParseeResultsGroup::failure();
        }

        // got to the next token if we got a match
        if (subResults)
            currentIndex += (*subResults).second;
    }

    return ParseeResultsGroup::success(parseeResults);
}

optional<pair<vector<ParseeResult>, int>> Parser::groupParseeResults(ParseeGroup group) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    
    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(group);
    if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE)
        return {};

    for (ParseeResult &result : resultsGroup.getResults())
        results.push_back(result);
    
    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(results, tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::repeatedGroupParseeResults(ParseeGroup group) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    
    ParseeResultsGroup resultsGroup;
    do {
        resultsGroup = parseeResultsGroupForParseeGroup(group);
        if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE)
            return {};

        for (ParseeResult &result : resultsGroup.getResults())
            results.push_back(result);
    } while (resultsGroup.getKind() == ParseeResultsGroupKind::SUCCESS);
    
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
    int startIndex = index;

    if (!tokens.at(index)->isOfKind({TokenKind::TYPE, TokenKind::BLOB}))
        return {};

    shared_ptr<Token> typeToken = tokens.at(index++);
    shared_ptr<ValueType> subType;
    uint64_t typeValueArg = 0;
    string typeName;

    if (tokens.at(index)->isOfKind({TokenKind::LESS})) {
        index++;

        // type name (in blob)
        if (tokens.at(index)->isOfKind({TokenKind::IDENTIFIER})) {
            typeName = tokens.at(index++)->getLexme();
        // subtype, count (data, ptr)
        } else {
            optional<pair<vector<ParseeResult>, int>> subResults = valueTypeParseeResults(index, tag);
            if (!subResults || (*subResults).first.empty())
                return {};
            subType = (*subResults).first[0].getValueType();
            index += (*subResults).second;

            if (tokens.at(index)->isOfKind({TokenKind::COMMA})) {
                index++;

                if (!tokens.at(index)->isOfKind({TokenKind::INTEGER_DEC, TokenKind::INTEGER_HEX, TokenKind::INTEGER_BIN, TokenKind::INTEGER_CHAR}))
                    return {};

                int storedIndex = currentIndex;
                currentIndex = index;
                shared_ptr<Expression> expressionValue = matchExpressionLiteral();
                typeValueArg = dynamic_pointer_cast<ExpressionLiteral>(expressionValue)->getUIntValue();
                currentIndex = storedIndex;
                index++;
            }
        }

        if (!tokens.at(index)->isOfKind({TokenKind::GREATER}))
            return {};
        index++;
    }

    shared_ptr<ValueType> valueType = ValueType::valueTypeForToken(typeToken, subType, typeValueArg, typeName);
    if (valueType == nullptr)
        return {};
    return pair(vector<ParseeResult>({ParseeResult::valueTypeResult(valueType, index - startIndex, tag)}), index - startIndex);
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

optional<pair<vector<ParseeResult>, int>> Parser::expressionParseeResults(int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression = nextExpression();
    if (errors.size() > errorsCount || expression == nullptr)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount, tag)}), tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::orParseeResults(ParseeGroup first, ParseeGroup second) {
    int startIndex = currentIndex;
    vector<ParseeResult> results;
    ParseeResultsGroup resultsGroup;

    // try matching first or second group
    resultsGroup = parseeResultsGroupForParseeGroup(first);
    if (resultsGroup.getKind() == ParseeResultsGroupKind::FAILURE) {
        return {};
    } else if (resultsGroup.getKind() == ParseeResultsGroupKind::NO_MATCH) {
        currentIndex = startIndex;
        resultsGroup = parseeResultsGroupForParseeGroup(second);
    }

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return {};

    for (ParseeResult &result : resultsGroup.getResults())
        results.push_back(result);

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(results, tokensCount);
}

optional<pair<vector<ParseeResult>, int>> Parser::statementBlockParseeResults(bool isMultiline, int tag) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Statement> statement;
    if (isMultiline)
        statement = matchStatementBlock({TokenKind::SEMICOLON});
    else
        statement = matchStatementBlock({TokenKind::NEW_LINE, TokenKind::COMMA});

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
