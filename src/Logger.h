#ifndef LOGGER_H
#define LOGGER_H

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Error;
class Location;
class Module;
class Parsee;
class Token;
struct EnumField;

class Statement;
class StatementAssignment;
class StatementBlob;
class StatementBlobDeclaration;
class StatementBlock;
class StatementEnum;
class StatementExpression;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementMetaExternFunction;
class StatementMetaExternVariable;
class StatementMetaImport;
class StatementModule;
class StatementProto;
class StatementProtoDeclaration;
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

class ValueType;
class ValueTypeBlob;
class ValueTypeBoxed;
class ValueTypeEnum;
class ValueTypeEnumField;
class ValueTypeFun;
class ValueTypePtr;

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
    static string toString(shared_ptr<StatementEnum> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementExpression> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementFunctionDeclaration> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaExternFunction> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaExternVariable> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementMetaImport> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementModule> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementProto> statement, vector<IndentKind> indents);
    static string toString(shared_ptr<StatementProtoDeclaration> statement, vector<IndentKind> indents);
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
    static string toString(shared_ptr<ExpressionChained> expression, vector<IndentKind> indents, bool isInline);
    static string toString(shared_ptr<ExpressionCompositeLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionGrouping> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionIfElse> expression, vector<IndentKind> indents, bool isInline);
    static string toString(shared_ptr<ExpressionLiteral> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionUnary> expression, vector<IndentKind> indents);
    static string toString(shared_ptr<ExpressionValue> expression, vector<IndentKind> indents);

    // value type
    static string toString(shared_ptr<ValueTypeBlob> valueTypeBlob);
    static string toString(shared_ptr<ValueTypeBoxed> valueTypeBoxed);
    static string toString(shared_ptr<ValueTypeEnum> valueType);
    static string toString(shared_ptr<ValueTypeEnumField> valueType);
    static string toString(shared_ptr<ValueTypeFun> valueType);
    static string toString(shared_ptr<ValueTypePtr> valueType);
 
    // general support
    static string toString(EnumField field, vector<IndentKind> indents);
    static string formattedLine(const string &line, const vector<IndentKind> &indents);
    static vector<IndentKind> adjustedLastIndent(vector<IndentKind> indents);

    // errors support
    static string toString(Parsee parsee);
    static string toString(TokenKind tokenKind); // only kind

public:
    static void print(const vector<shared_ptr<Token>> &tokens);
    static void print(shared_ptr<Module> module);
    static void printExportedHeaderStatements(const map<string, vector<shared_ptr<Statement>>> &statmentsMap);
    static void print(shared_ptr<Error> error);

    static string toString(shared_ptr<Location> location);
    static string toString(shared_ptr<ValueType> valueType);
    static string toString(ExpressionUnaryOperation operationUnary);
    static string toString(ExpressionBinaryOperation operationBinary);
};

#endif