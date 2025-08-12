#include "Parser/Statement/Statement.h"

class Expression;

enum class StatementAssignmentKind {
    SIMPLE,
    DATA,
    BLOB
};

class StatementAssignment: public Statement {
private:
    StatementAssignmentKind assignmentKind;
    string identifier;
    shared_ptr<Expression> indexExpression;
    string memberName;
    shared_ptr<Expression> valueExpression;
    
public:
    static shared_ptr<StatementAssignment> simple(string identifier, shared_ptr<Expression> valueExpression);
    static shared_ptr<StatementAssignment> data(string identifier, shared_ptr<Expression> indexExpression, shared_ptr<Expression> valueExpression);
    static shared_ptr<StatementAssignment> blob(string identifier, string memberName, shared_ptr<Expression> valueExpression);

    StatementAssignment();
    StatementAssignmentKind getAssignmentKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
    string getMemberName();
    shared_ptr<Expression> getValueExpression();
};