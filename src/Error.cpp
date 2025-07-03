#include "Error.h"

Error::Error(int line, int column, string lexme) :
line(line), column(column), lexme(lexme) { }

int Error::getLine() {
    return line;
}

int Error::getColumn() {
    return column;
}

string Error::getLexme() {
    return lexme;
}