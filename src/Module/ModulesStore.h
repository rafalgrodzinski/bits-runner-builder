#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

class Module;
class Statement;

using namespace std;

class ModulesStore {
private:
    string defaultModuleName;
    vector<string> moduleNames;
    map<string, vector<shared_ptr<Statement>>> bodyStatementsMap;
    map<string, vector<shared_ptr<Statement>>> blobStatementsMap;
    map<string, vector<shared_ptr<Statement>>> blobDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> functionDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> variableDeclarationStatementsMap;

public:
    ModulesStore(string defaultModuleName);
    void appendStatements(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Module>> getModules();
};

#endif