#include "Logger.h"

#include <iostream>

#include "Error.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementExpression.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionVariable.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionArrayLiteral.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionBlock.h"

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

        case TokenKind::LEFT_PAREN:
            return "(";
        case TokenKind::RIGHT_PAREN:
            return ")";
        case TokenKind::LEFT_SQUARE_BRACKET:
            return "[";
        case TokenKind::RIGHT_SQUARE_BRACKET:
            return "]";
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

        case TokenKind::IF:
            return "IF";
        case TokenKind::ELSE:
            return "ELSE";
        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RAW_FUNCTION:
            return "RAW";
        case TokenKind::RAW_SOURCE_LINE:
            return format("RAW_SOURCE_LINE({})", token->getLexme());
        case TokenKind::RETURN:
            return "RET";
        case TokenKind::REPEAT:
            return "REP";

        case TokenKind::M_EXTERN:
            return "@EXTERN";

        case TokenKind::NEW_LINE:
            return "↲";
        case TokenKind::END:
            return "END";
    }
}

string Logger::toString(TokenKind tokenKind) {
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

        case TokenKind::LEFT_PAREN:
            return "(";
        case TokenKind::RIGHT_PAREN:
            return ")";
        case TokenKind::LEFT_SQUARE_BRACKET:
            return "[";
        case TokenKind::RIGHT_SQUARE_BRACKET:
            return "]";
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

        case TokenKind::M_EXTERN:
            return "@EXTERN";

        case TokenKind::NEW_LINE:
            return "↲";
        case TokenKind::END:
            return "END";
    }
}

string Logger::toString(shared_ptr<ValueType> valueType) {
    switch (valueType->getKind()) {
        case ValueTypeKind::NONE:
            return "NONE";
        case ValueTypeKind::BOOL:
            return "BOOL";
        case ValueTypeKind::SINT32:
            return "SINT32";
        case ValueTypeKind::REAL32:
            return "REAL32";
        case ValueTypeKind::DATA:
            return "[]";
    }
}

string Logger::toString(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::META_EXTERN_FUNCTION:
            return toString(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
        case StatementKind::VARIABLE:
            return toString(dynamic_pointer_cast<StatementVariable>(statement));
        case StatementKind::FUNCTION:
            return toString(dynamic_pointer_cast<StatementFunction>(statement));
        case StatementKind::RAW_FUNCTION:
            return toString(dynamic_pointer_cast<StatementRawFunction>(statement));
        case StatementKind::BLOCK:
            return toString(dynamic_pointer_cast<StatementBlock>(statement));
        case StatementKind::ASSIGNMENT:
            return toString(dynamic_pointer_cast<StatementAssignment>(statement));
        case StatementKind::RETURN:
            return toString(dynamic_pointer_cast<StatementReturn>(statement));
        case StatementKind::REPEAT:
            return toString(dynamic_pointer_cast<StatementRepeat>(statement));
        case StatementKind::EXPRESSION:
            return toString(dynamic_pointer_cast<StatementExpression>(statement));
    }
}

string Logger::toString(shared_ptr<StatementMetaExternFunction> statement) {
    string text;

    string argsString;
    for (int i = 0; i < statement->getArguments().size(); i++) {
        auto arg = statement->getArguments().at(i);
        argsString +=  format("ARG({}, {})", arg.first, toString(arg.second));
    }
    text += format("@EXTERN FUN(\"{}\"|{}|{})", statement->getName(), argsString, toString(statement->getReturnValueType()));


    return text;
}

string Logger::toString(shared_ptr<StatementVariable> statement) {
    return format("{}({}|{})", statement->getName(), toString(statement->getValueType()), toString(statement->getExpression()));
}

string Logger::toString(shared_ptr<StatementFunction> statement) {
    string text;

    string argsString;
    for (int i = 0; i < statement->getArguments().size(); i++) {
        auto arg = statement->getArguments().at(i);
        argsString +=  format("ARG({}, {})", arg.first, toString(arg.second));
    }
    text += format("FUN(\"{}\"|{}|{}):\n", statement->getName(), argsString, toString(statement->getReturnValueType()));
    text += toString(statement->getStatementBlock());

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

string Logger::toString(shared_ptr<StatementBlock> statement) {
    string text;

    for (int i=0; i<statement->getStatements().size(); i++) {
        text += toString(statement->getStatements().at(i));
        if (i < statement->getStatements().size() - 1)
            text += "\n";
    }

    return text;
}

string Logger::toString(shared_ptr<StatementAssignment> statement) {
    return format("{} <- {}", statement->getName(), toString(statement->getExpression()));
}

string Logger::toString(shared_ptr<StatementReturn> statement) {
    string text = "RET";
    if (statement->getExpression() != nullptr)
        text += format("({})", toString(statement->getExpression()));
    return text;
}

string Logger::toString(shared_ptr<StatementRepeat> statement) {
    string text;

    string initStatement;
    string preCondition;
    string postCondition;

    if (statement->getInitStatement() != nullptr)
        initStatement = toString(statement->getInitStatement());
    
    if (statement->getPostConditionExpression() != nullptr)
        preCondition = toString(statement->getPreConditionExpression());
    
    if (statement->getPostConditionExpression() != nullptr)
        postCondition = toString(statement->getPostConditionExpression());

    text += format("REP({}|{}|{}):\n", initStatement, preCondition, postCondition);
    text += toString(statement->getBodyBlockStatement());

    return text;
}

string Logger::toString(shared_ptr<StatementExpression> statement) {
    return format("EXPR({})", toString(statement->getExpression()));
}

string Logger::toString(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
            return toString(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::IF_ELSE:
            return toString(dynamic_pointer_cast<ExpressionIfElse>(expression));
        case ExpressionKind::VAR:
            return toString(dynamic_pointer_cast<ExpressionVariable>(expression));
        case ExpressionKind::GROUPING:
            return toString(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::LITERAL:
            return toString(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::ARRAY_LITERAL:
            return toString(dynamic_pointer_cast<ExpressionArrayLiteral>(expression));
        case ExpressionKind::CALL:
            return toString(dynamic_pointer_cast<ExpressionCall>(expression));
        case ExpressionKind::BLOCK:
            return toString(dynamic_pointer_cast<ExpressionBlock>(expression));
    }
}

string Logger::toString(shared_ptr<ExpressionBinary> expression) {
    switch (expression->getOperation()) {
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
    string text = format("VAR({}", expression->getName());
    if (expression->getIndexExpression() != nullptr)
        text += format("|{}", toString(expression->getIndexExpression()));
    text += ")";
    return text;
}

string Logger::toString(shared_ptr<ExpressionGrouping> expression) {
    return format("({})", toString(expression->getExpression()));
}

string Logger::toString(shared_ptr<ExpressionLiteral> expression) {
    if (expression->getValueType() == nullptr)
        return "?";

    switch (expression->getValueType()->getKind()) {
        case ValueTypeKind::NONE:
            return "NONE";
        case ValueTypeKind::BOOL:
            return expression->getBoolValue() ? "true" : "false";
        case ValueTypeKind::SINT32:
            return to_string(expression->getSint32Value());
        case ValueTypeKind::REAL32:
            return to_string(expression->getReal32Value());
        default:
            return "?";
    }
}

string Logger::toString(shared_ptr<ExpressionArrayLiteral> expression) {
    string text;
    text += "[";
    for (int i=0; i<expression->getExpressions().size(); i++) {
        text += toString(expression->getExpressions().at(i));
        if (i < expression->getExpressions().size() - 1)
            text += ", ";
    }
    text += "]";
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
    text += toString(expression->getStatementBlock());
    if (!text.empty())
        text += '\n';
    if (expression->getResultStatementExpression() != nullptr)
        text += toString(expression->getResultStatementExpression());
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

void Logger::print(vector<shared_ptr<Statement>> statements) {
    for (shared_ptr<Statement> &statement : statements) {
        cout << toString(statement) << endl << endl;
    }
}

void Logger::print(shared_ptr<Error> error) {
    string message;
    switch (error->getKind()) {
        case ErrorKind::LEXER_ERROR: {
            string lexme = error->getLexme() ? *(error->getLexme()) : "";
            message = format("Unexpected token \"{}\" at line: {}, column: {}", lexme, error->getLine() + 1, error->getColumn() + 1);
            break;
        }
        case ErrorKind::PARSER_ERROR: {
            shared_ptr<Token> token = error->getActualToken();
            optional<TokenKind> expectedTokenKind = error->getExpectedTokenKind();
            optional<string> errorMessage = error->getMessage();

            if (expectedTokenKind) {
                message = format(
                    "Expected token {} but instead found {} at line: {}, column: {}",
                    toString(*expectedTokenKind), toString(token), token->getLine() + 1, token->getColumn() + 1
                );
            } else {
                message = format(
                    "Unexpected token \"{}\" found at line: {}, column: {}",
                    toString(token), token->getLine() + 1, token->getColumn() + 1
                );
            }
            if (errorMessage)
                message += format(". {}", *errorMessage);
            break;
        }
        case ErrorKind::BUILDER_ERROR: {
            string errorMessage = error->getMessage() ? *(error->getMessage()) : "";
            message = format("Error at line {}, column {}: {}", error->getLine(), error->getColumn(), errorMessage);
            break;
        }
    }
    cout << message << endl;
}