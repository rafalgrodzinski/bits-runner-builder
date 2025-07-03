#include "Statement.h"

Statement::Statement(StatementKind kind):
kind(kind) { }

StatementKind Statement::getKind() {
    return kind;
}
