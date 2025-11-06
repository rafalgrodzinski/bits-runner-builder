#include "TypesAnalyzer.h"

#include "Error.h"
#include "Logger.h"
#include "Parser/Expression/Expression.h"
#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/ValueType.h"

void TypesAnalyzer::checkModule(shared_ptr<StatementModule> module) {
    for (shared_ptr<Statement> statement : module->getStatements())
        checkStatement(statement, nullptr);

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }
}

//
// Statements
//
void TypesAnalyzer::checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType) {
    switch (statement->getKind()) {
        case StatementKind::MODULE:
            checkStatement(dynamic_pointer_cast<StatementModule>(statement));
            break;
        case StatementKind::FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::BLOCK:
            checkStatement(dynamic_pointer_cast<StatementBlock>(statement), returnType);
            break;
        case StatementKind::RETURN:
            checkStatement(dynamic_pointer_cast<StatementReturn>(statement), returnType);
            break;
        case StatementKind::EXPRESSION:
            checkStatement(dynamic_pointer_cast<StatementExpression>(statement));
            break;
    }
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementModule> statementModule) {
    checkStatement(statementModule, nullptr);
}


void TypesAnalyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType) {
    shared_ptr<ValueType> expressionType = typeForExpression(statementReturn->getExpression());

    if (expressionType != returnType)
        markError(statementReturn->getExpression()->getLine(), statementReturn->getExpression()->getColumn(), returnType, expressionType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementExpression> statementExpression) {

}

//
// Expressions
//
shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<Expression>) {
    return ValueType::NONE;
}

void TypesAnalyzer::markError(int line, int column, shared_ptr<ValueType> expectedType, shared_ptr<ValueType> actualType) {
    errors.push_back(Error::analyzerTypeError(line, column, expectedType, actualType));
}
