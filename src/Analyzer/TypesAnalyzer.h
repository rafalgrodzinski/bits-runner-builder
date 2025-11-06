#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>
#include <vector>

class Error;

class Expression;

class Statement;
class StatementBlock;
class StatementExpression;
class StatementFunction;
class StatementModule;
class StatementReturn;
class ValueType;

using namespace std;

class TypesAnalyzer {
private:
    vector<shared_ptr<Error>> errors;

    void checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementModule> statementModule);
    void checkStatement(shared_ptr<StatementFunction> statementFunction);
    void checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementExpression> statementExpression);

    shared_ptr<ValueType> typeForExpression(shared_ptr<Expression>);

    void markError(int line, int column, shared_ptr<ValueType> expectedType, shared_ptr<ValueType> actualType);

public:
    void checkModule(shared_ptr<StatementModule> module);
};

#endif