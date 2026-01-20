#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <memory>
#include <vector>

class Module;
class Statement;

using namespace std;

class ModulesStore {
public:
    void appendStatements(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Module>> getModules();
};

#endif