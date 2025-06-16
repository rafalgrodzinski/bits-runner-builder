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
    COMMA,
    COLON,
    SEMICOLON,
    QUESTION,
    QUESTION_QUESTION,
    LEFT_ARROW,
    RIGHT_ARROW,

    FUNCTION,
    RETURN,

    BOOL,
    INTEGER,
    REAL,
    IDENTIFIER,
    TYPE,

    NEW_LINE,
    END,

    INVALID
};

enum class ExpressionKind {
    LITERAL,
    GROUPING,
    BINARY,
    IF_ELSE,
    VAR,
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
    NONE,
    BOOL,
    SINT32,
    REAL32
};

#endif