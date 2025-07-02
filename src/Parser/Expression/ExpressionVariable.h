#include "Parser/Expression/Expression.h"

class ExpressionVariable: public Expression {
private:
    string name;

public:
    ExpressionVariable(string name);
    string getName();
};