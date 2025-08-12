#include "Parser/Expression/Expression.h"

enum class ExpressionVariableKind {
    SIMPLE,
    DATA,
    BLOB
};

class ExpressionVariable: public Expression {
private:
    ExpressionVariableKind variableKind;
    string identifier;
    shared_ptr<Expression> indexExpression;
    string memberName;

public:
    static shared_ptr<ExpressionVariable> simpleVariable(string identifer);
    static shared_ptr<ExpressionVariable> dataVariable(string identifier, shared_ptr<Expression> indexExpression);
    static shared_ptr<ExpressionVariable> blobVariable(string identifier, string memberName);

    ExpressionVariable();
    ExpressionVariableKind getVariableKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
    string getMemberName();
};