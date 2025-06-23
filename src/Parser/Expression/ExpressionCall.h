#include "Parser/Expression/Expression.h"

class ExpressionCall: public Expression {
private:
    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

public:
    ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions);
    string getName();
    vector<shared_ptr<Expression>> getArgumentExpressions();
    string toString(int indent) override;
};