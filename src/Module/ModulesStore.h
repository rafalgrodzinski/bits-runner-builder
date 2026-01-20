#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <memory>
#include <vector>

class Module;

using namespace std;

class ModulesStore {
public:
    void appendModule(shared_ptr<Module> module);
    vector<shared_ptr<Module>> getModules();
};

#endif