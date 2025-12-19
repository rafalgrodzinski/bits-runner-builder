#include "ExpressionCompositeLiteral.h"

#include "Lexer/Location.h"
#include "Lexer/Token.h"
#include "Parser/Expression/ExpressionLiteral.h"

shared_ptr<ExpressionCompositeLiteral> ExpressionCompositeLiteral::expressionCompositeLiteralForExpressions(vector<shared_ptr<Expression>> expressions, shared_ptr<Location> location) {
    shared_ptr<ExpressionCompositeLiteral> expression = make_shared<ExpressionCompositeLiteral>(location);
    expression->expressions = expressions;
    return expression;
}

shared_ptr<ExpressionCompositeLiteral> ExpressionCompositeLiteral::expressionCompositeLiteralForTokenString(shared_ptr<Token> tokenString) {
    if (tokenString->getKind() != TokenKind::STRING)
        return nullptr;

    shared_ptr<ExpressionCompositeLiteral> expression = make_shared<ExpressionCompositeLiteral>(tokenString->getLocation());

    vector<shared_ptr<Expression>> expressions;
    string stringValue = tokenString->getLexme();
    for (int i=1; i<stringValue.length()-1; i++) {
        string lexme = stringValue.substr(i, 1);
        if (stringValue[i] == '\\') {
            lexme = stringValue.substr(i, 2);
            i++;
        }
        shared_ptr<Token> token = make_shared<Token>(
            TokenKind::INTEGER_CHAR,
            lexme,
            make_shared<Location>(
                tokenString->getLocation()->getFileName(), 
                tokenString->getLocation()->getLine(), 
                tokenString->getLocation()->getColumn() + i
            )
        );
        shared_ptr<ExpressionLiteral> expression = ExpressionLiteral::expressionLiteralForToken(token);
        expressions.push_back(expression);

        // add terminal 0 if missing
        if (i == stringValue.length() - 2 && lexme.compare("\\0") != 0) {
            shared_ptr<Token> token = make_shared<Token>(
                TokenKind::INTEGER_CHAR, "\\0",
                make_shared<Location>(
                    tokenString->getLocation()->getFileName(), 
                    tokenString->getLocation()->getLine(), 
                    tokenString->getLocation()->getColumn() + i + lexme.length()
                )
            );
            shared_ptr<ExpressionLiteral> expression = ExpressionLiteral::expressionLiteralForToken(token);
            expressions.push_back(expression);
        }
    }

    expression->expressions = expressions;
    return expression;
}

ExpressionCompositeLiteral::ExpressionCompositeLiteral(shared_ptr<Location> location):
Expression(ExpressionKind::COMPOSITE_LITERAL, nullptr, location) { }

vector<shared_ptr<Expression>> ExpressionCompositeLiteral::getExpressions() {
    return expressions;
} 