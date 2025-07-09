#include "ExpressionArrayLiteral.h"

#include "Lexer/Token.h"
#include "Parser/Expression/ExpressionLiteral.h"

shared_ptr<ExpressionArrayLiteral> ExpressionArrayLiteral::expressionArrayLiteralForExpressions(vector<shared_ptr<Expression>> expressions) {
    shared_ptr<ExpressionArrayLiteral> expression = make_shared<ExpressionArrayLiteral>();
    expression->expressions = expressions;
    return expression;
}

shared_ptr<ExpressionArrayLiteral> ExpressionArrayLiteral::expressionArrayLiteralForTokenString(shared_ptr<Token> tokenString) {
    if (tokenString->getKind() != TokenKind::STRING)
        return nullptr;

    shared_ptr<ExpressionArrayLiteral> expression = make_shared<ExpressionArrayLiteral>();

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

        // add terminal 0 if missing
        if (i == stringValue.length() - 2 && lexme.compare("\\0") != 0) {
            shared_ptr<Token> token = make_shared<Token>(TokenKind::INTEGER_CHAR, "\\0", tokenString->getLine(), tokenString->getColumn() + i + lexme.length());
            shared_ptr<ExpressionLiteral> expression = ExpressionLiteral::expressionLiteralForToken(token);
            expressions.push_back(expression);
        }
    }

    expression->expressions = expressions;
    return expression;
}

ExpressionArrayLiteral::ExpressionArrayLiteral():
Expression(ExpressionKind::ARRAY_LITERAL, nullptr) { }

vector<shared_ptr<Expression>> ExpressionArrayLiteral::getExpressions() {
    return expressions;
} 