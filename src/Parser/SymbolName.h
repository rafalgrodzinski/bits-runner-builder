#ifndef SYMBOL_NAME_H
#define SYMBOL_NAME_H

#include <format>
#include <memory>
#include <string>

class SymbolName {
public:
    SymbolName(const std::string &name, const std::string &moduleName = "");

    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);

    bool isSubSymbol(std::shared_ptr<SymbolName> other) const;

    bool isEqual(std::shared_ptr<SymbolName> other) const;
    bool operator == (const SymbolName &other) const;
    bool operator < (const SymbolName &other) const;

private:
    std::string name;
    std::string moduleName;
};

#endif