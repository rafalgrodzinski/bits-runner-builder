#include "Parser.h"

Parser::Parser(vector<shared_ptr<Token>> tokens): tokens(tokens) {
}

vector<shared_ptr<Statement>> Parser::getStatements() {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds({TokenKind::END}, true, false)) {
        shared_ptr<Statement> statement = nextStatement();
        // Abort parsing if we got an error
        if (!statement->isValid()) {
            cerr << statement->toString(0);
            exit(1);
        }
        statements.push_back(statement);
    }

    return statements;
}

//
// Statement
//
shared_ptr<Statement> Parser::nextStatement() {
   shared_ptr<Statement> statement;

    statement = matchStatementFunctionDeclaration();
    if (statement != nullptr)
        return statement;

    statement = matchStatementVarDeclaration();
    if (statement != nullptr)
        return statement;

    statement = matchStatementReturn();
    if (statement != nullptr)
        return statement;

    statement = matchStatementExpression();
    if (statement != nullptr)
        return statement;

    return matchStatementInvalid();
}

shared_ptr<Statement> Parser::matchStatementFunctionDeclaration() {
    //if (!matchesTokenKinds({TokenKind::IDENTIFIER, TokenKind::COLON, TokenKind::FUNCTION}))
     if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::COLON, TokenKind::FUNCTION}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex);
    currentIndex++;
    currentIndex++; // skip colon
    currentIndex++; // skip fun

    // FIXME: implement function arguments
    while (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, false))
        currentIndex++;

    currentIndex++; // new line
    shared_ptr<Statement> statementBlock = matchStatementBlock();
    if (statementBlock == nullptr)
        return matchStatementInvalid();
    else if (!statementBlock->isValid())
        return statementBlock;
    else
        return make_shared<StatementFunctionDeclaration>(identifierToken->getLexme(), ValueType::SINT32, dynamic_pointer_cast<StatementBlock>(statementBlock));
}

shared_ptr<Statement> Parser::matchStatementVarDeclaration() {
    if (!tryMatchingTokenKinds({TokenKind::IDENTIFIER, TokenKind::COLON, TokenKind::TYPE}, true, false))
        return nullptr;

    shared_ptr<Token> identifierToken = tokens.at(currentIndex);
    currentIndex++;
    currentIndex++; // skip colon
    shared_ptr<Token> valueTypeToken = tokens.at(currentIndex);
    currentIndex++; // skip fun

    // Expect left arrow
    if (!tryMatchingTokenKinds({TokenKind::LEFT_ARROW}, true, true))
        return matchStatementInvalid();

    shared_ptr<Expression> expression = nextExpression();
    if (expression == nullptr || !expression->isValid())
        return matchStatementInvalid();

    ValueType valueType;
    if (valueTypeToken->getLexme().compare("Void") == 0)
        valueType = ValueType::VOID;
    else if (valueTypeToken->getLexme().compare("Bool") == 0)
        valueType = ValueType::BOOL;
    else if (valueTypeToken->getLexme().compare("SInt32") == 0)
        valueType = ValueType::SINT32;
    else if (valueTypeToken->getLexme().compare("Real32") == 0)
        valueType = ValueType::REAL32;
    else
        return matchStatementInvalid();

    // Expect new line
    if (!tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true))
        return matchStatementInvalid("Expected a new line");

    return make_shared<StatementVarDeclaration>(identifierToken->getLexme(), valueType, expression);
}

shared_ptr<Statement> Parser::matchStatementBlock() {
    vector<shared_ptr<Statement>> statements;

    while (!tryMatchingTokenKinds({TokenKind::SEMICOLON, TokenKind::COLON}, false, false)) {
        shared_ptr<Statement> statement = nextStatement();
        if (statement == nullptr)
            return matchStatementInvalid();
        else if (!statement->isValid())
            return statement;
        else
            statements.push_back(statement);
    }
    // consune ';' only
    if (tryMatchingTokenKinds({TokenKind::SEMICOLON}, true, true)) {
        if (!tryMatchingTokenKinds({TokenKind::NEW_LINE, TokenKind::END}, false, false))
            return matchStatementInvalid();

        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);
    }

    return make_shared<StatementBlock>(statements);
}

shared_ptr<Statement> Parser::matchStatementReturn() {
    if (!tryMatchingTokenKinds({TokenKind::RETURN}, true, true))
        return nullptr;

    shared_ptr<Expression> expression = nextExpression();
    if (expression != nullptr && !expression->isValid())
        return matchStatementInvalid();

    if (!tryMatchingTokenKinds({TokenKind::NEW_LINE, TokenKind::SEMICOLON}, false, false))
        return matchStatementInvalid();
    
    tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);
    
    return make_shared<StatementReturn>(expression);
}

shared_ptr<Statement> Parser::matchStatementExpression() {
    shared_ptr<Expression> expression = nextExpression();

    if (expression == nullptr)
        return nullptr;
    else if (!expression->isValid())
        return make_shared<StatementInvalid>(tokens.at(currentIndex), expression->toString(0));

    // Consume new line
    tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

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
    
    expression = matchExpressionVar();
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

    expression = matchExpressionLiteral();
    if (expression != nullptr)
        return expression;
    
    expression = matchExpressionVar();
    if (expression != nullptr)
        return expression;
    
    expression = matchExpressionGrouping();
    if (expression != nullptr)
        return expression;

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionLiteral() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds(Token::tokensLiteral, false, true))
        return make_shared<ExpressionLiteral>(token);

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionGrouping() {
    shared_ptr<Token> token = tokens.at(currentIndex);
    if (tryMatchingTokenKinds({TokenKind::LEFT_PAREN}, true, true)) {
        shared_ptr<Expression> expression = matchTerm();
        // has grouped expression failed?
        if (expression == nullptr) {
            return matchExpressionInvalid();
        } else if(!expression->isValid()) {
            return expression;
        } else if (tryMatchingTokenKinds({TokenKind::RIGHT_PAREN}, true, true)) {
            return make_shared<ExpressionGrouping>(expression);
        } else {
            return matchExpressionInvalid();
        }
    }

    return nullptr;
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
        return matchExpressionInvalid();
    } else if (!right->isValid()) {
        return right;
    } else {
        return make_shared<ExpressionBinary>(token, left, right);
    }

    return nullptr;
}

shared_ptr<Expression> Parser::matchExpressionIfElse() {
    // Try maching '?'
    shared_ptr<Token> token = tokens.at(currentIndex);

    //if (!tryAdvancingForOneOfTokenKinds({TokenKind::QUESTION}))
    if (!tryMatchingTokenKinds({TokenKind::QUESTION}, true, true))
        return nullptr;

    // Then get condition
    shared_ptr<Expression> condition = nextExpression();
    if (condition == nullptr)
        return matchExpressionInvalid();
    else if (!condition->isValid())
        return condition;
    
    // Match ':', '\n', or ':\n'
    if (tryMatchingTokenKinds({TokenKind::COLON, TokenKind::NEW_LINE}, true, false))
        currentIndex += 2;
    else if (tryMatchingTokenKinds({TokenKind::COLON, TokenKind::NEW_LINE}, false, false))
        currentIndex++;
    else
        return matchExpressionInvalid();

    // Match then block
    shared_ptr<Statement> thenBlock = matchStatementBlock();
    if (thenBlock == nullptr)
        return matchExpressionInvalid();
    else if (!thenBlock->isValid())
        return matchExpressionInvalid(); // FIXME

    // Match else block
    shared_ptr<Statement> elseBlock;

    if (tryMatchingTokenKinds({TokenKind::COLON}, true, true)) {
        tryMatchingTokenKinds({TokenKind::NEW_LINE}, true, true);

        elseBlock = matchStatementBlock();
        if (elseBlock == nullptr)
            return matchExpressionInvalid();
        else if (!elseBlock->isValid())
            return matchExpressionInvalid(); // FIXME

        // hack to treat statement as an expression
        if (tokens.at(currentIndex-1)->getKind() == TokenKind::NEW_LINE)
            currentIndex--;
    }

    return make_shared<ExpressionIfElse>(condition, dynamic_pointer_cast<StatementBlock>(thenBlock), dynamic_pointer_cast<StatementBlock>(elseBlock));
}

shared_ptr<Expression> Parser::matchExpressionVar() {
    shared_ptr<Token> token = tokens.at(currentIndex);

    if (tryMatchingTokenKinds({TokenKind::IDENTIFIER}, true, true))
        return make_shared<ExpressionVar>(token->getLexme());
    
    return nullptr;
}

shared_ptr<ExpressionInvalid> Parser::matchExpressionInvalid() {
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
