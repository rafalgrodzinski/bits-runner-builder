#include "Module.h"

Module:: Module(string name, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements) :
name(name), headerStatements(headerStatements), bodyStatements(bodyStatements) { }

string Module::getName() {
    return name;
}

vector<shared_ptr<Statement>> Module::getHeaderStatements() {
    return headerStatements;
}

vector<shared_ptr<Statement>> Module::getBodyStatements() {
    return bodyStatements;
}