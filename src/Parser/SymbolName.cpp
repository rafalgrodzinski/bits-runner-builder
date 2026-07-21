#include "SymbolName.h"

SymbolName::SymbolName(const string &name, const string &moduleName):
name(name), moduleName(moduleName) { }

string SymbolName::getName() const {
    return name;
}

string SymbolName::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string SymbolName::getModuleName() const {
    return moduleName;
}

void SymbolName::setModuleName(const string &moduleName) {
    this->moduleName = moduleName;
}

bool SymbolName::isSubSymbol(shared_ptr<SymbolName> other) const {
    return getGlobalName().starts_with(other->getGlobalName());
}