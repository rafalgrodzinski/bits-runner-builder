#include "Parser.h"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens) {
}

/*Expression Parser::getExpression() {
    return term();
}*/

/*Expression Parser::term() {

}*/

/*Expression Parser::primary() {

}*/

Expression Parser::matchInteger() {
    //Token token = tokens.at(currentIndex);
    //if (token.getKind() == Token::Kind::INTEGER)
    //    ExpressionInteger(token);

    return Expression::Invalid;
}