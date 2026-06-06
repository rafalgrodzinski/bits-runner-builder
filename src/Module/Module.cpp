#include "Module.h"

Module:: Module(string name, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements):
name(std::move(name)), headerStatements(std::move(headerStatements)), bodyStatements(std::move(bodyStatements)) { }

string Module::getName() const {
    return name;
}

vector<shared_ptr<Statement>> Module::getHeaderStatements() const {
    return headerStatements;
}

vector<shared_ptr<Statement>> Module::getBodyStatements() const {
    return bodyStatements;
}