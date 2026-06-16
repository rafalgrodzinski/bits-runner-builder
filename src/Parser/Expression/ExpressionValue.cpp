#include "ExpressionValue.h"

shared_ptr<ExpressionValue> ExpressionValue::simple(const string &identifier, shared_ptr<Location> location) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(identifier, location);
    expression->valueKind = ExpressionValueKind::SIMPLE;
    return expression;
}

shared_ptr<ExpressionValue> ExpressionValue::data(const string &identifier, shared_ptr<Expression> indexExpression, shared_ptr<Location> location) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(identifier, location);
    expression->valueKind = ExpressionValueKind::DATA;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionValue::ExpressionValue(const string &identifier, shared_ptr<Location> location):
Expression(ExpressionKind::VALUE, nullptr, location) {
    size_t pos = identifier.find('.');
    if (pos != string::npos) {
        this->moduleName = identifier.substr(0, pos);
        this->identifier = identifier.substr(pos + 1, identifier.size());
    } else {
        this->identifier = identifier;
    }
}

ExpressionValueKind ExpressionValue::getValueKind() const {
    return valueKind;
}

string ExpressionValue::getIdentifier() const {
    if (moduleName.empty()) {
        return identifier;
    } else {
        return format("{}.{}", moduleName, identifier);
    }
}

string ExpressionValue::getModuleName() const {
    return moduleName;
}

void ExpressionValue::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

shared_ptr<Expression> ExpressionValue::getIndexExpression() const {
    return indexExpression;
}