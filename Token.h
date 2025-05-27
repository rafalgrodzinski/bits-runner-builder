#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

class Token {
public:
    enum Kind {
        integer,
        real,
        eof
    };

private:
    Kind kind;

public:
    Token(Kind kind);
    std::string toString();
};

#endif