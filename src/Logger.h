#ifndef LOGGER_H
#define LOGGER_H

#include <vector>

class Token;
enum class TokenKind;
enum class ParseeKind;
class ValueType;

class Statement;
class StatementModule;
class StatementImport;
class StatementMetaExternFunction;
class StatementVariable;
class StatementFunction;
class StatementRawFunction;
class StatementBlob;
class StatementBlock;
class StatementAssignment;
class StatementReturn;
class StatementRepeat;
class StatementExpression;

class Expression;
class ExpressionBinary;
class ExpressionUnary;
class ExpressionIfElse;
class ExpressionVariable;
class ExpressionGrouping;
class ExpressionLiteral;
class ExpressionCompositeLiteral;
class ExpressionCall;
class ExpressionBlock;
class ExpressionChained;

class Error;

using namespace std;

class Logger {
private:
    static string toString(shared_ptr<Token> token);
    static string toString(TokenKind tokenKind);
    static string toString(ParseeKind parseeKind);
    static string toString(shared_ptr<ValueType> valueType);

    static string toString(shared_ptr<Statement> statement);
    static string toString(shared_ptr<StatementModule> statement);
    static string toString(shared_ptr<StatementImport> statement);
    static string toString(shared_ptr<StatementMetaExternFunction> statement);
    static string toString(shared_ptr<StatementVariable> statement);
    static string toString(shared_ptr<StatementFunction> statement);
    static string toString(shared_ptr<StatementRawFunction> statement);
    static string toString(shared_ptr<StatementBlob> statement);
    static string toString(shared_ptr<StatementBlock> statement);
    static string toString(shared_ptr<StatementAssignment> statement);
    static string toString(shared_ptr<StatementReturn> statement);
    static string toString(shared_ptr<StatementRepeat> statement);
    static string toString(shared_ptr<StatementExpression> statement);

    static string toString(shared_ptr<Expression> expression);
    static string toString(shared_ptr<ExpressionBinary> expression);
    static string toString(shared_ptr<ExpressionUnary> expression);
    static string toString(shared_ptr<ExpressionIfElse> expression);
    static string toString(shared_ptr<ExpressionVariable> expression);
    static string toString(shared_ptr<ExpressionGrouping> expression);
    static string toString(shared_ptr<ExpressionLiteral> expression);
    static string toString(shared_ptr<ExpressionCompositeLiteral> expression);
    static string toString(shared_ptr<ExpressionCall> expression);
    static string toString(shared_ptr<ExpressionBlock> expression);
    static string toString(shared_ptr<ExpressionChained> expression);

public:
    static void print(vector<shared_ptr<Token>> tokens);
    static void print(shared_ptr<StatementModule> statement);
    static void print(shared_ptr<Error> error);
};

#endif