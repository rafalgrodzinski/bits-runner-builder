#ifndef LOGGER_H
#define LOGGER_H

#include <format>
#include <memory>
#include <string>
#include <vector>

class Error;
class Location;
class Parsee;
class Token;
class ValueType;

class Statement;
class StatementAssignment;
class StatementBlob;
class StatementBlobDeclaration;
class StatementBlock;
class StatementExpression;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementMetaExternFunction;
class StatementMetaExternVariable;
class StatementMetaImport;
class StatementModule;
class StatementRawFunction;
class StatementRepeat;
class StatementReturn;
class StatementVariable;
class StatementVariableDeclaration;

class Expression;
class ExpressionBinary;
class ExpressionBlock;
class ExpressionCall;
class ExpressionCast;
class ExpressionChained;
class ExpressionCompositeLiteral;
class ExpressionGrouping;
class ExpressionIfElse;
class ExpressionLiteral;
class ExpressionUnary;
class ExpressionValue;

enum class ExpressionBinaryOperation;
enum class ExpressionUnaryOperation;
enum class TokenKind;

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
    // lexer
    static string toString(shared_ptr<Token> token); // kind and contents

    // parser statements
    static string toString(shared_ptr<Statement> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementAssignment> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementBlob> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementBlobDeclaration> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementBlock> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementExpression> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunctionDeclaration> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaExternFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaExternVariable> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaImport> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementModule> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementRawFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementRepeat> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementReturn> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementVariable> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementVariableDeclaration> statement, vector<IndentKind> indents);

    // parser expressions
    static string toString(shared_ptr<Expression> expression, vector<IndentKind> indents, bool isInline);
    static string toString(shared_ptr<ExpressionBinary> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionBlock> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionCall> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionCast> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionChained> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionCompositeLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionGrouping> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionIfElse> expression, vector<IndentKind> indents, bool isInline);
    static string toString(shared_ptr<ExpressionLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionUnary> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionValue> expression, vector<IndentKind> indents);

    // general support
    static string formattedLine(string line, vector<IndentKind> indents);
    static vector<IndentKind> adjustedLastIndent(vector<IndentKind> indents);

    // errors support
    static string toString(Parsee parsee);
    static string toString(TokenKind tokenKind); // only kind

public:
    static void print(vector<shared_ptr<Token>> tokens);
    static void printModuleStatements(string moduleName, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements);
    static void printExportedStatements(string moduleName, vector<shared_ptr<Statement>> statments);
    static void print(shared_ptr<Error> error);

    static string toString(shared_ptr<Location> location);
    static string toString(shared_ptr<ValueType> valueType);
    static string toString(ExpressionUnaryOperation operationUnary);
    static string toString(ExpressionBinaryOperation operationBinary);
};

#endif