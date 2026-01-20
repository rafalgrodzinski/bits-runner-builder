#include "Module.h"

Module:: Module(string name, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements, map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap) :
name(name), headerStatements(headerStatements), bodyStatements(bodyStatements), importableHeaderStatementsMap(importableHeaderStatementsMap) { }

string Module::getName() {
    return name;
}

vector<shared_ptr<Statement>> Module::getHeaderStatements() {
    return headerStatements;
}

vector<shared_ptr<Statement>> Module::getBodyStatements() {
    return bodyStatements;
}

vector<shared_ptr<Statement>> Module::getImportableHeaderStatements(string moduleName) {
    return importableHeaderStatementsMap[moduleName];
}