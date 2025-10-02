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
class StatementFunctionDeclaration;
class StatementRawFunction;
class StatementBlob;
class StatementBlobDeclaration;
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

enum class IndentKind {
    ROOT,
    EMPTY,
    NODE,
    NODE_LAST,
    BRANCH
};

class Logger {
private:
    static string toString(shared_ptr<Token> token);
    static string toString(TokenKind tokenKind);
    static string toString(ParseeKind parseeKind);
    static string toString(shared_ptr<ValueType> valueType);

    static string toString(shared_ptr<Statement> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementModule> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementImport> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaExternFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementVariable> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunctionDeclaration> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementRawFunction> statement);
    static string toString(shared_ptr<StatementBlob> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementBlobDeclaration> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementBlock> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementAssignment> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementReturn> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementRepeat> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementExpression> statement, vector<IndentKind> indents);

    static string toString(shared_ptr<Expression> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionBinary> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionUnary> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionIfElse> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionVariable> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionGrouping> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionCompositeLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionCall> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionBlock> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionChained> expression, vector<IndentKind> indents);

    static string formattedLine(string line, vector<IndentKind> indents);
    static vector<IndentKind> adjustedLastIndent(vector<IndentKind> indents);

public:
    static void print(vector<shared_ptr<Token>> tokens);
    static void print(shared_ptr<StatementModule> statement);
    static void print(shared_ptr<Error> error);
};

#endif