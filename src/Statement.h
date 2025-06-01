#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

using namespace std;

class Statement {
public:
    enum Kind {
        INVALID
    };

private:
    Kind kind;

public:
    Statement(Kind kind);
    string toString();
};

#endif