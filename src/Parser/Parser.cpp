#include "Parser.h"

#include "Error.h"
#include "Logger.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionArrayLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
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

Parser::Parser(vector<shared_ptr<Token>> tokens) :
tokens(tokens) { }

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds({TokenKind::END}, true, false)) {
        shared_ptr<Statement> statement = nextStatement();
        if (statement != nullptr) {
            statements.push_back(statement);

            // Expect new line after statement
            if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
                markError(TokenKind::NEW_LINE, {});
        }
    }

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

    return statements;
}

//
// Statement
//
shared_ptr<Statement> Parser::nextStatement() {
    shared_ptr<Statement> statement;
    int errorsCount = errors.size();

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

    markError({}, {});
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

    markError({}, {});
    return nullptr;
}

shared_ptr<Statement> Parser::matchStatementMetaExternFunction() {
    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::M_EXTERN, true, false, false),
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                Parsee::tokenParsee(TokenKind::FUNCTION, true, false, false),
                // arguments
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                            Parsee::valueTypeParsee(true, true, true),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                                        Parsee::valueTypeParsee(true, true, true)
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
                            Parsee::valueTypeParsee(true, true, true)
                        }
                    ), false, true, false
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    int i = 0;
    // identifier
    identifier = resultsGroup.getResults().at(i++).getToken()->getLexme();
    // arguments
    while (i < resultsGroup.getResults().size()-1 && resultsGroup.getResults().at(i).getKind() == ParseeResultKind::TOKEN) {
            pair<string, shared_ptr<ValueType>> argument;
            argument.first = resultsGroup.getResults().at(i++).getToken()->getLexme();
            argument.second = resultsGroup.getResults().at(i++).getValueType();
            arguments.push_back(argument);
    }
    // return type
    if (i < resultsGroup.getResults().size())
        returnType = resultsGroup.getResults().at(i).getValueType();

    return make_shared<StatementMetaExternFunction>(identifier, arguments, returnType);
}

shared_ptr<Statement> Parser::matchStatementVariable() {
    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                Parsee::valueTypeParsee(true, true, false),
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                            Parsee::expressionParsee(true, true, true)
                        }
                    ), false, true, false
                )
            }
        )
    );

    if (resultsGroup.getKind() != ParseeResultsGroupKind::SUCCESS)
        return nullptr;

    string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
    shared_ptr<ValueType> valueType = resultsGroup.getResults().at(1).getValueType();
    shared_ptr<Expression> expression = resultsGroup.getResults().size() > 2 ? resultsGroup.getResults().at(2).getExpression() : nullptr;

    return make_shared<StatementVariable>(identifier, valueType, expression);
}

shared_ptr<Statement> Parser::matchStatementFunction() {
    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    shared_ptr<Statement> statementBlock;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                Parsee::tokenParsee(TokenKind::FUNCTION, true, false, false),
                // arguments
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            // first argument
                            Parsee::tokenParsee(TokenKind::COLON, true, false, false),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                            Parsee::valueTypeParsee(true, true, true),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                                        Parsee::valueTypeParsee(true, true, true)
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
                            Parsee::valueTypeParsee(true, true, true)
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
            int i = 0;
            // identifier
            identifier = resultsGroup.getResults().at(i++).getToken()->getLexme();
            // arguments
            while (i < resultsGroup.getResults().size()-1 && resultsGroup.getResults().at(i).getKind() == ParseeResultKind::TOKEN) {
                    pair<string, shared_ptr<ValueType>> argument;
                    argument.first = resultsGroup.getResults().at(i++).getToken()->getLexme();
                    argument.second = resultsGroup.getResults().at(i++).getValueType();
                    arguments.push_back(argument);
            }
            // return type
            if (i < resultsGroup.getResults().size())
                returnType = resultsGroup.getResults().at(i).getValueType();
        }
            break;
        case ParseeResultsGroupKind::NO_MATCH:
            return nullptr;
        case ParseeResultsGroupKind::FAILURE:
            break;
    }

    // block
    statementBlock = matchStatementBlock({TokenKind::SEMICOLON, TokenKind::END});
    if (statementBlock == nullptr)
        return nullptr;

    // closing semicolon
    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true)) {
        markError(TokenKind::SEMICOLON, {});
        return nullptr;
    }

    return make_shared<StatementFunction>(identifier, arguments, returnType, dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementRawFunction() {
    string identifier;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    string rawSource;

    ParseeResultsGroup resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                Parsee::tokenParsee(TokenKind::RAW_FUNCTION, true, false, false),
                // constraints
                Parsee::groupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::LESS, true, false, true),
                            Parsee::tokenParsee(TokenKind::STRING, true, true, true),
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
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                            Parsee::valueTypeParsee(true, true, true),
                            // additional arguments
                            Parsee::repeatedGroupParsee(
                                ParseeGroup(
                                    {
                                        Parsee::tokenParsee(TokenKind::COMMA, true, false, false),
                                        Parsee::tokenParsee(TokenKind::NEW_LINE, false, false, false),
                                        Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                                        Parsee::valueTypeParsee(true, true, true)
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
                            Parsee::valueTypeParsee(true, true, true)
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
            int i = 0;
            // identifier
            identifier = resultsGroup.getResults().at(i++).getToken()->getLexme();
            // constraints
            constraints = resultsGroup.getResults().at(i++).getToken()->getLexme();
            // remove enclosing quotes
                if (constraints.length() >= 2)
                    constraints = constraints.substr(1, constraints.length() - 2);
            // arguments
            while (i < resultsGroup.getResults().size()-1 && resultsGroup.getResults().at(i).getKind() == ParseeResultKind::TOKEN) {
                    pair<string, shared_ptr<ValueType>> argument;
                    argument.first = resultsGroup.getResults().at(i++).getToken()->getLexme();
                    argument.second = resultsGroup.getResults().at(i++).getValueType();
                    arguments.push_back(argument);
            }
            // return type
            if (i < resultsGroup.getResults().size())
                returnType = resultsGroup.getResults().at(i).getValueType();
        }
            break;
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
        markError(TokenKind::SEMICOLON, {});
        return nullptr;
    }

    return make_shared<StatementRawFunction>(identifier, constraints, arguments, returnType, rawSource);
}

shared_ptr<Statement> Parser::matchStatementBlob() {
    ParseeResultsGroup resultsGroup;

    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> variables;

    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                Parsee::tokenParsee(TokenKind::BLOB, true, false, false),
                Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true),
                Parsee::repeatedGroupParsee(
                    ParseeGroup(
                        {
                            Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                            Parsee::valueTypeParsee(true, true, true),
                            Parsee::tokenParsee(TokenKind::NEW_LINE, true, false, true)
                        }
                    ), false, true, false
                ),
                Parsee::tokenParsee(TokenKind::SEMICOLON, true, false, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS:
            identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            for (int i=1; i<resultsGroup.getResults().size(); i+=2) {
                pair<string, shared_ptr<ValueType>> arg;
                arg.first = resultsGroup.getResults().at(i).getToken()->getLexme();
                arg.second = resultsGroup.getResults().at(i+1).getValueType();
                variables.push_back(arg);
            }
           break;
        case ParseeResultsGroupKind::NO_MATCH:
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
            break;
    }

    return make_shared<StatementBlob>(identifier, variables);
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
            markError(TokenKind::NEW_LINE, {});
    }

    return make_shared<StatementBlock>(statements);
}

shared_ptr<Statement> Parser::matchStatementAssignment() {
    ParseeResultsGroup resultsGroup;

    // simple
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // expression
                Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                Parsee::expressionParsee(true, true, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            shared_ptr<Expression> valueExpression = resultsGroup.getResults().at(1).getExpression();
            return StatementAssignment::simple(identifier, valueExpression);
        }
        case ParseeResultsGroupKind::NO_MATCH:
            break;
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
    }

    // data
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // index expression
                Parsee::tokenParsee(TokenKind::LEFT_SQUARE_BRACKET, true, false, false),
                Parsee::expressionParsee(true, true, true),
                Parsee::tokenParsee(TokenKind::RIGHT_SQUARE_BRACKET, true, false, true),
                // expression
                Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                Parsee::expressionParsee(true, true, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            shared_ptr<Expression> indexExpression = resultsGroup.getResults().at(1).getExpression();
            shared_ptr<Expression> valueExpression = resultsGroup.getResults().at(2).getExpression();
            return StatementAssignment::data(identifier, indexExpression, valueExpression);
        }
        case ParseeResultsGroupKind::NO_MATCH:
            break;
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
    }

    // blob
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // member name
                Parsee::tokenParsee(TokenKind::DOT, true, false, false),
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true),
                // expression
                Parsee::tokenParsee(TokenKind::LEFT_ARROW, true, false, false),
                Parsee::expressionParsee(true, true, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            string memberName = resultsGroup.getResults().at(1).getToken()->getLexme();
            shared_ptr<Expression> valueExpression = resultsGroup.getResults().at(2).getExpression();
            return StatementAssignment::blob(identifier, memberName, valueExpression);
        }
        case ParseeResultsGroupKind::NO_MATCH:
            break;
        case ParseeResultsGroupKind::FAILURE:
            return nullptr;
    }

    return nullptr;
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
    if (!tryMatchingTokenKinds({TokenKind::REPEAT}, true, true))
        return nullptr;

    shared_ptr<Statement> initStatement;
    shared_ptr<Expression> preConditionExpression;
    shared_ptr<Expression> postConditionExpression;
    shared_ptr<Statement> bodyBlockStatement;

    bool isMultiLine;

    // initial
    initStatement = matchStatementVariable() ?: matchStatementAssignment();

    if (!tryMatchingTokenKinds({TokenKind::COLON}, false, true)) {
        // got initial, expect comma
        if (initStatement != nullptr && !tryMatchingTokenKinds({TokenKind::COMMA}, true, true)) {
            markError(TokenKind::COMMA, {});
            goto afterIf;
        }

        // optional new line
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

        // pre condition
        preConditionExpression = nextExpression();

        if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
            // got pre-condition, expect comma
            if (!tryMatchingTokenKinds({TokenKind::COMMA}, true, true)) {
                markError(TokenKind::COMMA, {});
                goto afterIf;
            }

            // optional new line
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

            // post condition
            postConditionExpression = nextExpression();
            
            // expect colon
            if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
                markError(TokenKind::COLON, {});
                goto afterIf;
            }
        }
    }
    afterIf:

    isMultiLine = tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

    // body
    if (isMultiLine)
        bodyBlockStatement = matchStatementBlock({TokenKind::SEMICOLON});
    else
        bodyBlockStatement = matchStatementBlock({TokenKind::NEW_LINE});

    if (bodyBlockStatement == nullptr)
        return nullptr;
    
    tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true);

    return make_shared<StatementRepeat>(initStatement, preConditionExpression, postConditionExpression, dynamic_pointer_cast<StatementBlock>(bodyBlockStatement));
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

    expression = matchEquality();
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
        shared_ptr<Expression> expression = matchPrimary();
        if (expression == nullptr)
            return nullptr;
        return make_shared<ExpressionUnary>(token, expression);
    }

    return matchPrimary();
}

shared_ptr<Expression> Parser::matchPrimary() {
    shared_ptr<Expression> expression;
    int errorsCount = errors.size();

    expression = matchExpressionGrouping();
    if (expression != nullptr || errors.size() > errorsCount)
        return expression;

    expression = matchExpressionArrayLiteral();
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
    if (tryMatchingTokenKinds({TokenKind::LEFT_PAREN}, true, true)) {
        shared_ptr<Expression> expression = matchTerm();
        // has grouped expression failed?
        if (expression == nullptr) {
            return nullptr;
        } else if (tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true)) {
            return make_shared<ExpressionGrouping>(expression);
        } else {
            markError(TokenKind::RIGHT_PAREN, {});
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

shared_ptr<Expression> Parser::matchExpressionArrayLiteral() {
    if (tryMatchingTokenKinds({TokenKind::STRING}, true, false)) {
        return ExpressionArrayLiteral::expressionArrayLiteralForTokenString(tokens.at(currentIndex++));
    } else if (tryMatchingTokenKinds({TokenKind::LEFT_SQUARE_BRACKET}, true, true)) {
        vector<shared_ptr<Expression>> expressions;
        if (!tryMatchingTokenKinds({TokenKind::RIGHT_SQUARE_BRACKET}, true, true)) {
            do {
                shared_ptr<Expression> expression = nextExpression();
                if (expression != nullptr)
                    expressions.push_back(expression);
            } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));

            if (!tryMatchingTokenKinds({TokenKind::RIGHT_SQUARE_BRACKET}, true, true)) {
                markError(TokenKind::RIGHT_SQUARE_BRACKET, {});
                return nullptr;
            }
        }

        return ExpressionArrayLiteral::expressionArrayLiteralForExpressions(expressions);
    }

    return nullptr;
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

    // blob
    resultsGroup = parseeResultsGroupForParseeGroup(
        ParseeGroup(
            {
                // identifier
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, false),
                // member name
                Parsee::tokenParsee(TokenKind::DOT, true, false, false),
                Parsee::tokenParsee(TokenKind::IDENTIFIER, true, true, true)
            }
        )
    );

    switch (resultsGroup.getKind()) {
        case ParseeResultsGroupKind::SUCCESS: {
            string identifier = resultsGroup.getResults().at(0).getToken()->getLexme();
            string memberName = resultsGroup.getResults().at(1).getToken()->getLexme();
            return ExpressionVariable::blob(identifier, memberName);
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
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::LEFT_PAREN}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex);
    currentIndex++; // identifier
    currentIndex++; // left parenthesis

    vector<shared_ptr<Expression>> argumentExpressions;
    if (!tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // optional new line

            shared_ptr<Expression> argumentExpression = nextExpression();
            if (argumentExpression == nullptr)
                return nullptr;
            argumentExpressions.push_back(argumentExpression);
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));

        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // optional new line
        if (!tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true)) {
            markError(TokenKind::RIGHT_PAREN, {});
            return nullptr;
        }
    }

    return make_shared<ExpressionCall>(identifierToken->getLexme(), argumentExpressions);
}

shared_ptr<Expression> Parser::matchExpressionIfElse() {
    if (!tryMatchingTokenKinds({TokenKind::IF}, true, true))
        return nullptr;

    shared_ptr<Expression> condition;
    shared_ptr<Expression> thenBlock;
    shared_ptr<Expression> elseBlock;

    // condition expression
    condition = nextExpression();
    if (condition == nullptr)
        return nullptr;
    
    if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        markError(TokenKind::COLON, {});
        return nullptr;
    }
    
    // then
    bool isMultiLine = tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

    // then block
    if (isMultiLine)
        thenBlock = matchExpressionBlock({TokenKind::ELSE, TokenKind::SEMICOLON});
    else
        thenBlock = matchExpressionBlock({TokenKind::ELSE, TokenKind::NEW_LINE});

    if (thenBlock == nullptr)
        return nullptr;

    // else
    if (tryMatchingTokenKinds({TokenKind::ELSE}, true, true)) {
        isMultiLine = (tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true));

        // else block
        if (isMultiLine)
            elseBlock = matchExpressionBlock({TokenKind::SEMICOLON});
        else
            elseBlock = matchExpressionBlock({TokenKind::NEW_LINE});

        if (elseBlock == nullptr)
            return nullptr;
    }
    tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true);

    return make_shared<ExpressionIfElse>(condition, dynamic_pointer_cast<ExpressionBlock>(thenBlock), dynamic_pointer_cast<ExpressionBlock>(elseBlock));
}

shared_ptr<Expression> Parser::matchExpressionBinary(shared_ptr<Expression> left) {
    shared_ptr<Token> token = tokens.at(currentIndex);
    shared_ptr<Expression> right;
    // What level of binary expression are we having?
    if (tryMatchingTokenKinds(Token::tokensEquality, false, true)) {
        right = matchComparison();
    } else if (tryMatchingTokenKinds(Token::tokensComparison, false, true)) {
        right = matchTerm();
    } else if (tryMatchingTokenKinds(Token::tokensTerm, false, true)) {
        right = matchFactor();
    } else if (tryMatchingTokenKinds(Token::tokensFactor, false, true)) {
        right = matchPrimary();
    }

    if (right == nullptr) {
        markError({}, "Expected expression");
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
            markError(TokenKind::NEW_LINE, {});
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
                subResults = tokenParseeResults(currentIndex, parsee.getTokenKind());
                break;
            case ParseeKind::VALUE_TYPE:
                subResults = valueTypeParseeResults(currentIndex);
                break;
            case ParseeKind::EXPRESSION:
                subResults = expressionParseeResults(currentIndex);
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
            markError(parsee.getTokenKind(), {});
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

optional<pair<vector<ParseeResult>, int>> Parser::tokenParseeResults(int index, TokenKind tokenKind) {
    shared_ptr<Token> token = tokens.at(index);
    if (token->isOfKind({tokenKind}))
        return pair(vector<ParseeResult>({ParseeResult::tokenResult(token)}), 1);
    return {};
}

optional<pair<vector<ParseeResult>, int>> Parser::valueTypeParseeResults(int index) {
    int startIndex = index;

    if (!tokens.at(index)->isOfKind({TokenKind::TYPE}))
        return {};

    shared_ptr<Token> typeToken = tokens.at(index++);
    shared_ptr<ValueType> subType;
    int typeArg = 0;

    if (tokens.at(index)->isOfKind({TokenKind::LESS})) {
        index++;
        optional<pair<vector<ParseeResult>, int>> subResults = valueTypeParseeResults(index);
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
            typeArg = dynamic_pointer_cast<ExpressionLiteral>(expressionValue)->getU32Value();
            currentIndex = storedIndex;
            index++;
        }

        if (!tokens.at(index)->isOfKind({TokenKind::GREATER}))
            return {};
        index++;
    }

    shared_ptr<ValueType> valueType = ValueType::valueTypeForToken(typeToken, subType, typeArg);
    return pair(vector<ParseeResult>({ParseeResult::valueTypeResult(valueType, index - startIndex)}), index - startIndex);
}

optional<pair<vector<ParseeResult>, int>> Parser::expressionParseeResults(int index) {
    int startIndex = currentIndex;
    int errorsCount = errors.size();
    shared_ptr<Expression> expression = nextExpression();
    if (errors.size() > errorsCount)
        return {};

    int tokensCount = currentIndex - startIndex;
    currentIndex = startIndex;
    return pair(vector<ParseeResult>({ParseeResult::expressionResult(expression, tokensCount)}), tokensCount);
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

void Parser::markError(optional<TokenKind> expectedTokenKind, optional<string> message) {
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

    errors.push_back(Error::parserError(actualToken, expectedTokenKind, message));
}
