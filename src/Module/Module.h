#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <string>
#include <vector>

class Statement;

using namespace std;

class Module {
public:
    Module(vector<shared_ptr<Statement>> statements);

    string getName();
    vector<shared_ptr<Statement>> getStatements();
    vector<shared_ptr<Statement>> getHeaderStatements();
    vector<shared_ptr<Statement>> getExportedHeaderStatements(string moduleName);
};

#endif