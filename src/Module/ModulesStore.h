#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Module;
class Statement;
class ValueType;

using namespace std;

class ModulesStore {
private:
    string defaultModuleName;
    vector<string> moduleNames;

    // header
    map<string, vector<shared_ptr<Statement>>> importStatementsMap;
    map<string, vector<shared_ptr<Statement>>> blobDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> blobStatementsMap;
    map<string, vector<shared_ptr<Statement>>> variableDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> functionDeclarationStatementsMap;
    // body
    map<string, vector<shared_ptr<Statement>>> bodyStatementsMap;
    // exported
    map<string, vector<shared_ptr<Statement>>> exportedBlobStatementsMap;
    map<string, vector<shared_ptr<Statement>>> exportedBlobDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> exportedVariableDeclarationStatementsMap;
    map<string, vector<shared_ptr<Statement>>> exportedFunctionDeclarationStatementsMap;

    shared_ptr<ValueType> typeForExportedStatementFromType(shared_ptr<ValueType> valueType, string moduleName);

public:
    ModulesStore(string defaultModuleName);
    void appendStatements(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Module>> getModules();
    map<string, vector<shared_ptr<Statement>>> getExportedHeaderStatementsMap();
};

#endif