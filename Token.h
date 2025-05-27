#ifndef TOKEN_H
#define TOKEN_H

class Token {
public:
    enum Kind {
        integer,
        real,
        eof
    };
};

#endif