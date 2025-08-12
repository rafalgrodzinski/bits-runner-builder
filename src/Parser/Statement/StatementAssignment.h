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
    string identifier;
    shared_ptr<Expression> indexExpression;
    string memberName;
    shared_ptr<Expression> valueExpression;
    
public:
    StatementAssignment();
    static shared_ptr<StatementAssignment> variableAssignment(string identifier, shared_ptr<Expression> valueExpression);
    static shared_ptr<StatementAssignment> dataAssignment(string identifier, shared_ptr<Expression> indexExpression, shared_ptr<Expression> valueExpression);
    static shared_ptr<StatementAssignment> blobAssignment(string identifier, string memberName, shared_ptr<Expression> valueExpression);
    StatementAssignmentKind getAssignmentKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
    string getMemberName();
    shared_ptr<Expression> getValueExpression();
};