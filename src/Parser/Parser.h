#ifndef PARSER_H
#define PARSER_H

#include <vector>

class Error;

class Token;
enum class TokenKind;
class ValueType;

class Expression;
class Statement;
class StatementModule;

class Parsee;
class ParseeGroup;
class ParseeResult;
class ParseeResultsGroup;

using namespace std;

class Parser {
private:
    string defaultModuleName;
    vector<shared_ptr<Error>> errors;
    vector<shared_ptr<Token>> tokens;
    int currentIndex = 0;

    shared_ptr<Statement> nextStatement();
    shared_ptr<Statement> nextInBlockStatement();

    shared_ptr<Statement> matchStatementModule();
    shared_ptr<Statement> matchStatementImport();
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

    shared_ptr<Expression> nextExpression();
    shared_ptr<Expression> matchLogicalSecond(); // or
    shared_ptr<Expression> matchLogicalFirst(); // and
    shared_ptr<Expression> matchLogicalUnary(); // not
    shared_ptr<Expression> matchEquality(); // =, !=
    shared_ptr<Expression> matchComparison(); // <, <=, >, >=
    shared_ptr<Expression> matchTerm(); // +, -
    shared_ptr<Expression> matchFactor(); // *, /, %
    shared_ptr<Expression> matchUnary(); // +, -
    shared_ptr<Expression> matchPrimary(); // literal, ()

    shared_ptr<Expression> matchExpressionGrouping();
    shared_ptr<Expression> matchExpressionLiteral();
    shared_ptr<Expression> matchExpressionCompositeLiteral();
    shared_ptr<Expression> matchExpressionVariable();
    shared_ptr<Expression> matchExpressionCall();
    shared_ptr<Expression> matchExpressionIfElse();
    shared_ptr<Expression> matchExpressionBinary(shared_ptr<Expression> left);
    shared_ptr<Expression> matchExpressionBlock(vector<TokenKind> terminalTokenKinds);

    ParseeResultsGroup parseeResultsGroupForParseeGroup(ParseeGroup group);
    optional<pair<vector<ParseeResult>, int>> groupParseeResults(ParseeGroup group);
    optional<pair<vector<ParseeResult>, int>> repeatedGroupParseeResults(ParseeGroup group);
    optional<pair<vector<ParseeResult>, int>> tokenParseeResults(TokenKind tokenKind, int tag);
    optional<pair<vector<ParseeResult>, int>> valueTypeParseeResults(int index, int tag);
    optional<pair<vector<ParseeResult>, int>> statementParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> statementInBlockParseeResults(bool getShouldIncludeExpressionStatement, int tag);
    optional<pair<vector<ParseeResult>, int>> expressionParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> orParseeResults(ParseeGroup first, ParseeGroup second);
    optional<pair<vector<ParseeResult>, int>> statementBlockParseeResults(bool isMultiline, int tag);
    optional<pair<vector<ParseeResult>, int>> expressionBlockSingleLineParseeResults(int tag);
    optional<pair<vector<ParseeResult>, int>> expressionBlockMultiLineParseeResults(int tag);
    bool tryMatchingTokenKinds(vector<TokenKind> kinds, bool shouldMatchAll, bool shouldAdvance);

    void markError(optional<TokenKind> expectedTokenKind, optional<Parsee> expectedParsee, optional<string> message);

public:
    Parser(string defaultModuleName, vector<shared_ptr<Token>> tokens);
    shared_ptr<StatementModule> getStatementModule();
};

#endif