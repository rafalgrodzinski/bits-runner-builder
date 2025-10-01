#include "Logger.h"

#include <iostream>

#include "Error.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementImport.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementExpression.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionChained.h"

string Logger::toString(shared_ptr<Token> token) {
    switch (token->getKind()) {
        case TokenKind::PLUS:
            return "+";
        case TokenKind::MINUS:
            return "-";
        case TokenKind::STAR:
            return "*";
        case TokenKind::SLASH:
            return "/";
        case TokenKind::PERCENT:
            return "%";

        case TokenKind::OR:
            return "OR";
        case TokenKind::XOR:
            return "XOR";
        case TokenKind::AND:
            return "AND";
        case TokenKind::NOT:
            return "NOT";
        
        case TokenKind::EQUAL:
            return "=";
        case TokenKind::NOT_EQUAL:
            return "≠";
        case TokenKind::LESS:
            return "<";
        case TokenKind::LESS_EQUAL:
            return "≤";
        case TokenKind::GREATER:
            return ">";
        case TokenKind::GREATER_EQUAL:
            return "≥";

        case TokenKind::LEFT_ROUND_BRACKET:
            return "(";
        case TokenKind::RIGHT_ROUND_BRACKET:
            return ")";
        case TokenKind::LEFT_SQUARE_BRACKET:
            return "[";
        case TokenKind::RIGHT_SQUARE_BRACKET:
            return "]";
        case TokenKind::LEFT_CURLY_BRACKET:
            return "{";
        case TokenKind::RIGHT_CURLY_BRACKET:
            return "}";
        case TokenKind::COMMA:
            return ",";
        case TokenKind::COLON:
            return ":";
        case TokenKind::SEMICOLON:
            return ";";
        case TokenKind::LEFT_ARROW:
            return "←";
        case TokenKind::RIGHT_ARROW:
            return "→";
        case TokenKind::DOT:
            return ".";

        case TokenKind::BOOL:
            return "BOOL(" + token->getLexme() + ")";
        case TokenKind::INTEGER_DEC:
            return "INT_DEC(" + token->getLexme() + ")";
        case TokenKind::INTEGER_HEX:
            return "INT_HEX(" + token->getLexme() + ")";
        case TokenKind::INTEGER_BIN:
            return "INT_BIN(" + token->getLexme() + ")";
        case TokenKind::INTEGER_CHAR:
            return "INT_CHAR(" + token->getLexme() + ")";
        case TokenKind::REAL:
            return "REAL(" + token->getLexme() + ")";
        case TokenKind::STRING:
            return "STRING(" + token->getLexme() + ")";
        case TokenKind::IDENTIFIER:
            return "ID(" + token->getLexme() + ")";
        case TokenKind::TYPE:
            return "TYPE(" + token->getLexme() + ")";

        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RAW_FUNCTION:
            return "RAW";
        case TokenKind::RAW_SOURCE_LINE:
            return format("RAW_SOURCE_LINE({})", token->getLexme());
        case TokenKind::BLOB:
            return "BLOB";
        case TokenKind::RETURN:
            return "RET";
        case TokenKind::REPEAT:
            return "REP";
        case TokenKind::IF:
            return "IF";
        case TokenKind::ELSE:
            return "ELSE";
        
        case TokenKind::M_MODULE:
            return "@MODULE";
        case TokenKind::M_IMPORT:
            return "@IMPORT";
        case TokenKind::M_EXPORT:
            return "@EXPORT";
        case TokenKind::M_EXTERN:
            return "@EXTERN";
        case TokenKind::META:
            return "@";

        case TokenKind::NEW_LINE:
            return "↲";
        case TokenKind::END:
            return "END";
    }
}

string Logger::toString(TokenKind tokenKind) {
    TokenKind tk = tokenKind;
    switch (tokenKind) {
        case TokenKind::PLUS:
            return "+";
        case TokenKind::MINUS:
            return "-";
        case TokenKind::STAR:
            return "*";
        case TokenKind::SLASH:
            return "/";
        case TokenKind::PERCENT:
            return "%";

        case TokenKind::OR:
            return "OR";
        case TokenKind::XOR:
            return "XOR";
        case TokenKind::AND:
            return "AND";
        case TokenKind::NOT:
            return "NOT";
        
        case TokenKind::EQUAL:
            return "=";
        case TokenKind::NOT_EQUAL:
            return "≠";
        case TokenKind::LESS:
            return "<";
        case TokenKind::LESS_EQUAL:
            return "≤";
        case TokenKind::GREATER:
            return ">";
        case TokenKind::GREATER_EQUAL:
            return "≥";

        case TokenKind::LEFT_ROUND_BRACKET:
            return "(";
        case TokenKind::RIGHT_ROUND_BRACKET:
            return ")";
        case TokenKind::LEFT_SQUARE_BRACKET:
            return "[";
        case TokenKind::RIGHT_SQUARE_BRACKET:
            return "]";
        case TokenKind::LEFT_CURLY_BRACKET:
            return "{";
        case TokenKind::RIGHT_CURLY_BRACKET:
            return "}";
        case TokenKind::COMMA:
            return ",";
        case TokenKind::COLON:
            return ":";
        case TokenKind::SEMICOLON:
            return ";";
        case TokenKind::LEFT_ARROW:
            return "←";
        case TokenKind::RIGHT_ARROW:
            return "→";
        case TokenKind::DOT:
            return ".";

        case TokenKind::BOOL:
            return "LITERAL(BOOLEAN)";
        case TokenKind::INTEGER_DEC:
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            return "LITERAL(INTEGER)";
        case TokenKind::REAL:
            return "LITERAL(REAL)";
        case TokenKind::STRING:
            return "LITERAL(STRING)";
        case TokenKind::IDENTIFIER:
            return "LITERAL(ID)";
        case TokenKind::TYPE:
            return "TYPE";

        case TokenKind::IF:
            return "IF";
        case TokenKind::ELSE:
            return "ELSE";
        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RAW_FUNCTION:
            return "RAW";
        case TokenKind::RETURN:
            return "RET";
        case TokenKind::REPEAT:
            return "REP";

        case TokenKind::M_MODULE:
            return "@MODULE";
        case TokenKind::M_IMPORT:
            return "@IMPORT";
        case TokenKind::M_EXPORT:
            return "@EXPORT";
        case TokenKind::M_EXTERN:
            return "@EXTERN";
        case TokenKind::META:
            return "@";

        case TokenKind::NEW_LINE:
            return "↲";
        case TokenKind::END:
            return "END";
    }
}

string Logger::toString(ParseeKind parseeKind) {
    switch (parseeKind) {
        case ParseeKind::TOKEN:
            return "Token";
        case ParseeKind::VALUE_TYPE:
            return "Value Type";
        case ParseeKind::STATEMENT:
            return "Statement";
        case ParseeKind::STATEMENT_IN_BLOCK:
            return "Statement in Block";
        case ParseeKind::EXPRESSION:
            return "Expression";
        default:
            return "Other";
    }
}

string Logger::toString(shared_ptr<ValueType> valueType) {
    if (valueType == nullptr)
        return "{INVALID}";

    switch (valueType->getKind()) {
        case ValueTypeKind::NONE:
            return "NONE";
        case ValueTypeKind::BOOL:
            return "BOOL";
        case ValueTypeKind::U8:
            return "U8";
        case ValueTypeKind::U32:
            return "U32";
        case ValueTypeKind::U64:
            return "U64";
        case ValueTypeKind::S8:
            return "S8";
        case ValueTypeKind::S32:
            return "S32";
        case ValueTypeKind::S64:
            return "S64";
        case ValueTypeKind::R32:
            return "R32";
        case ValueTypeKind::DATA:
            return "[]";
        case ValueTypeKind::BLOB:
            return format("BLOB({})", valueType->getTypeName());
        case ValueTypeKind::PTR:
            return format("PTR<{}>", toString(valueType->getSubType()));
    }
}

string Logger::toString(shared_ptr<Statement> statement, vector<IndentKind> indents) {
    switch (statement->getKind()) {
        case StatementKind::MODULE:
            return toString(dynamic_pointer_cast<StatementModule>(statement), indents);
        case StatementKind::META_IMPORT:
            return toString(dynamic_pointer_cast<StatementImport>(statement));
        case StatementKind::META_EXTERN_FUNCTION:
            return toString(dynamic_pointer_cast<StatementMetaExternFunction>(statement), indents);
        case StatementKind::VARIABLE:
            return toString(dynamic_pointer_cast<StatementVariable>(statement), indents);
        case StatementKind::FUNCTION:
            return toString(dynamic_pointer_cast<StatementFunction>(statement), indents);
        case StatementKind::RAW_FUNCTION:
            return toString(dynamic_pointer_cast<StatementRawFunction>(statement));
        case StatementKind::BLOB:
            return toString(dynamic_pointer_cast<StatementBlob>(statement));
        case StatementKind::BLOCK:
            return toString(dynamic_pointer_cast<StatementBlock>(statement), indents);
        case StatementKind::ASSIGNMENT:
            return toString(dynamic_pointer_cast<StatementAssignment>(statement));
        case StatementKind::RETURN:
            return toString(dynamic_pointer_cast<StatementReturn>(statement));
        case StatementKind::REPEAT:
            return toString(dynamic_pointer_cast<StatementRepeat>(statement), indents);
        case StatementKind::EXPRESSION:
            return toString(dynamic_pointer_cast<StatementExpression>(statement));
    }
}

string Logger::toString(shared_ptr<StatementModule> statement, vector<IndentKind> indents) {
    string text;

    string line = format("MODULE: `{}`", statement->getName());
    text += formattedLine(line, indents);

    indents.at(indents.size()-1) = IndentKind::NONE;

    int statementsCount = statement->getStatements().size();
    for (int i=0; i<statementsCount; i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < statementsCount - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);
        text += toString(statement->getStatements().at(i), currentIndents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementImport> statement) {
    return format("IMPORT({})", statement->getName());
}

string Logger::toString(shared_ptr<StatementMetaExternFunction> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("@EXTERN FUN: `{}` → {}", statement->getName(), toString(statement->getReturnValueType()));
    text += formattedLine(line, indents);

    // arguments
    vector<IndentKind> argIndents = indents;
    argIndents.at(argIndents.size()-1) = IndentKind::BRANCH;
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}`: {}", arg.first, toString(arg.second));
        text += formattedLine(line, argIndents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementVariable> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("VAR: `{}`: {}", statement->getName(), toString(statement->getValueType()));
    text += formattedLine(line, indents);

    // initializer
    if (statement->getExpression() != nullptr) {
        indents.at(indents.size()-1) = IndentKind::BRANCH;
        line = format("← {}", toString(statement->getExpression()));
        text += formattedLine(line, indents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementFunction> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("FUN: `{}`", statement->getName());
    text += formattedLine(line, indents);

    indents.at(indents.size()-1) = IndentKind::BRANCH;

    // arguments
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}`: {}", arg.first, toString(arg.second));
        text += formattedLine(line, indents);
    }

    // body
    text += toString(statement->getStatementBlock(), indents);

    return text;
}

string Logger::toString(shared_ptr<StatementRawFunction> statement) {
    string text;

    string argsString;
    for (int i = 0; i < statement->getArguments().size(); i++) {
        auto arg = statement->getArguments().at(i);
        argsString +=  format("ARG({}, {})", arg.first, toString(arg.second));
    }
    text += format("RAW(\"{}\"|{}|{}):\n", statement->getName(), argsString, toString(statement->getReturnValueType()));

    text += statement->getRawSource();

    return text;
}

string Logger::toString(shared_ptr<StatementBlob> statement) {
    string text;

    text += format("BLOB(\"{}\"):\n", statement->getIdentifier());
    for (pair<string, shared_ptr<ValueType>> &variable : statement->getVariables())
        text += format("{}: {}\n", variable.first, toString(variable.second));

    return text;
}

string Logger::toString(shared_ptr<StatementBlock> statement, vector<IndentKind> indents) {
    string text;

    int statementsCount = statement->getStatements().size();
    for (int i=0; i<statementsCount; i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < statementsCount - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += toString(statement->getStatements().at(i), currentIndents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementAssignment> statement) {
    string text;
    /*if (statement->getParentStatement() != nullptr) {
        text += toString(statement->getParentStatement());
        text += ".";
    }*/
    //text += toString(statement->getChainExpression());
    return text;
}

string Logger::toString(shared_ptr<StatementReturn> statement) {
    string text = "RET";
    if (statement->getExpression() != nullptr)
        text += format("({})", toString(statement->getExpression()));
    return text;
}

string Logger::toString(shared_ptr<StatementRepeat> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    text += formattedLine("REP", indents);

    indents.at(indents.size()-1) = IndentKind::NONE;

    // init-statement
    if (statement->getInitStatement() != nullptr) {
        line = toString(statement->getInitStatement(), indents);
        text += formattedLine(line, indents);
    }

    // pre-condition
    if (statement->getPreConditionExpression() != nullptr) {
        line = toString(statement->getPreConditionExpression());
        text += formattedLine(line, indents);
    }

    // post-condition
    if (statement->getPostConditionExpression() != nullptr) {
        line = toString(statement->getPostConditionExpression());
        text += formattedLine(line, indents);
    }

    // post-statement
        if (statement->getPostStatement() != nullptr) {
        line = toString(statement->getPostStatement(), indents);
        text += formattedLine(line, indents);
    }

    // body
    text += toString(statement->getBodyBlockStatement(), indents);


    /*string initStatement;
    string preCondition;
    string postCondition;

    if (statement->getInitStatement() != nullptr)
        initStatement = toString(statement->getInitStatement(), {});
    
    if (statement->getPostConditionExpression() != nullptr)
        preCondition = toString(statement->getPreConditionExpression());
    
    if (statement->getPostConditionExpression() != nullptr)
        postCondition = toString(statement->getPostConditionExpression());

    text += format("REP({}|{}|{}):\n", initStatement, preCondition, postCondition);
    text += toString(statement->getBodyBlockStatement(), { });*/

    return text;
}

string Logger::toString(shared_ptr<StatementExpression> statement) {
    return format("EXPR({})", toString(statement->getExpression()));
}

string Logger::toString(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
        return toString(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::UNARY:
            return toString(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::IF_ELSE:
            return toString(dynamic_pointer_cast<ExpressionIfElse>(expression));
        case ExpressionKind::VARIABLE:
            return toString(dynamic_pointer_cast<ExpressionVariable>(expression));
        case ExpressionKind::GROUPING:
            return toString(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::LITERAL:
            return toString(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::COMPOSITE_LITERAL:
            return toString(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression));
        case ExpressionKind::CALL:
            return toString(dynamic_pointer_cast<ExpressionCall>(expression));
        case ExpressionKind::BLOCK:
            return toString(dynamic_pointer_cast<ExpressionBlock>(expression));
        case ExpressionKind::CHAINED:
            return toString(dynamic_pointer_cast<ExpressionChained>(expression));
    }
}

string Logger::toString(shared_ptr<ExpressionBinary> expression) {
    switch (expression->getOperation()) {
        case ExpressionBinaryOperation::OR:
            return "{OR " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::XOR:
            return "{XOR " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::AND:
            return "{AND " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::EQUAL:
            return "{= " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::NOT_EQUAL:
            return "{!= " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::LESS:
            return "{< " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::LESS_EQUAL:
            return "{<= " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::GREATER:
            return "{> " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::GREATER_EQUAL:
            return "{<= " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::ADD:
            return "{+ " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::SUB:
            return "{- " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::MUL:
            return "{* " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::DIV:
            return "{/ " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::MOD:
            return "{% " + toString(expression->getLeft()) + " " + toString(expression->getRight()) + "}";
        case ExpressionBinaryOperation::INVALID:
            return "{INVALID}";
    }
}

string Logger::toString(shared_ptr<ExpressionUnary> expression) {
    switch (expression->getOperation()) {
        case ExpressionUnaryOperation::NOT:
            return "{NOT " + toString(expression->getExpression()) + "}";
        case ExpressionUnaryOperation::PLUS:
            return "+" + toString(expression->getExpression());
        case ExpressionUnaryOperation::MINUS:
            return "-" + toString(expression->getExpression());
        case ExpressionUnaryOperation::INVALID:
            return "{INVALID}";
    }
}

string Logger::toString(shared_ptr<ExpressionIfElse> expression) {
    string text;

    text += format("IF({}):\n", toString(expression->getCondition()));
    text += toString(expression->getThenBlock());
    if (expression->getElseBlock() != nullptr) {
        text += "\nELSE:\n";
        text += toString(expression->getElseBlock());
    }
    text += "\n;";

    return text;
}

string Logger::toString(shared_ptr<ExpressionVariable> expression) {
    switch (expression->getVariableKind()) {
        case ExpressionVariableKind::SIMPLE:
            return format("VAR({})", expression->getIdentifier());
        case ExpressionVariableKind::DATA:
            return format("VAR({}|{})", expression->getIdentifier(), toString(expression->getIndexExpression()));
    }
}

string Logger::toString(shared_ptr<ExpressionGrouping> expression) {
    return format("({})", toString(expression->getExpression()));
}

string Logger::toString(shared_ptr<ExpressionLiteral> expression) {
    switch (expression->getLiteralKind()) {
        case LiteralKind::BOOL:
        return expression->getBoolValue() ? "true" : "false";
        case LiteralKind::UINT:
            return to_string(expression->getUIntValue());
        case LiteralKind::SINT:
            return to_string(expression->getSIntValue());
        case LiteralKind::REAL:
            return to_string(expression->getRealValue());
    }
}

string Logger::toString(shared_ptr<ExpressionCompositeLiteral> expression) {
    string text;
    text += "{";
    for (int i=0; i<expression->getExpressions().size(); i++) {
        text += toString(expression->getExpressions().at(i));
        if (i < expression->getExpressions().size() - 1)
            text += ", ";
    }
    text += "}";
    return text;
}

string Logger::toString(shared_ptr<ExpressionCall> expression) {
    string argsString;
    for (int i = 0; i < expression->getArgumentExpressions().size(); i++) {
        argsString += toString(expression->getArgumentExpressions().at(i));
        if (i < expression->getArgumentExpressions().size() - 1)
            argsString += ", ";
    }
    return format("CALL({}|{})", expression->getName(), argsString);
}

string Logger::toString(shared_ptr<ExpressionBlock> expression) {
    string text;
    text += toString(expression->getStatementBlock(), { });
    if (!text.empty())
        text += '\n';
    if (expression->getResultStatementExpression() != nullptr)
        text += toString(expression->getResultStatementExpression());
    return text;
}

string Logger::toString(shared_ptr<ExpressionChained> expression) {
    string text;

    /*if (expression->getParentExpression() != nullptr) {
        text += toString(expression->getParentExpression());
        text += ".";
    }
    text += toString(expression->getExpression());*/

    return text;
}

string Logger::formattedLine(string line, vector<IndentKind> indents) {
    string text;

    // Draw tree
    for (IndentKind &indent : indents) {
        switch (indent) {
            case IndentKind::NONE:
                text += "  ";
                break;
            case IndentKind::NODE:
                text += "┣━";
                break;
            case IndentKind::NODE_LAST:
                text += "┗━";
                break;
            case IndentKind::BRANCH:
                text += "┃ ";
                break;
        }
    }

    // And then print the contents
    text += line;
    text += "\n";

    return text;
}

void Logger::print(vector<shared_ptr<Token>> tokens) {
        for (int i=0; i<tokens.size(); i++) {
            cout << i << "|" << toString(tokens.at(i));
            if (i < tokens.size() - 1)
                cout << "  ";
        }
        cout << endl;
}

void Logger::print(shared_ptr<StatementModule> statement) {
    cout << toString(statement, {IndentKind::NODE_LAST}) << endl << endl;
}

void Logger::print(shared_ptr<Error> error) {
    string message;
    switch (error->getKind()) {
        case ErrorKind::LEXER_ERROR: {
            string lexme = error->getLexme() ? *(error->getLexme()) : "";
            message = format("At line {}, column {}: Unexpected token \"{}\"", error->getLine() + 1, error->getColumn() + 1, lexme);
            break;
        }
        case ErrorKind::PARSER_ERROR: {
            shared_ptr<Token> token = error->getActualToken();
            optional<TokenKind> expectedTokenKind = error->getExpectedTokenKind();
            optional<Parsee> expectedParsee = error->getExpectedParsee();
            optional<string> errorMessage = error->getMessage();

            if (expectedParsee) {
                message = format(
                    "At line {}, column {}, Expected {} but found {} instead",
                    token->getLine() + 1, token->getColumn() + 1, toString((*expectedParsee).getKind()), toString(token)
                );
            } else if (expectedTokenKind) {
                message = format(
                    "At line {}, column {}: Expected token {} but found {} instead",
                    token->getLine() + 1, token->getColumn() + 1, toString(*expectedTokenKind), toString(token)
                );
            } else {
                message = format(
                    "At line {}, column {}: Unexpected token \"{}\" found",
                    token->getLine() + 1, token->getColumn() + 1, toString(token)
                );
            }
            if (errorMessage)
                message += format(". {}", *errorMessage);
            break;
        }
        case ErrorKind::BUILDER_ERROR: {
            string errorMessage = error->getMessage() ? *(error->getMessage()) : "";
            message = format("At line {}, column {}: {}", error->getLine(), error->getColumn(), errorMessage);
            break;
        }
    }
    cout << message << endl;
}