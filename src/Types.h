#ifndef TYPES_H
#define TYPES_H

enum class TokenKind {        
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    
    LEFT_PAREN,
    RIGHT_PAREN,
    COLON,
    SEMICOLON,
    QUESTION,
    QUESTION_QUESTION,

    FUNCTION,
    RETURN,

    BOOL,
    INTEGER,
    REAL,
    IDENTIFIER,

    NEW_LINE,
    END,

    INVALID
};

enum class ExpressionKind {
    LITERAL,
    GROUPING,
    BINARY,
    IF_ELSE,
    INVALID
};

enum class StatementKind {
    FUNCTION_DECLARATION,
    VAR_DECLARATION,
    BLOCK,
    RETURN,
    EXPRESSION,
    INVALID
};

enum class ValueType {
    VOID,
    BOOL,
    SINT32,
    REAL32
};

#endif