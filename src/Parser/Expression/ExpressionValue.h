#ifndef EXPRESSION_VALUE_H
#define EXPRESSION_VALUE_H

#include <format>

#include "Expression.h"

enum class ExpressionValueKind {
    BUILT_IN_COUNT,
    BUILT_IN_ADR,
    BUILT_IN_SIZE,
    BUILT_IN_VADR,
    BUILT_IN_VAL_SIMPLE,
    BUILT_IN_VAL_DATA,
    BUILT_IN_TAG,
    DATA,
    ENUM,
    FUN,
    SIMPLE
};

class ExpressionValue: public Expression {
friend class Analyzer;

public:
    static shared_ptr<ExpressionValue> data(const string &identifier, shared_ptr<Expression> indexExpression, shared_ptr<Location> location);
    static shared_ptr<ExpressionValue> enumeration(const string &name, shared_ptr<Location> location);
    static shared_ptr<ExpressionValue> simple(const string &identifer, shared_ptr<Location> location);

    ExpressionValue(const string &identifier, shared_ptr<Location> location);

    ExpressionValueKind getValueKind() const;
    string getIdentifier() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
    shared_ptr<Expression> getIndexExpression() const;

private:
    ExpressionValueKind valueKind;
    string identifier;
    string moduleName;
    shared_ptr<Expression> indexExpression;
};

#endif