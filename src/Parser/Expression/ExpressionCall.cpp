#include "ExpressionCall.h"

ExpressionCall::ExpressionCall(const string &name, vector<shared_ptr<Expression>> argumentExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CALL, nullptr, location), argumentExpressions(std::move(argumentExpressions)) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.size());
    } else {
        this->name = name;
    }
}

string ExpressionCall::getName() const {
    return name;
}

string ExpressionCall::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string ExpressionCall::getModuleName() const {
    return moduleName;
}

void ExpressionCall::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() const {
    return argumentExpressions;
}
