#include "ExpressionArrayLiteral.h"

#include "Lexer/Token.h"
#include "Parser/Expression/ExpressionLiteral.h"

ExpressionArrayLiteral::ExpressionArrayLiteral(vector<shared_ptr<Expression>> expressions):
Expression(ExpressionKind::ARRAY_LITERAL, nullptr), expressions(expressions) { }

shared_ptr<ExpressionArrayLiteral> ExpressionArrayLiteral::expressionArrayLiteralForExpressions(vector<shared_ptr<Expression>> expressions) {
    return make_shared<ExpressionArrayLiteral>(expressions);
}

shared_ptr<ExpressionArrayLiteral> ExpressionArrayLiteral::expressionArrayLiteralForTokenString(shared_ptr<Token> tokenString) {
    if (tokenString->getKind() != TokenKind::STRING)
        return nullptr;

    vector<shared_ptr<Expression>> expressions;
    string stringValue = tokenString->getLexme();
    for (int i=1; i<stringValue.length()-1; i++) {
        string lexme = stringValue.substr(i, 1);
        if (stringValue[i] == '\\') {
            lexme = stringValue.substr(i, 2);
            i++;
        }
        shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_CHAR, lexme, tokenString->getLine(), tokenString->getColumn() + i);
        shared_ptr<ExpressionLiteral> expression = ExpressionLiteral::expressionLiteralForToken(token);
        expressions.push_back(expression);
    }
    return make_shared<ExpressionArrayLiteral>(expressions);
}

vector<shared_ptr<Expression>> ExpressionArrayLiteral::getExpressions() {
    return expressions;
} 