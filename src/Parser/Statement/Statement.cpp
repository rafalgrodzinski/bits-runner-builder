#include "Statement.h"

Statement::Statement(StatementKind kind, int line, int column):
kind(kind), line(line), column(column) { }

StatementKind Statement::getKind() {
    return kind;
}

int Statement::getLine() {
    return line;
}

int Statement::getColumn() {
    return column;
}
