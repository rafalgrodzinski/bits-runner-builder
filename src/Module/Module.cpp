#include "Module.h"

using namespace std;

// MARK: - Public

Module:: Module(const string &name, const vector<shared_ptr<Statement>> &statements):
name(name),
statements(statements) { }

string Module::getName() const {
    return name;
}

vector<shared_ptr<Statement>> Module::getStatements() const {
    return statements;
}