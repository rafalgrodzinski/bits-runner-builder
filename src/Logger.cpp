#include "Logger.h"

#include <iostream>
#include <sstream>

#include "Error.h"
#include "Lexer/Location.h"
#include "Lexer/Token.h"
#include "Parser/Parsee/Parsee.h"
#include "Parser/ValueType.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementMetaImport.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionCast.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionValue.h"

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

        case TokenKind::BIT_OR:
            return "|";
        case TokenKind::BIT_XOR:
            return "^";
        case TokenKind::BIT_AND:
            return "&";
        case TokenKind::BIT_NOT:
            return "~";
        case TokenKind::BIT_SHL:
            return "<<";
        case TokenKind::BIT_SHR:
            return ">>";
        
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
        case TokenKind::FLOAT:
            return "FLOAT(" + token->getLexme() + ")";
        case TokenKind::STRING:
            return "STRING(" + token->getLexme() + ")";
        case TokenKind::IDENTIFIER:
            return "ID(" + token->getLexme() + ")";
        case TokenKind::TYPE:
            return "TYPE(" + token->getLexme() + ")";
        case TokenKind::DATA:
            return "DATA";
        case TokenKind::BLOB:
            return "BLOB";
        case TokenKind::PTR:
            return "PTR";
        case TokenKind::RAW_SOURCE_LINE:
            return format("RAW_SOURCE_LINE({})", token->getLexme());

        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RAW_FUNCTION:
            return "RAW";
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

string Logger::toString(shared_ptr<Statement> statement, vector<IndentKind> indents) {
    switch (statement->getKind()) {
        case StatementKind::MODULE:
            return toString(dynamic_pointer_cast<StatementModule>(statement), indents);
        case StatementKind::META_IMPORT:
            return toString(dynamic_pointer_cast<StatementMetaImport>(statement), indents);
        case StatementKind::META_EXTERN_VARIABLE:
            return toString(dynamic_pointer_cast<StatementMetaExternVariable>(statement), indents);
        case StatementKind::META_EXTERN_FUNCTION:
            return toString(dynamic_pointer_cast<StatementMetaExternFunction>(statement), indents);
        case StatementKind::VARIABLE_DECLARATION:
            return toString(dynamic_pointer_cast<StatementVariableDeclaration>(statement), indents);
        case StatementKind::VARIABLE:
            return toString(dynamic_pointer_cast<StatementVariable>(statement), indents);
        case StatementKind::FUNCTION_DECLARATION:
            return toString(dynamic_pointer_cast<StatementFunctionDeclaration>(statement), indents);
        case StatementKind::FUNCTION:
            return toString(dynamic_pointer_cast<StatementFunction>(statement), indents);
        case StatementKind::RAW_FUNCTION:
            return toString(dynamic_pointer_cast<StatementRawFunction>(statement), indents);
        case StatementKind::BLOB:
            return toString(dynamic_pointer_cast<StatementBlob>(statement), indents);
        case StatementKind::BLOB_DECLARATION:
            return toString(dynamic_pointer_cast<StatementBlobDeclaration>(statement), indents);
        case StatementKind::BLOCK:
            return toString(dynamic_pointer_cast<StatementBlock>(statement), indents);
        case StatementKind::ASSIGNMENT:
            return toString(dynamic_pointer_cast<StatementAssignment>(statement), indents);
        case StatementKind::RETURN:
            return toString(dynamic_pointer_cast<StatementReturn>(statement), indents);
        case StatementKind::REPEAT:
            return toString(dynamic_pointer_cast<StatementRepeat>(statement), indents);
        case StatementKind::EXPRESSION:
            return toString(dynamic_pointer_cast<StatementExpression>(statement), indents);
    }
}

string Logger::toString(shared_ptr<StatementAssignment> statement, vector<IndentKind> indents) {
    string line;

    // left hand
    line += toString(statement->getExpressionChained(), indents, true);

    line += " ← ";

    // right hand
    line += toString(statement->getValueExpression(), indents, true);

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<StatementBlob> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("{}BLOB `{}`", (statement->getShouldExport() ? "@EXPORT " : ""), statement->getName());
    if (!statement->getMembers().empty())
        line += ":";
    text += formattedLine(line, indents);

    // members
    indents = adjustedLastIndent(indents);
    for (pair<string, shared_ptr<ValueType>> &member : statement->getMembers()) {
        line = format("`{}` {}", member.first, toString(member.second));
        text += formattedLine(line, indents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementBlobDeclaration> statement, vector<IndentKind> indents) {
    string line = format ("BLOB DECL `{}`", statement->getName());
    return formattedLine(line, indents);
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

string Logger::toString(shared_ptr<StatementExpression> statement, vector<IndentKind> indents) {
    return toString(statement->getExpression(), indents, false); 
}

string Logger::toString(shared_ptr<StatementFunction> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("{}FUN `{}` → {}", (statement->getShouldExport() ? "@EXPORT " : ""), statement->getName(), toString(statement->getReturnValueType()));
    if (!statement->getArguments().empty())
        line += ":";
    text += formattedLine(line, indents);

    indents = adjustedLastIndent(indents);

    // arguments
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}` {}", arg.first, toString(arg.second));
        text += formattedLine(line, indents);
    }

    // body
    text += toString(statement->getStatementBlock(), indents);

    return text;
}

string Logger::toString(shared_ptr<StatementFunctionDeclaration> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("FUN DECL `{}` → {}", statement->getName(), toString(statement->getReturnValueType()));
    if (!statement->getArguments().empty())
        line += ":";
    text += formattedLine(line, indents);

    indents = adjustedLastIndent(indents);

    // arguments
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}` {}", arg.first, toString(arg.second));
        text += formattedLine(line, indents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementMetaExternFunction> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("@EXTERN FUN `{}` → {}", statement->getName(), toString(statement->getReturnValueType()));
    if (!statement->getArguments().empty())
        line += ":";
    text += formattedLine(line, indents);

    // arguments
    indents = adjustedLastIndent(indents);
    vector<IndentKind> argIndents = indents;
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}` {}", arg.first, toString(arg.second));
        text += formattedLine(line, argIndents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementMetaExternVariable> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("@EXTERN VAR `{}` {}", statement->getIdentifier(), toString(statement->getValueType()));
    text += formattedLine(line, indents);

    return text;
}

string Logger::toString(shared_ptr<StatementMetaImport> statement, vector<IndentKind> indents) {
    string line = format("@IMPORT `{}`", statement->getName());
    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<StatementModule> statement, vector<IndentKind> indents) {
    string text;

    string line = format("MODULE `{}`:", statement->getName());
    text += formattedLine(line, indents);

    indents = adjustedLastIndent(indents);
    
    // header
    indents.push_back(IndentKind::NODE);
    text += formattedLine("HEADER", indents);
    indents.at(indents.size()-1) = IndentKind::BRANCH;

    int headerStatementsCount = statement->getHeaderStatements().size();
    for (int i=0; i<headerStatementsCount; i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < headerStatementsCount - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += toString(statement->getHeaderStatements().at(i), currentIndents);
    }

    // body
    indents.at(indents.size()-1) = IndentKind::NODE_LAST;
    text += formattedLine("BODY", indents);
    indents.at(indents.size()-1) = IndentKind::EMPTY;

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

string Logger::toString(shared_ptr<StatementRawFunction> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("RAW `{}`, {} → {}", statement->getName(), statement->getConstraints(), toString(statement->getReturnValueType()));
    if (!statement->getArguments().empty())
        line += ":";
    text += formattedLine(line, indents);

    indents = adjustedLastIndent(indents);

    // arguments
    for (pair<string, shared_ptr<ValueType>> arg : statement->getArguments()) {
        line = format("`{}` {}", arg.first, toString(arg.second));
        text += formattedLine(line, indents);
    }

    indents = adjustedLastIndent(indents);

    // body
    vector<string> sourceLines;
    stringstream stream(statement->getRawSource());
    for (string sourceLine; getline(stream, sourceLine, '\n');) {
        sourceLines.push_back(sourceLine);
    }

    for (int i=0; i<sourceLines.size(); i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < sourceLines.size() - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += formattedLine(sourceLines.at(i), currentIndents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementRepeat> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = "REP";
    if (
        statement->getInitStatement() != nullptr ||
        statement->getPreConditionExpression() != nullptr ||
        statement->getPostConditionExpression() != nullptr ||
        statement->getPostStatement() != nullptr) {
        line += ":";
    }
    text += formattedLine(line, indents);

    indents = adjustedLastIndent(indents);

    // init-statement
    if (statement->getInitStatement() != nullptr)
        text += toString(statement->getInitStatement(), indents);

    // pre-condition
    if (statement->getPreConditionExpression() != nullptr)
        text += toString(statement->getPreConditionExpression(), indents, false);

    // post-condition
    if (statement->getPostConditionExpression() != nullptr)
        text += toString(statement->getPostConditionExpression(), indents, false);

    // post-statement
        if (statement->getPostStatement() != nullptr)
        text += toString(statement->getPostStatement(), indents);

    // body
    text += toString(statement->getBodyBlockStatement(), indents);

    return text;
}

string Logger::toString(shared_ptr<StatementReturn> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = "RET";
    if (statement->getExpression() != nullptr)
    line += ":";
    text += formattedLine(line, indents);

    // expression
    if (statement->getExpression() != nullptr) {
        indents = adjustedLastIndent(indents);
        text += toString(statement->getExpression(), indents, false);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementVariable> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("{}VAR `{}` {}", (statement->getShouldExport() ? "@EXPORT " : ""), statement->getIdentifier(), toString(statement->getValueType()));
    if (statement->getExpression() != nullptr)
        line += ":";
    text += formattedLine(line, indents);

    // initializer
    if (statement->getExpression() != nullptr) {
        indents = adjustedLastIndent(indents);
        line = format("← {}", toString(statement->getExpression(), indents, true));
        text += formattedLine(line, indents);
    }

    return text;
}

string Logger::toString(shared_ptr<StatementVariableDeclaration> statement, vector<IndentKind> indents) {
    string text;
    string line;

    // name
    line = format("VAR DECL `{}` {}", statement->getIdentifier(), toString(statement->getValueType()));
    text += formattedLine(line, indents);

    return text;
}

string Logger::toString(shared_ptr<Expression> expression, vector<IndentKind> indents, bool isInline) {
    switch (expression->getKind()) {
        case ExpressionKind::NONE:
            return formattedLine("NONE", indents);
        case ExpressionKind::BINARY:
            return toString(dynamic_pointer_cast<ExpressionBinary>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::UNARY:
            return toString(dynamic_pointer_cast<ExpressionUnary>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::IF_ELSE:
            return toString(dynamic_pointer_cast<ExpressionIfElse>(expression), indents, isInline);
        case ExpressionKind::VALUE:
            return toString(dynamic_pointer_cast<ExpressionValue>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::GROUPING:
            return toString(dynamic_pointer_cast<ExpressionGrouping>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::LITERAL:
            return toString(dynamic_pointer_cast<ExpressionLiteral>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::COMPOSITE_LITERAL:
            return toString(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::CALL:
            return toString(dynamic_pointer_cast<ExpressionCall>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::BLOCK:
            return toString(dynamic_pointer_cast<ExpressionBlock>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::CHAINED:
            return toString(dynamic_pointer_cast<ExpressionChained>(expression), isInline ? vector<IndentKind>() : indents);
        case ExpressionKind::CAST:
            return toString(dynamic_pointer_cast<ExpressionCast>(expression), isInline ? vector<IndentKind>() : indents);
    }
}

string Logger::toString(shared_ptr<ExpressionBinary> expression, vector<IndentKind> indents) {
    string op;

    switch (expression->getOperation()) {
        case ExpressionBinaryOperation::OR:
            op = "OR";
            break;
        case ExpressionBinaryOperation::XOR:
            op = "XOR";
            break;
        case ExpressionBinaryOperation::AND:
            op = "AND";
            break;
        case ExpressionBinaryOperation::BIT_OR:
            op = "|";
            break;
        case ExpressionBinaryOperation::BIT_XOR:
            op = "^";
            break;
        case ExpressionBinaryOperation::BIT_AND:
            op = "&";
            break;
        case ExpressionBinaryOperation::BIT_SHL:
            op = "<<";
            break;
        case ExpressionBinaryOperation::BIT_SHR:
            op = ">>";
            break;
        case ExpressionBinaryOperation::EQUAL:
            op = "=";
            break;
        case ExpressionBinaryOperation::NOT_EQUAL:
            op = "!=";
            break;
        case ExpressionBinaryOperation::LESS:
            op = "<";
            break;
        case ExpressionBinaryOperation::LESS_EQUAL:
            op = "<=";
            break;
        case ExpressionBinaryOperation::GREATER:
            op = ">";
            break;
        case ExpressionBinaryOperation::GREATER_EQUAL:
            op = ">=";
            break;
        case ExpressionBinaryOperation::ADD:
            op = "+";
            break;
        case ExpressionBinaryOperation::SUB:
            op = "-";
            break;
        case ExpressionBinaryOperation::MUL:
            op = "*";
            break;
        case ExpressionBinaryOperation::DIV:
            op = "/";
            break;
        case ExpressionBinaryOperation::MOD:
            op = "%";
            break;
    }

    string line = format("({} {} {})", toString(expression->getLeft(), indents, true), op, toString(expression->getRight(), indents, true));

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionBlock> expression, vector<IndentKind> indents) {
    string text;

    indents.push_back(IndentKind::NODE);
    for (shared_ptr<Statement> &statement : expression->getStatementBlock()->getStatements()) {
        text += toString(statement, indents);
    }

    indents.at(indents.size()-1) = IndentKind::NODE_LAST;
    text += toString(expression->getResultStatementExpression(), indents);

    return text;
}

string Logger::toString(shared_ptr<ExpressionCall> expression, vector<IndentKind> indents) {
    string text;

    if (indents.size() > 0) {
        string line;
        line = format("CALL `{}`", expression->getName());
        if (!expression->getArgumentExpressions().empty())
            line += ":";
        text += formattedLine(line, indents);

        indents = adjustedLastIndent(indents);

        for (shared_ptr<Expression> argExpression : expression->getArgumentExpressions())
            text += toString(argExpression, indents, false);
    } else {
        text = format("`{}`(", expression->getName());
        int expressionsCount = expression->getArgumentExpressions().size();
        for (int i=0; i<expressionsCount; i++) {
            text += toString(expression->getArgumentExpressions().at(i), indents, true);
            if (i < expressionsCount-1)
                text += ", ";
        }
        text += ")";
    }

    return text;
}

string Logger::toString(shared_ptr<ExpressionCast> expression, vector<IndentKind> indents) {
    string line = toString(expression->getValueType());
    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionChained> expression, vector<IndentKind> indents) {
    string line;

    int expressionsCount = expression->getChainExpressions().size();
    for (int i=0; i<expressionsCount; i++) {
        line += toString(expression->getChainExpressions().at(i), indents, true);

        // Need to remove the last new line if present (for example block adds it) 
        if (line.at(line.length() - 1) == '\n')
            line = line.substr(0, line.length() - 1);

        if (i < expressionsCount-1)
            line += ".";
    }

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionCompositeLiteral> expression, vector<IndentKind> indents) {
    string line;

    int expressionsCount = expression->getExpressions().size();
    line += "{ ";
    for (int i=0; i<expressionsCount; i++) {
        line += toString(expression->getExpressions().at(i), indents, true);
        if (i < expressionsCount-1)
            line += ",";
        line += " ";
    }
    line += "}";

    line = format("{}｢{}｣", line, toString(expression->getValueType()));

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionGrouping> expression, vector<IndentKind> indents) {
    string line = format("({})", toString(expression->getSubExpression(), indents, true));
    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionIfElse> expression, vector<IndentKind> indents, bool isInline) {
    string text;
    string line;

    // name
    text += formattedLine("IF", isInline ? vector<IndentKind>() : indents);
    // Only the initial IF is inline
    if (isInline)
        text += "\n";
    
    //condition
    indents = adjustedLastIndent(indents);
    text += toString(expression->getConditionExpression(), indents, false);

    // then
    if (expression->getElseExpression() != nullptr)
        indents.push_back(IndentKind::NODE);
    else
        indents.push_back(IndentKind::NODE_LAST);

    text += formattedLine("THEN", indents);
    indents = adjustedLastIndent(indents);
    text += toString(expression->getThenExpression(), indents, false);

    // else
    if (expression->getElseExpression() != nullptr) {
        indents.at(indents.size()-1) = IndentKind::NODE_LAST;
        text += formattedLine("ELSE", indents);
        indents = adjustedLastIndent(indents);
        // expression blocks add node_last themselves
        if (expression->getElseExpression()->getKind() != ExpressionKind::BLOCK)
            indents.push_back(IndentKind::NODE_LAST);
        text += toString(expression->getElseExpression(), indents, false);
    }

    // Need to remove the last new line, since the parent will add it
    if (isInline)
        text = text.substr(0, text.length() - 1);

    return text;
}

string Logger::toString(shared_ptr<ExpressionLiteral> expression, vector<IndentKind> indents) {
    string line;

    switch (expression->getLiteralKind()) {
        case ExpressionLiteralKind::BOOL:
            line = expression->getBoolValue() ? "true" : "false";
            break;
        case ExpressionLiteralKind::FLOAT:
            line = format("{}｢{}｣", expression->getFloatValue(), toString(expression->getValueType()));
            break;
        case ExpressionLiteralKind::INT:
            line = format("{}｢{}｣", expression->getSIntValue(), toString(expression->getValueType()));
            break;
    }

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionUnary> expression, vector<IndentKind> indents) {
    string line;

    switch (expression->getOperation()) {
        case ExpressionUnaryOperation::NOT:
            line = format("NOT {}", toString(expression->getSubExpression(), indents, true));
            break;
        case ExpressionUnaryOperation::BIT_NOT:
            line = format("~{}", toString(expression->getSubExpression(), indents, true));
            break;
        case ExpressionUnaryOperation::PLUS:
            line = format("+({})", toString(expression->getSubExpression(), indents, true));
            break;
        case ExpressionUnaryOperation::MINUS:
            line = format("-({})", toString(expression->getSubExpression(), indents, true));
            break;
    }

    return formattedLine(line, indents);
}

string Logger::toString(shared_ptr<ExpressionValue> expression, vector<IndentKind> indents) {
    string line;

    switch (expression->getValueKind()) {
        case ExpressionValueKind::BUILT_IN_ADR:
        case ExpressionValueKind::BUILT_IN_COUNT:
        case ExpressionValueKind::BUILT_IN_SIZE:
        case ExpressionValueKind::BUILT_IN_VADR:
        case ExpressionValueKind::BUILT_IN_VAL_SIMPLE:
        case ExpressionValueKind::BUILT_IN_VAL_DATA:
            line = format("{}｢{}｣", expression->getIdentifier(), toString(expression->getValueType()));
            break;
        case ExpressionValueKind::DATA:
            line = format("`{}`[{}]｢{}｣", expression->getIdentifier(), toString(expression->getIndexExpression(), indents, true), toString(expression->getValueType()));
            break;
        case ExpressionValueKind::FUN:
            line = format("`{}`", expression->getIdentifier());
            break;
        case ExpressionValueKind::SIMPLE:
            line = format("`{}`｢{}｣", expression->getIdentifier(), toString(expression->getValueType()));
            break;
    }

    return formattedLine(line, indents);
}

string Logger::formattedLine(string line, vector<IndentKind> indents) {
    // Just return the input, if no indents (useful for inline expressions)
    if (indents.empty())
        return line;

    // Otherwise figure out the tree structure
    string text;

    // Draw tree
    for (IndentKind &indent : indents) {
        switch (indent) {
            case IndentKind::ROOT:
                text = "";
                break;
            case IndentKind::EMPTY:
                text += "   ";
                break;
            case IndentKind::NODE:
                text += " ┣━";
                break;
            case IndentKind::NODE_LAST:
                text += " ┗━";
                break;
            case IndentKind::BRANCH:
                text += " ┃ ";
                break;
        }
    }

    // And then print the contents
    text += line;
    text += "\n";

    return text;
}

vector<IndentKind> Logger::adjustedLastIndent(vector<IndentKind> indents) {
    if (indents.empty())
        return indents;

    IndentKind newKind;

    switch (indents.at(indents.size()-1)) {
        case IndentKind::EMPTY:
        case IndentKind::NODE_LAST:
            newKind = IndentKind::EMPTY;
            break;
        case IndentKind::BRANCH:
        case IndentKind::NODE:
            newKind = IndentKind::BRANCH;
            break;
        case IndentKind::ROOT:
            newKind = IndentKind::ROOT;
            break;
    }

    indents.at(indents.size()-1) = newKind;

    return indents;
}

string Logger::toString(Parsee parsee) {
    switch (parsee.getKind()) {
        case ParseeKind::GROUP:
            return "PARSEE_GROUP";
        case ParseeKind::REPEATED_GROUP:
            return "PARSEE_REPEATED_GROUP";
        case ParseeKind::ONE_OF:
            return "PARSEE_ONE_OF";
        case ParseeKind::TOKEN:
            return toString(parsee.getTokenKind());
        case ParseeKind::VALUE_TYPE:
            return "Value Type";
        case ParseeKind::STATEMENT:
            return "Statement";
        case ParseeKind::STATEMENT_IN_BLOCK:
            return "Statement in Block";
        case ParseeKind::EXPRESSION:
            return "Expression";
        case ParseeKind::STATEMENT_BLOCK_SINGLE_LINE:
        case ParseeKind::STATEMENT_BLOCK_MULTI_LINE:
            return "Statement Block";
        case ParseeKind::EXPRESSION_BLOCK_SINGLE_LINE:
        case ParseeKind::EXPRESSION_BLOCK_MULTI_LINE:
            return "Expression Block";
        case ParseeKind::IF_ELSE:
            return "Expression If-Else";
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

        case TokenKind::OR:
            return "OR";
        case TokenKind::XOR:
            return "XOR";
        case TokenKind::AND:
            return "AND";
        case TokenKind::NOT:
            return "NOT";

        case TokenKind::BIT_OR:
            return "|";
        case TokenKind::BIT_XOR:
            return "^";
        case TokenKind::BIT_AND:
            return "&";
        case TokenKind::BIT_NOT:
            return "~";
        case TokenKind::BIT_SHL:
            return "<<";
        case TokenKind::BIT_SHR:
            return ">>";
        
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
        case TokenKind::FLOAT:
            return "LITERAL(FLOAT)";
        case TokenKind::STRING:
            return "LITERAL(STRING)";
        case TokenKind::IDENTIFIER:
            return "LITERAL(ID)";
        case TokenKind::TYPE:
            return "TYPE";
        case TokenKind::DATA:
            return "DATA";
        case TokenKind::BLOB:
            return "BLOB";
        case TokenKind::PTR:
            return "PTR";
        case TokenKind::RAW_SOURCE_LINE:
            return "RAW_SOURCE_LINE";

        case TokenKind::FUNCTION:
            return "FUN";
        case TokenKind::RAW_FUNCTION:
            return "RAW";
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

void Logger::print(vector<shared_ptr<Token>> tokens) {
        for (int i=0; i<tokens.size(); i++) {
            cout << i << "|" << toString(tokens.at(i));
            if (i < tokens.size() - 1)
                cout << "  ";
        }
        cout << endl;
}

string Logger::toString(ExpressionUnaryOperation operationUnary) {
    switch (operationUnary) {
        case ExpressionUnaryOperation::NOT:
            return "NOT";
        case ExpressionUnaryOperation::BIT_NOT:
            return "BIT_NOT";
        case ExpressionUnaryOperation::PLUS:
            return "PLUS";
        case ExpressionUnaryOperation::MINUS:
            return "MINUS";
    }
}

//
// Public
//

void Logger::printModuleStatements(string moduleName, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements) {
    string text;

    text += format("MODULE `{}`:\n", moduleName);
    vector<IndentKind> indents = {IndentKind::ROOT};
    
    // header
    indents.push_back(IndentKind::NODE);
    text += formattedLine("HEADER", indents);
    indents.at(indents.size()-1) = IndentKind::BRANCH;


    for (int i=0; i<headerStatements.size(); i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < headerStatements.size() - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += toString(headerStatements.at(i), currentIndents);
    }

    // body
    indents.at(indents.size()-1) = IndentKind::NODE_LAST;
    text += formattedLine("BODY", indents);
    indents.at(indents.size()-1) = IndentKind::EMPTY;

    for (int i=0; i<bodyStatements.size(); i++) {
        vector<IndentKind> currentIndents = indents;
        if (i < bodyStatements.size() - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += toString(bodyStatements.at(i), currentIndents);
    }

    cout << text;
}

void Logger::printExportedStatements(string moduleName, vector<shared_ptr<Statement>> statments) {
    string text;

    text += format("EXPORTED STATEMENTS `{}`:\n", moduleName);

    int statementsCount = statments.size();
    for (int i=0; i<statementsCount; i++) {
        vector<IndentKind> currentIndents = {IndentKind::ROOT};
        if (i < statementsCount - 1)
            currentIndents.push_back(IndentKind::NODE);
        else
            currentIndents.push_back(IndentKind::NODE_LAST);

        text += toString(statments.at(i), currentIndents);
    }

    cout << text;
}

void Logger::print(shared_ptr<Error> error) {
    string message;
    switch (error->getKind()) {
        case ErrorKind::MESSAGE: {
            string errorMessage = *(error->getMessage());
            message = format("🔥 In {}: {}", toString(error->getLocation()), errorMessage);
            break;
        }
        case ErrorKind::LEXER_ERROR: {
            string lexme = error->getLexme() ? *(error->getLexme()) : "";
            message = format("🔥 In {}: Unexpected token \"{}\"", toString(error->getLocation()), lexme);
            break;
        }
        case ErrorKind::PARSER_ERROR: {
            shared_ptr<Token> token = error->getActualToken();
            optional<TokenKind> expectedTokenKind = error->getExpectedTokenKind();
            optional<Parsee> expectedParsee = error->getExpectedParsee();
            optional<string> errorMessage = error->getMessage();

            if (expectedParsee) {
                message = format(
                    "🔥 In {}: Expected parsee {} but found {} instead",
                    toString(token->getLocation()),
                    toString((*expectedParsee)), 
                    toString(token)
                );
            } else if (expectedTokenKind) {
                message = format(
                    "🔥 In {}: Expected token {} but found {} instead",
                    toString(token->getLocation()),
                    toString(*expectedTokenKind),
                    toString(token)
                );
            } else {
                message = format(
                    "🔥 In {}: Unexpected token {} found",
                    toString(token->getLocation()),
                    toString(token)
                );
            }
            if (errorMessage)
                message += format(". {}", *errorMessage);
            break;
        }
        case ErrorKind::BUILDER_ERROR: {
            int line = *(error->getLine()) + 1;
            int column = *(error->getColumn()) + 1;
            string errorMessage = *(error->getMessage());
            message = format("🔥 At line {}, column {}: {}", line, column, errorMessage);
            break;
        }
        case ErrorKind::BUILDER_FUNCTION_ERROR: {
            string functionName = *(error->getFunctionName());
            string errorMessage = *(error->getMessage());
            message = format("🔥 Building function \"{}\" failed: {}", functionName, errorMessage);
            break;
        }
        case ErrorKind::BUILDER_MODULE_ERROR: {
            string moduleName = *(error->getModuleName());
            string errorMessage = *(error->getMessage());
            message = format("🔥 Building module \"{}\" failed: {}", moduleName, errorMessage);
            break;
        }
    }
    cout << message << endl;
}

string Logger::toString(shared_ptr<Location> location) {
    if (location != nullptr) {
        string fileName = location->getFileName();
        int line = location->getLine() + 1;
        int column = location->getColumn() + 1;
        return format("file {}, line {}, column {}", fileName, line, column);
    } else {
        return "{UNKNOWN LOCATION}";
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
        case ValueTypeKind::INT:
            return "INT";
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
        case ValueTypeKind::FLOAT:
            return "FLOAT";
        case ValueTypeKind::F32:
            return "F32";
        case ValueTypeKind::F64:
            return "F64";
        case ValueTypeKind::DATA: {
            if (valueType->getCountExpression() != nullptr)
                return format("DATA<{}, {}>", toString(valueType->getSubType()), toString(valueType->getCountExpression(), {}, false));
            else
                return format("DATA<{}>", toString(valueType->getSubType()));
        }
        case ValueTypeKind::BLOB:
            return format("BLOB<`{}`>", *(valueType->getBlobName()));
        case ValueTypeKind::FUN: {
            string text = "FUN";
            // args
            vector<shared_ptr<ValueType>> argumentTypes = *(valueType->getArgumentTypes());
            for (int i=0; i<argumentTypes.size(); i++) {
                if (i > 0)
                    text += ",";
                text += format(" {}", toString(argumentTypes.at(i)));
            }
            // return
            if (valueType->getReturnType() != nullptr)
                text += format(" -> {}", toString(valueType->getReturnType()));
            return text;
        }
        case ValueTypeKind::PTR:
            return format("PTR<{}>", toString(valueType->getSubType()));
        case ValueTypeKind::COMPOSITE:
            return format("COMPOSITE");
    }
}

string Logger::toString(ExpressionBinaryOperation operationBinary) {
    switch (operationBinary) {
        case ExpressionBinaryOperation::OR:
            return "OR";
        case ExpressionBinaryOperation::XOR:
            return "XOR";
        case ExpressionBinaryOperation::AND:
            return "AND";

        case ExpressionBinaryOperation::BIT_OR:
            return "BIT_OR";
        case ExpressionBinaryOperation::BIT_XOR:
            return "BIT_XOR";
        case ExpressionBinaryOperation::BIT_AND:
            return "BIT_AND";
        case ExpressionBinaryOperation::BIT_SHL:
            return "BIT_SHL";
        case ExpressionBinaryOperation::BIT_SHR:
            return "BIT_SHR";

        case ExpressionBinaryOperation::EQUAL:
            return "=";
        case ExpressionBinaryOperation::NOT_EQUAL:
            return "!=";
        case ExpressionBinaryOperation::LESS:
            return "<";
        case ExpressionBinaryOperation::LESS_EQUAL:
            return "<=";
        case ExpressionBinaryOperation::GREATER:
            return ">";
        case ExpressionBinaryOperation::GREATER_EQUAL:
            return ">=";

        case ExpressionBinaryOperation::ADD:
            return "+";
        case ExpressionBinaryOperation::SUB:
            return "-";
        case ExpressionBinaryOperation::MUL:
            return "*";
        case ExpressionBinaryOperation::DIV:
            return "/";
        case ExpressionBinaryOperation::MOD:
            return "%";
    }
}