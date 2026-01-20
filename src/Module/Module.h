#ifndef MODULE_H
#define MODULE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

class Statement;

using namespace std;

class Module {
private:
    string name;
    vector<shared_ptr<Statement>> headerStatements;
    vector<shared_ptr<Statement>> bodyStatements;
    map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap;

public:
    Module(string name, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements, map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap);
    string getName();
    vector<shared_ptr<Statement>> getHeaderStatements();
    vector<shared_ptr<Statement>> getBodyStatements();
    vector<shared_ptr<Statement>> getImportableHeaderStatements(string moduleName);
};

#endif