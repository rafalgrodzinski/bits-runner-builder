#include "Parser.h"

#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionInvalid.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementInvalid.h"

Parser::Parser(vector<shared_ptr<Token>> tokens): tokens(tokens) {
}

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds({TokenKind::END}, true, false)) {
        shared_ptr<Statement> statement = nextStatement();
        // Abort parsing if we got an error
        if (!statement->isValid()) {
            //cerr << statement->toString(0);
            exit(1);
        }
        statements.push_back(statement);

        // Expect new line after statement
        if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true)) {
            cerr << "Expected new line" << endl;
            exit(1);
        }
    }

    return statements;
}

//
// Statement
//
shared_ptr<Statement> Parser::nextStatement() {
   shared_ptr<Statement> statement;

    statement = matchStatementFunction();
    if (statement != nullptr)
        return statement;

    statement = matchStatementVariable();
    if (statement != nullptr)
        return statement;

    statement = matchStatementMetaExternFunction();
    if (statement != nullptr)
        return statement;

    return matchStatementInvalid("Unexpected token");
}

shared_ptr<Statement> Parser::nextInBlockStatement() {
    shared_ptr<Statement> statement;

    statement = matchStatementVariable();
    if (statement != nullptr)
        return statement;

    statement = matchStatementAssignment();
    if (statement != nullptr)
        return statement;

    statement = matchStatementReturn();
    if (statement != nullptr)
        return statement;
    
    statement = matchStatementRepeat();
    if (statement != nullptr)
        return statement;

    statement = matchStatementExpression();
    if (statement != nullptr)
        return statement;

    return matchStatementInvalid("Unexpected token");
}

shared_ptr<Statement> Parser::matchStatementMetaExternFunction() {
    if (!tryMatchingTokenKinds({TokenKind::M_EXTERN, TokenKind::IDENTIFIER, TokenKind::FUNCTION}, true, false))
        return nullptr;

    string name;
    vector<pair<string, ValueType>> arguments;
    ValueType returnType = ValueType::NONE;

    currentIndex++; // skip meta
    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    currentIndex++; // skip fun

    // arguments
    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line
            if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false))
                return matchStatementInvalid("Expected function argument");
            shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
            shared_ptr<Token> typeToken = tokens.at(currentIndex++);
            optional<ValueType> argumentType = valueTypeForToken(typeToken);
            if (!argumentType)
                return matchStatementInvalid("Invalid argument type");
            
            arguments.push_back(pair<string, ValueType>(identifierToken->getLexme(), *argumentType));
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));
    }

    // Return type
    if (tryMatchingTokenKinds({TokenKind::RIGHT_ARROW}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line

        shared_ptr<Token> typeToken = tokens.at(currentIndex);
        optional<ValueType> type = valueTypeForToken(typeToken);
        if (!type)
            return matchStatementInvalid("Expected return type");
        returnType = *type;

        currentIndex++; // type
    }

    return make_shared<StatementMetaExternFunction>(identifierToken->getLexme(), arguments, returnType);
}

shared_ptr<Statement> Parser::matchStatementVariable() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    shared_ptr<Token> valueTypeToken = tokens.at(currentIndex);

    ValueType valueType;
    if (valueTypeToken->getLexme().compare("bool") == 0)
        valueType = ValueType::BOOL; 
    else if (valueTypeToken->getLexme().compare("sint32") == 0)
        valueType = ValueType::SINT32;
    else if (valueTypeToken->getLexme().compare("real32") == 0)
        valueType = ValueType::REAL32;
    else
        return matchStatementInvalid("Invalid type");

    currentIndex++; // type

    // Expect left arrow
    if (!tryMatchingTokenKinds({TokenKind::LEFT_ARROW}, true, true))
        return matchStatementInvalid("Expected left arrow");

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr || !expression->isValid())
        return matchStatementInvalid("Invalid expression");

    return make_shared<StatementVariable>(identifierToken->getLexme(), valueType, expression);
}

shared_ptr<Statement> Parser::matchStatementFunction() {
     if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::FUNCTION}, true, false))
        return nullptr;

    string name;
    vector<pair<string, ValueType>> arguments;
    ValueType returnType = ValueType::NONE;
    shared_ptr<Statement> statementBlock;

    // name
    name = tokens.at(currentIndex++)->getLexme();
    currentIndex++; // skip fun

    // arguments
    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line
            if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false))
                return matchStatementInvalid("Expected function argument");
            shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
            shared_ptr<Token> typeToken = tokens.at(currentIndex++);
            optional<ValueType> argumentType = valueTypeForToken(typeToken);
            if (!argumentType)
                return matchStatementInvalid("Invalid argument type");
            
            arguments.push_back(pair<string, ValueType>(identifierToken->getLexme(), *argumentType));
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));
    }

    // return type
    if (tryMatchingTokenKinds({TokenKind::RIGHT_ARROW}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line

        shared_ptr<Token> typeToken = tokens.at(currentIndex);
        optional<ValueType> type = valueTypeForToken(typeToken);
        if (!type)
            return matchStatementInvalid("Expected return type");
        returnType = *type;

        currentIndex++; // type
    }

    // consume new line
    if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
        return matchStatementInvalid("Expected new line after function declaration");

    // block
    statementBlock = matchStatementBlock({TokenKind::SEMICOLON});
    if (statementBlock == nullptr || !statementBlock->isValid())
        return statementBlock ?: matchStatementInvalid();

    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true))
        return matchStatementInvalid("Expected a \";\" after a function declaration");

    return make_shared<StatementFunction>(name, arguments, returnType, dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementBlock(vector<TokenKind> terminalTokenKinds) {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds(terminalTokenKinds, false, false)) {
        shared_ptr<Statement> statement = nextInBlockStatement();
        if (statement == nullptr || !statement->isValid())
            return statement ?: matchStatementInvalid("Expected statement");
        statements.push_back(statement);

        if (tryMatchingTokenKinds(terminalTokenKinds, false, false))
            break;

        // except new line
        if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
            return matchStatementInvalid("Expected new line");
    }

    return make_shared<StatementBlock>(statements);
}

shared_ptr<Statement> Parser::matchStatementAssignment() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::LEFT_ARROW}, true, false))
        return nullptr;
     
    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    currentIndex++; // arrow

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr || !expression->isValid())
        return matchStatementInvalid("Expected expression");

    return make_shared<StatementAssignment>(identifierToken->getLexme(), expression);
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    if (!tryMatchingTokenKinds({TokenKind::RETURN}, true, true))
        return nullptr;

    shared_ptr<Expression> expression = nextExpression();
    if (expression != nullptr && !expression->isValid())
        return matchStatementInvalid("Expected expression");
    
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
    if (initStatement != nullptr && !initStatement->isValid())
        initStatement = nullptr;

    if (!tryMatchingTokenKinds({TokenKind::COLON}, false, true)) {
        // got initial, expect comma
        if (initStatement != nullptr && !tryMatchingTokenKinds({TokenKind::COMMA}, true, true))
            return matchStatementInvalid("Expected comma after initial statement");

        // optional new line
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

        // pre condition
        preConditionExpression = nextExpression();
        if (preConditionExpression != nullptr && !preConditionExpression->isValid())
            return matchStatementInvalid("Expected pre-condition expression");

        if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
            // got pre-condition, expect comma
            if (!tryMatchingTokenKinds({TokenKind::COMMA}, true, true))
                return matchStatementInvalid("Expected comma after pre-condition statement");

            // optional new line
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

            // post condition
            postConditionExpression = nextExpression();
            if (postConditionExpression == nullptr || !postConditionExpression->isValid())
                return matchStatementInvalid("Expected post-condition expression");
            
            // expect colon
            if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true))
                return matchStatementInvalid("Expected \":\"");
        }
    }

    isMultiLine = tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

    // body
    if (isMultiLine)
        bodyBlockStatement = matchStatementBlock({TokenKind::SEMICOLON});
    else
        bodyBlockStatement = matchStatementBlock({TokenKind::NEW_LINE});

    if (bodyBlockStatement == nullptr || !bodyBlockStatement->isValid())
        return bodyBlockStatement ?: matchStatementInvalid("Expected block statement");
    
    tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true);

    return make_shared<StatementRepeat>(initStatement, preConditionExpression, postConditionExpression, dynamic_pointer_cast<StatementBlock>(bodyBlockStatement));
}

shared_ptr<Statement> Parser::matchStatementExpression() {
    shared_ptr<Expression> expression = nextExpression();

    if (expression == nullptr)
        return nullptr;
    else if (!expression->isValid())
        return make_shared<StatementInvalid>(tokens.at(currentIndex), expression->toString(0));

    return make_shared<StatementExpression>(expression);
}

shared_ptr<StatementInvalid> Parser::matchStatementInvalid(string message) {
    return make_shared<StatementInvalid>(tokens.at(currentIndex), message);
}

//
// Expression
//
shared_ptr<Expression> Parser::nextExpression() {
    shared_ptr<Expression> expression;

    expression = matchEquality();
    if (expression != nullptr)
        return expression;
    
    expression = matchExpressionIfElse();
    if (expression != nullptr)
        return expression;
    
    expression = matchExpressionVariable();
    if (expression != nullptr)
        return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchEquality() {
    shared_ptr<Expression> expression = matchComparison();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tryMatchingTokenKinds({Token::tokensEquality}, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchComparison() {
    shared_ptr<Expression> expression = matchTerm();
    if (expression == nullptr || !expression->isValid())
        return expression;
    
    while (tryMatchingTokenKinds({Token::tokensComparison}, false, false))
        expression = matchExpressionBinary(expression);
    
    return expression;
}

shared_ptr<Expression> Parser::matchTerm() {
    shared_ptr<Expression> expression = matchFactor();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tryMatchingTokenKinds({Token::tokensTerm}, false, false))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchFactor() {
    shared_ptr<Expression> expression = matchPrimary();
    if (expression == nullptr || !expression->isValid())
        return expression;

    while (tokens.at(currentIndex)->isOfKind(Token::tokensFactor))
        expression = matchExpressionBinary(expression);

    return expression;
}

shared_ptr<Expression> Parser::matchPrimary() {
    shared_ptr<Expression> expression;

    expression = matchExpressionGrouping();
    if (expression != nullptr)
        return expression;

    expression = matchExpressionLiteral();
    if (expression != nullptr)
        return expression;

    expression = matchExpressionCall();
    if (expression != nullptr)
        return expression;

    expression = matchExpressionVariable();
    if (expression != nullptr)
    return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionGrouping() {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (tryMatchingTokenKinds({TokenKind::LEFT_PAREN}, true, true)) {
        shared_ptr<Expression> expression = matchTerm();
        // has grouped expression failed?
        if (expression == nullptr || !expression->isValid()) {
            return expression ?: matchExpressionInvalid("Expected expression");
        } else if (tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true)) {
            return make_shared<ExpressionGrouping>(expression);
        } else {
            return matchExpressionInvalid("Unexpected token");
        }
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionLiteral() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLiteral, false, true))
        return make_shared<ExpressionLiteral>(token);

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionVariable() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds({TokenKind::IDENTIFIER}, true, true))
        return make_shared<ExpressionVariable>(token->getLexme());
    
    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionCall() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::LEFT_PAREN}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex);
    currentIndex++; // identifier
    currentIndex++; // left parenthesis

    vector<shared_ptr<Expression>> argumentExpressions;
    do {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // optional new line

        shared_ptr<Expression> argumentExpression = nextExpression();
        if (argumentExpression == nullptr || !argumentExpression->isValid())
            return argumentExpression;
        argumentExpressions.push_back(argumentExpression);
    } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));

    tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // optional new line
    if (!tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true))
        return matchExpressionInvalid("Expected \")\"");

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
    if (condition == nullptr || !condition->isValid())
        return condition ?: matchExpressionInvalid("Expected condition expression");
    
    if (!tryMatchingTokenKinds({TokenKind::COLON}, true, true))
        return matchExpressionInvalid("Expected \":\"");
    
    // then
    bool isMultiLine = false;

    if (tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
        isMultiLine = true;

    // then block
    if (isMultiLine)
        thenBlock = matchExpressionBlock({TokenKind::ELSE, TokenKind::SEMICOLON});
    else
        thenBlock = matchExpressionBlock({TokenKind::ELSE, TokenKind::NEW_LINE});
    if (thenBlock == nullptr || !thenBlock->isValid())
        return thenBlock ?: matchExpressionInvalid("Expected then block");

    // else
    if (tryMatchingTokenKinds({TokenKind::ELSE}, true, true)) {
        if (tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
            isMultiLine = true;

        // else block
        if (isMultiLine)
            elseBlock = matchExpressionBlock({TokenKind::SEMICOLON});
        else
            elseBlock = matchExpressionBlock({TokenKind::NEW_LINE});

        if (elseBlock == nullptr || !elseBlock->isValid())
            return elseBlock ?: matchExpressionInvalid("Expected else block");
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
        return matchExpressionInvalid("Expected right-side expression");
    } else if (!right->isValid()) {
        return right;
    } else {
        return make_shared<ExpressionBinary>(token, left, right);
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionBlock(vector<TokenKind> terminalTokenKinds) {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds(terminalTokenKinds, false, false)) {
        shared_ptr<Statement> statement = nextInBlockStatement();
        if (statement == nullptr || !statement->isValid())
            return matchExpressionInvalid("Expected statement");
        statements.push_back(statement);

        if (tryMatchingTokenKinds(terminalTokenKinds, false, false))
            break;

        // except new line
        if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
            return matchExpressionInvalid("Expected new line");
    }

    return make_shared<ExpressionBlock>(statements);
}

shared_ptr<ExpressionInvalid> Parser::matchExpressionInvalid(string message) {
    return make_shared<ExpressionInvalid>(tokens.at(currentIndex));
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

optional<ValueType> Parser::valueTypeForToken(shared_ptr<Token> token) {
    if (token->getKind() != TokenKind::TYPE)
        return {};
    
    if (token->getLexme().compare("bool") == 0)
        return ValueType::BOOL; 
    else if (token->getLexme().compare("sint32") == 0)
        return ValueType::SINT32;
    else if (token->getLexme().compare("real32") == 0)
        return ValueType::REAL32;

    return {};
}
