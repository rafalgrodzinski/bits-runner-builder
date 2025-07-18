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
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementRepeat.h"

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
    if (!tryMatchingTokenKinds({TokenKind::M_EXTERN, TokenKind::IDENTIFIER, TokenKind::FUNCTION}, true, false))
        return nullptr;

    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;

    currentIndex++; // skip meta
    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    currentIndex++; // skip fun

    // arguments
    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line
            if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false)) {
                markError({}, "Expected function argument");
                return nullptr;
            }
            shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
            //shared_ptr<Token> argumentTypeToken = tokens.at(currentIndex++);
            shared_ptr<ValueType> argumentType = matchValueType();
            if (argumentType == nullptr) {
                markError(TokenKind::TYPE, {});
                return nullptr;
            }
            
            arguments.push_back(pair<string, shared_ptr<ValueType>>(identifierToken->getLexme(), argumentType));
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));
    }

    // Return type
    if (tryMatchingTokenKinds({TokenKind::RIGHT_ARROW}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line

        //shared_ptr<Token> returnTypeToken = tokens.at(currentIndex);
        returnType = matchValueType();
        if (returnType == nullptr) {
            markError(TokenKind::TYPE, {});
            return nullptr;
        }
    }

    return make_shared<StatementMetaExternFunction>(identifierToken->getLexme(), arguments, returnType);
}

shared_ptr<Statement> Parser::matchStatementVariable() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    shared_ptr<ValueType> valueType = matchValueType();

    // Expect left arrow
    if (!tryMatchingTokenKinds({TokenKind::LEFT_ARROW}, true, true)) {
        markError(TokenKind::LEFT_ARROW, {});
        return nullptr;
    }

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr)
        return nullptr;

    return make_shared<StatementVariable>(identifierToken->getLexme(), valueType, expression);
}

shared_ptr<Statement> Parser::matchStatementFunction() {
     if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::FUNCTION}, true, false))
        return nullptr;

    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    shared_ptr<Statement> statementBlock;

    // name
    name = tokens.at(currentIndex++)->getLexme();
    currentIndex++; // skip fun

    // arguments
    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line
            if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false)) {
                markError({}, "Expected function argument");
                return nullptr;
            }
            shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
            shared_ptr<ValueType> argumentType = matchValueType();
            if (argumentType == nullptr) {
                markError(TokenKind::TYPE, {});
                return nullptr;
            }
            
            arguments.push_back(pair<string, shared_ptr<ValueType>>(identifierToken->getLexme(), argumentType));
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));
    }

    // return type
    if (tryMatchingTokenKinds({TokenKind::RIGHT_ARROW}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line

        returnType = matchValueType();
        if (returnType == nullptr) {
            markError(TokenKind::TYPE, {});
            return nullptr;
        }
    }

    // consume new line
    if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true)) {
        markError(TokenKind::NEW_LINE, {});
        return nullptr;
    }

    // block
    statementBlock = matchStatementBlock({TokenKind::SEMICOLON});
    if (statementBlock == nullptr)
        return nullptr;

    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true)) {
        markError(TokenKind::SEMICOLON, {});
        return nullptr;
    }

    return make_shared<StatementFunction>(name, arguments, returnType, dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementRawFunction() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::RAW_FUNCTION}, true, false))
        return nullptr;

    string name;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnType = ValueType::NONE;
    string rawSource;

    // name
    name = tokens.at(currentIndex++)->getLexme();
    currentIndex++; // skip raw

    // constraints

    if (tryMatchingTokenKinds({TokenKind::LESS}, true, true)) {
        if (tokens.at(currentIndex)->isOfKind({TokenKind::STRING})) {
            constraints = tokens.at(currentIndex++)->getLexme();
            // remove enclosing quotes
            if (constraints.length() >= 2)
                constraints = constraints.substr(1, constraints.length() - 2);
        }
        if (!tryMatchingTokenKinds({TokenKind::GREATER}, true, true))
            markError({TokenKind::GREATER}, {});
    }

    // arguments
    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        do {
            tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line
            if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::TYPE}, true, false)) {
                markError({}, "Expected function argument");
                return nullptr;
            }
            shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
            shared_ptr<ValueType> argumentType = matchValueType();
            if (argumentType == nullptr) {
                markError(TokenKind::TYPE, {});
                return nullptr;
            }
            
            arguments.push_back(pair<string, shared_ptr<ValueType>>(identifierToken->getLexme(), argumentType));
        } while (tryMatchingTokenKinds({TokenKind::COMMA}, true, true));
    }

    // return type
    if (tryMatchingTokenKinds({TokenKind::RIGHT_ARROW}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true); // skip new line

        returnType = matchValueType();
        if (returnType == nullptr) {
            markError(TokenKind::TYPE, {});
            return nullptr;
        }
    }

    // consume new line
    if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true)) {
        markError(TokenKind::NEW_LINE, {});
        return nullptr;
    }

    // source
    while (tryMatchingTokenKinds({TokenKind::RAW_SOURCE_LINE}, true, false)) {
        if (!rawSource.empty())
            rawSource += "\n";
        rawSource += tokens.at(currentIndex++)->getLexme();

        // Consume optional new line (for example because of a comment)
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);
    }

    if(!tryMatchingTokenKinds({TokenKind::SEMICOLON}, false, true)) {
        markError(TokenKind::SEMICOLON, {});
        return nullptr;
    }

    return make_shared<StatementRawFunction>(name, constraints, arguments, returnType, rawSource);
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
    int startIndex = currentIndex;

    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER}, true, false))
        return nullptr;
    shared_ptr<Token> identifierToken = tokens.at(currentIndex++);
    shared_ptr<Expression> indexExpression;

    if (tryMatchingTokenKinds({TokenKind::LEFT_SQUARE_BRACKET}, true, true)) {
        indexExpression = nextExpression();
        if (indexExpression == nullptr)
            return nullptr;

        if (!tryMatchingTokenKinds({TokenKind::RIGHT_SQUARE_BRACKET}, true, true)) {
            markError(TokenKind::RIGHT_SQUARE_BRACKET, {});
            return nullptr;
        }
    }

    // assignment requires left arrow, otherwise abort
    if (!tryMatchingTokenKinds({TokenKind::LEFT_ARROW}, true, true)) {
        currentIndex = startIndex;
        return nullptr;
    }

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr)
        return nullptr;

    return make_shared<StatementAssignment>(identifierToken->getLexme(), indexExpression, expression);
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    if (!tryMatchingTokenKinds({TokenKind::RETURN}, true, true))
        return nullptr;

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr)
        return nullptr;
    
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

    markError({}, {});
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
    shared_ptr<Expression> expression = matchPrimary();
    if (expression == nullptr)
        return nullptr;

    if (tokens.at(currentIndex)->isOfKind(Token::tokensFactor))
        expression = matchExpressionBinary(expression);

    return expression;
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
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER}, true, false))
        return nullptr;
    shared_ptr<Token> idToken = tokens.at(currentIndex++);
    shared_ptr<Expression> indexExpression;

    if (tryMatchingTokenKinds({TokenKind::LEFT_SQUARE_BRACKET}, true, true)) {
        indexExpression = nextExpression();
        if (indexExpression == nullptr)
            return nullptr;

        if (!tryMatchingTokenKinds({TokenKind::RIGHT_SQUARE_BRACKET}, true, true)) {
            markError(TokenKind::RIGHT_SQUARE_BRACKET, {});
            return nullptr;
        }
    }
    
    return make_shared<ExpressionVariable>(idToken->getLexme(), indexExpression);
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

shared_ptr<ValueType> Parser::matchValueType() {
    if (!tryMatchingTokenKinds({TokenKind::TYPE}, true, false))
        return nullptr;
    shared_ptr<Token> typeToken = tokens.at(currentIndex++);
    shared_ptr<ValueType> subType;
    int valueArg = 0;

    if (tryMatchingTokenKinds({TokenKind::LESS}, true, true)) {
        if (!tryMatchingTokenKinds({TokenKind::TYPE}, true, false)) {
            markError(TokenKind::TYPE, {});
            return nullptr;
        }
        subType = matchValueType();
        if (subType == nullptr)
            return subType;

        if (tryMatchingTokenKinds({TokenKind::COMMA}, true, true)) {
            if (!tryMatchingTokenKinds({TokenKind::INTEGER_DEC, TokenKind::INTEGER_HEX, TokenKind::INTEGER_BIN, TokenKind::INTEGER_CHAR}, false, false)) {
                markError({}, "Expected integer literal");
                return nullptr;
            }
            shared_ptr<Expression> expressionValue = matchExpressionLiteral();
            if (expressionValue == nullptr) {
                markError({}, "Expected integer literal");
                return nullptr;
            }

            valueArg = dynamic_pointer_cast<ExpressionLiteral>(expressionValue)->getSint32Value();
        }


        if (!tryMatchingTokenKinds({TokenKind::GREATER}, true, true)) {
            markError(TokenKind::GREATER, {});
            return nullptr;
        }
    }

    return ValueType::valueTypeForToken(typeToken, subType, valueArg);
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
