#ifndef MODULE_H
#define MODULE_H

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

public:
    Module(string name, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> bodyStatements);
    string getName();
    vector<shared_ptr<Statement>> getHeaderStatements();
    vector<shared_ptr<Statement>> getBodyStatements();
};

#endif