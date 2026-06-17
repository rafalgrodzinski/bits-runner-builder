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
    Module(const string &name, const vector<shared_ptr<Statement>> &headerStatements, const vector<shared_ptr<Statement>> &bodyStatements);

    string getName() const;
    vector<shared_ptr<Statement>> getHeaderStatements() const;
    vector<shared_ptr<Statement>> getBodyStatements() const;
};

#endif