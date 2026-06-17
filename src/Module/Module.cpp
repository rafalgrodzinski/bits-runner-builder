#include "Module.h"

Module:: Module(const string &name, const vector<shared_ptr<Statement>> &headerStatements, const vector<shared_ptr<Statement>> &bodyStatements):
name(name), headerStatements(headerStatements), bodyStatements(bodyStatements) { }

string Module::getName() const {
    return name;
}

vector<shared_ptr<Statement>> Module::getHeaderStatements() const {
    return headerStatements;
}

vector<shared_ptr<Statement>> Module::getBodyStatements() const {
    return bodyStatements;
}