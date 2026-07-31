#ifndef SYMBOL_NAME_H
#define SYMBOL_NAME_H

#include <format>
#include <string>

using namespace std;

class SymbolName {
public:
    SymbolName(const string &name, const string &moduleName = "");

    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);

    bool isSubSymbol(shared_ptr<SymbolName> other) const;

    bool operator == (const SymbolName &other) const;
    bool operator < (const SymbolName &other) const;

private:
    string name;
    string moduleName;
};

#endif