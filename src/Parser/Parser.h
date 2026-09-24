#ifndef PARSER_H
#define PARSER_H

#include <format>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Error;

enum class TokenKind;
class Token;
class ValueType;

enum class StatementKind;
class Statement;
class StatementModule;

class Expression;
class ExpressionChained;

class Parsee;
class ParseeResult;
class ParseeResultsGroup;

class Parser {
public:
    Parser(const std::vector<std::shared_ptr<Token>> &tokens);

    std::vector<std::shared_ptr<Statement>> getStatements();

private:
    std::vector<std::shared_ptr<Error>> errors;
    std::vector<std::shared_ptr<Token>> tokens;
    int currentIndex = 0;

    // Statements
    std::shared_ptr<Statement> nextInBlockStatement();

    std::shared_ptr<Statement> matchStatementAssignment();
    std::shared_ptr<Statement> matchStatementBlob();
    std::shared_ptr<Statement> matchStatementBlock(std::vector<TokenKind> terminalTokenKinds);
    std::shared_ptr<Statement> matchStatementEnum();
    std::shared_ptr<Statement> matchStatementExpression();
    std::shared_ptr<Statement> matchStatementFunction();
    std::shared_ptr<Statement> matchStatementFunctionDeclaration();
    std::shared_ptr<Statement> matchStatementMetaExternFunction();
    std::shared_ptr<Statement> matchStatementMetaExternVariable();
    std::shared_ptr<Statement> matchStatementMetaImport();
    std::shared_ptr<Statement> matchStatementModule();
    std::shared_ptr<Statement> matchStatementProto();
    std::shared_ptr<Statement> matchStatementRawFunction();
    std::shared_ptr<Statement> matchStatementRepeat();
    std::shared_ptr<Statement> matchStatementReturn();
    std::shared_ptr<Statement> matchStatementVariable();
    std::shared_ptr<Statement> matchStatementVariableDeclaration();

    // Expressions
    std::shared_ptr<Expression> nextExpression();
    std::shared_ptr<Expression> matchLogicalOrXor(); // or, xor
    std::shared_ptr<Expression> matchLogicalAnd(); // and
    std::shared_ptr<Expression> matchLogicalNot(); // not

    std::shared_ptr<Expression> matchEquality(); // =, !=
    std::shared_ptr<Expression> matchComparison(); // <, <=, >, >=

    std::shared_ptr<Expression> matchBitwiseTest(); // &?
    std::shared_ptr<Expression> matchBitwiseOrXor(); // |, ^
    std::shared_ptr<Expression> matchBitwiseAnd(); // &
    std::shared_ptr<Expression> matchBitwiseShift(); // <<, >>
    std::shared_ptr<Expression> matchBitwiseNot(); // ~

    std::shared_ptr<Expression> matchTerm(); // +, -
    std::shared_ptr<Expression> matchFactor(); // *, /, %
    std::shared_ptr<Expression> matchUnary(); // +, -

    std::shared_ptr<Expression> matchExpressionChained(std::shared_ptr<ExpressionChained> expression); // .stuff

    std::shared_ptr<Expression> matchPrimary(); // literal, ()
    std::shared_ptr<Expression> matchExpressionBinary(std::shared_ptr<Expression> left);
    std::shared_ptr<Expression> matchExpressionBlock(std::vector<TokenKind> terminalTokenKinds);
    std::shared_ptr<Expression> matchExpressionCall();
    std::shared_ptr<Expression> matchExpressionCast();
    std::shared_ptr<Expression> matchExpressionCompositeLiteral();
    std::shared_ptr<Expression> matchExpressionGrouping();
    std::shared_ptr<Expression> matchExpressionIfElse(std::optional<bool> isMultiLine);
    std::shared_ptr<Expression> matchExpressionLiteral();
    std::shared_ptr<Expression> matchExpressionValue();

    std::shared_ptr<ValueType> matchValueType();

    // Parsee
    ParseeResultsGroup parseeResultsGroupForParsees(std::vector<Parsee> parsees);
    std::optional<std::pair<std::vector<ParseeResult>, int>> groupParseeResults(std::vector<Parsee> groupParsees);
    std::optional<std::pair<std::vector<ParseeResult>, int>> repeatedGroupParseeResults(std::vector<Parsee> repeatedParsees);
    std::optional<std::pair<std::vector<ParseeResult>, int>> oneOfParseeResults(std::vector<std::vector<Parsee>> parsees);
    std::optional<std::pair<std::vector<ParseeResult>, int>> tokenParseeResults(TokenKind tokenKind, int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> valueTypeParseeResults(int index, int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> statementKindsParseeResults(std::vector<StatementKind> statementKinds, int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> expressionParseeResults(bool isNumeric, int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> statementBlockParseeResults(bool isMultiline, int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> expressionBlockSingleLineParseeResults(int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> expressionBlockMultiLineParseeResults(int tag);
    std::optional<std::pair<std::vector<ParseeResult>, int>> ifElseParseeResults(std::optional<bool> isMultiLine, int tag);

    // Support
    std::optional<std::vector<std::shared_ptr<Token>>> tryMatchingTokenKinds(
        std::vector<TokenKind> kinds,
        bool shouldMatchAll,
        bool shouldAdvance,
        bool shouldSkipNewLine
    );
    void markError(std::optional<TokenKind> expectedTokenKind, std::optional<Parsee> expectedParsee, std::optional<std::string> message);
};

#endif