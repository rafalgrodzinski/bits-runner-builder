#include "Parser/Statement/Statement.h"

class Expression;
class ValueType;

class StatementVariable: public Statement {
private:
    string name;
    shared_ptr<ValueType> valueType;
    shared_ptr<Expression> expression;

public:
    StatementVariable(string name, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression);
    string getName();
    shared_ptr<ValueType> getValueType();
    shared_ptr<Expression> getExpression();
};