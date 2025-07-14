#include "Parser/Statement/Statement.h"

class Expression;

class StatementRawFunction: public Statement {
private:
    string name;
    string constraints;
    string rawSource;

public:
    StatementRawFunction(string name, string constraints, string rawSource);
    string getName();
    string getConstraints();
    string getRawSource();
};