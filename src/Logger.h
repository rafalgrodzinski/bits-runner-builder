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
class ValueTypeComposite;
class ValueTypeData;
class ValueTypeEnum;
class ValueTypeEnumField;
class ValueTypeFun;
class ValueTypeProto;
class ValueTypePtr;
class ValueTypeSimple;

enum class IndentKind {
    ROOT,
    EMPTY,
    NODE,
    NODE_LAST,
    BRANCH
};

class Logger {
public:
    static void print(const std::vector<std::shared_ptr<Token>> &tokens);
    static void print(std::shared_ptr<Module> module);
    static void printExportedHeaderStatements(const std::map<std::string, std::vector<std::shared_ptr<Statement>>> &statmentsMap);
    static void print(std::shared_ptr<Error> error);

    static std::string toString(std::shared_ptr<Location> location);
    static std::string toString(std::shared_ptr<ValueType> valueType);
    static std::string toString(ExpressionUnaryOperation operationUnary);
    static std::string toString(ExpressionBinaryOperation operationBinary);

private:
    // lexer
    static std::string toString(std::shared_ptr<Token> token); // kind and contents

    // parser statements
    static std::string toString(std::shared_ptr<Statement> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementAssignment> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementBlob> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementBlobDeclaration> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementBlock> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementEnum> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementExpression> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementFunction> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementFunctionDeclaration> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementMetaExternFunction> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementMetaExternVariable> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementMetaImport> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementModule> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementProto> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementProtoDeclaration> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementRawFunction> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementRepeat> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementReturn> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementVariable> statement, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<StatementVariableDeclaration> statement, std::vector<IndentKind> indents);

    // parser expressions
    static std::string toString(std::shared_ptr<Expression> expression, std::vector<IndentKind> indents, bool isInline);
    static std::string toString(std::shared_ptr<ExpressionBinary> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionBlock> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionCall> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionCast> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionChained> expression, std::vector<IndentKind> indents, bool isInline);
    static std::string toString(std::shared_ptr<ExpressionCompositeLiteral> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionGrouping> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionIfElse> expression, std::vector<IndentKind> indents, bool isInline);
    static std::string toString(std::shared_ptr<ExpressionLiteral> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionUnary> expression, std::vector<IndentKind> indents);
    static std::string toString(std::shared_ptr<ExpressionValue> expression, std::vector<IndentKind> indents);

    // value type
    static std::string toString(std::shared_ptr<ValueTypeBlob> valueTypeBlob);
    static std::string toString(std::shared_ptr<ValueTypeBoxed> valueTypeBoxed);
    static std::string toString(std::shared_ptr<ValueTypeComposite> ValueTypeComposite);
    static std::string toString(std::shared_ptr<ValueTypeData> valueTypeData);
    static std::string toString(std::shared_ptr<ValueTypeEnum> valueTypeEnum);
    static std::string toString(std::shared_ptr<ValueTypeEnumField> valueTypeEnumField);
    static std::string toString(std::shared_ptr<ValueTypeFun> valueTypeFun);
    static std::string toString(std::shared_ptr<ValueTypeProto> valueTypeProto);
    static std::string toString(std::shared_ptr<ValueTypePtr> valueTypePtr);
    static std::string toString(std::shared_ptr<ValueTypeSimple> valueTypeSimple);
 
    // general support
    static std::string toString(EnumField field, std::vector<IndentKind> indents);
    static std::string formattedLine(const std::string &line, const std::vector<IndentKind> &indents);
    static std::vector<IndentKind> adjustedLastIndent(std::vector<IndentKind> indents);

    // errors support
    static std::string toString(Parsee parsee);
    static std::string toString(TokenKind tokenKind); // only kind
};

#endif