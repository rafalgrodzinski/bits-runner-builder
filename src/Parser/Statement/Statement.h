#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Location;

enum class StatementKind {
    ASSIGNMENT,
    BLOB,
    BLOB_DECLARATION,
    BLOCK,
    ENUM,
    EXPRESSION,
    FUNCTION,
    FUNCTION_DECLARATION,
    META_EXTERN_FUNCTION,
    META_EXTERN_VARIABLE,
    META_IMPORT,
    MODULE,
    PROTO,
    PROTO_DECLARATION,
    RAW_FUNCTION,
    REPEAT,
    RETURN,
    VARIABLE,
    VARIABLE_DECLARATION
};

class Statement {
public:
    Statement(StatementKind kind, std::shared_ptr<Location> location);
    virtual ~Statement() { }

    StatementKind getKind() const;
    std::shared_ptr<Location> getLocation() const;

private:
    StatementKind kind;
    std::shared_ptr<Location> location;
};

#endif