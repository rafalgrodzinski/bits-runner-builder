#include "Statement.h"

#include "Lexer/Location.h"

Statement::Statement(StatementKind kind, shared_ptr<Location> location):
kind(kind), location(location) { }

StatementKind Statement::getKind() const {
    return kind;
}

shared_ptr<Location> Statement::getLocation() const {
    return location;
}
