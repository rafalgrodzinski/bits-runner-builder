#include "Parser/Statement/Statement.h"

class Expression;

enum class StatementAssignmentKind {
    VARIABLE,
    DATA,
    BLOB
};

class StatementAssignment: public Statement {
private:
    StatementAssignmentKind assignmentKind;
    string name;
    shared_ptr<Expression> indexExpression;
    shared_ptr<Expression> expression;
    string memberName;

public:
    StatementAssignment(string name, shared_ptr<Expression> indexExpressio, shared_ptr<Expression> expression);
    StatementAssignmentKind getAssignmentKind();
    string getName();
    shared_ptr<Expression> getIndexExpression();
    shared_ptr<Expression> getExpression();
    string getMemberName();
};