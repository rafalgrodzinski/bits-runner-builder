#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <optional>
#include <vector>

class Error;

class Token;
enum class TokenKind;
class ValueType;

class Statement;
class StatementModule;

class Expression;
class ExpressionChained;

class Parsee;
class ParseeResult;
class ParseeResultsGroup;

using namespace std;

class Parser {
private:
    string defaultModuleName;
    vector<shared_ptr<Error>> errors;
    vector<shared_ptr<Token>> tokens;
    int currentIndex = 0;

    // Statements
    shared_ptr<Statement> nextStatement();
    shared_ptr<Statement> nextInBlobStatement();
    shared_ptr<Statement> nextInBlockStatement();

    shared_ptr<Statement> matchStatementModule();
    shared_ptr<Statement> matchStatementImport();
    shared_ptr<Statement> matchStatementMetaExternVariable();
    shared_ptr<Statement> matchStatementMetaExternFunction();
    shared_ptr<Statement> matchStatementVariable();
    shared_ptr<Statement> matchStatementFunction();
    shared_ptr<Statement> matchStatementRawFunction();
    shared_ptr<Statement> matchStatementBlob();

    shared_ptr<Statement> matchStatementBlock(vector<TokenKind> terminalTokenKinds);
    shared_ptr<Statement> matchStatementAssignment();
    shared_ptr<Statement> matchStatementReturn();
    shared_ptr<Statement> matchStatementRepeat();
    shared_ptr<Statement> matchStatementExpression();

    // Expressions
    shared_ptr<Expression> nextExpression();
    shared_ptr<Expression> matchLogicalOrXor(); // or, xor
    shared_ptr<Expression> matchLogicalAnd(); // and
    shared_ptr<Expression> matchLogicalNot(); // not

    shared_ptr<Expression> matchEquality(); // =, !=
    shared_ptr<Expression> matchComparison(); // <, <=, >, >=

    shared_ptr<Expression> matchBitwiseOrXor(); // |, ^
    shared_ptr<Expression> matchBitwiseAnd(); // &
    shared_ptr<Expression> matchBitwiseShift(); // <<, >>
    shared_ptr<Expression> matchBitwiseNot(); // ~

    shared_ptr<Expression> matchTerm(); // +, -
    shared_ptr<Expression> matchFactor(); // *, /, %
    shared_ptr<Expression> matchUnary(); // +, -

    shared_ptr<Expression> matchExpressionChained(shared_ptr<ExpressionChained> expression); // .stuff

    shared_ptr<Expression> matchPrimary(); // literal, ()
    shared_ptr<Expression> matchExpressionGrouping();
    shared_ptr<Expression> matchExpressionCompositeLiteral();
    shared_ptr<Expression> matchExpressionLiteral();
    shared_ptr<Expression> matchExpressionCall();
    shared_ptr<Expression> matchExpressionVariable();
    shared_ptr<Expression> matchExpressionCast();

    shared_ptr<Expression> matchExpressionIfElse(optional<bool> isMultiLine);
    shared_ptr<Expression> matchExpressionBinary(shared_ptr<Expression> left);
    shared_ptr<Expression> matchExpressionBlock(vector<TokenKind> terminalTokenKinds);

    shared_ptr<ValueType> matchValueType();
    shared_ptr<ValueType> typeForExportedStatementFromType(shared_ptr<ValueType> valueType, string moduleName);

    // Parsee
    ParseeResultsGroup parseeResultsGroupForParsees(vector<Parsee> parsees);
    optional<pair<vector<ParseeResult>, int>> groupParseeResults(vector<Parsee> groupParsees);
    optional<pair<vector<ParseeResult>, int>> repeatedGroupParseeResults(vector<Parsee> repeatedParsees);
    optional<pair<vector<ParseeResult>, int>> oneOfParseeResults(vector<vector<Parsee>> parsees);
    optional<pair<vector<ParseeResult>, int>> tokenParseeResults(TokenKind tokenKind, int tag);
    optional<pair<vector<ParseeResult>, int>> valueTypeParseeResults(int index, int tag);
    optional<pair<vector<ParseeResult>, int>> statementParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> statementInBlobParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> statementInBlockParseeResults(bool getShouldIncludeExpressionStatement, int tag);
    optional<pair<vector<ParseeResult>, int>> expressionParseeResults(bool isNumeric, int tag);
    optional<pair<vector<ParseeResult>, int>> statementBlockParseeResults(bool isMultiline, int tag);
    optional<pair<vector<ParseeResult>, int>> expressionBlockSingleLineParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> expressionBlockMultiLineParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> ifElseParseeResults(bool isMultiLine, int tag);

    // Support
    optional<vector<shared_ptr<Token>>> tryMatchingTokenKinds(vector<TokenKind> kinds, bool shouldMatchAll, bool shouldAdvance);
    void markError(optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);

public:
    Parser(string defaultModuleName, vector<shared_ptr<Token>> tokens);
    shared_ptr<StatementModule> getStatementModule();
};

#endif