#include "Parser/Statement/Statement.h"

class Expression;

class StatementVariable: public Statement {
private:
    string name;
    ValueType valueType;
    shared_ptr<Expression> expression;

public:
    StatementVariable(string name, ValueType valueType, shared_ptr<Expression> expression);
    string getName();
    ValueType getValueType();
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};