#include "Parser/Expression/Expression.h"

class ExpressionInvalid: public Expression {
private:
    shared_ptr<Token> token;

public:
    ExpressionInvalid(shared_ptr<Token> token);
    shared_ptr<Token> getToken();
    string toString(int indent) override;
};