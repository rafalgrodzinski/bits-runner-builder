#include "SymbolName.h"

SymbolName::SymbolName(const string &name, const string &moduleName) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->name = name.substr(pos + 1, name.length());
        this->moduleName = name.substr(0, pos);
    } else {
        this->name = name;
        this->moduleName = moduleName;
    }
}

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
    if (moduleName.empty())
        this->moduleName = moduleName;
}

bool SymbolName::isSubSymbol(shared_ptr<SymbolName> other) const {
    return getGlobalName().starts_with(other->getGlobalName());
}

bool SymbolName::operator == (const SymbolName &other) const {
    return getGlobalName() == other.getGlobalName();
}

bool SymbolName::operator < (const SymbolName &other) const {
    return getGlobalName() < other.getGlobalName();
}