#ifndef STATEMENT_VARIABLE_DECLARATION_H
#define STATEMENT_VARIABLE_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;

class StatementVariableDeclaration: public Statement {
friend class Analyzer;

private:
    bool shouldExport;
    string identifier;
    string moduleName;
    bool isRoot;
    shared_ptr<ValueType> valueType;

public:
    StatementVariableDeclaration(
        bool shouldExport,
        const string &identifier,
        const string &moduleName,
        bool isRoot,
        shared_ptr<ValueType> valueType,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getIdentifier() const;
    string getGlobalIdentifier() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
    bool getIsRoot() const;
    shared_ptr<ValueType> getValueType() const;
};

#endif