#ifndef ERROR_H
#define ERROR_H

#include <iostream>

using namespace std;

class Error {
private:
    int line;
    int column;
    string lexme;

public:
    Error(int line, int column, string lexme);
    int getLine();
    int getColumn();
    string getLexme();
};

#endif