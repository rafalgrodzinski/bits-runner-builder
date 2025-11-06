#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>

class StatementModule;
class StatementFunction;

using namespace std;

class TypesAnalyzer {
public:
    void checkModule(shared_ptr<StatementModule> module);
    void checkStatement(shared_ptr<Statement> statement);
    void checkStatementFunction(shared_ptr<StatementFunction> statementFunction);

};

#endif