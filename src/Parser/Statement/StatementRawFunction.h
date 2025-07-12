#include "Parser/Statement/Statement.h"

class Expression;

class StatementRawFunction: public Statement {
private:
    string name;
    string rawSource;

public:
    StatementRawFunction(string name, string rawSource);
    string getName();
    string getRawSource();
};