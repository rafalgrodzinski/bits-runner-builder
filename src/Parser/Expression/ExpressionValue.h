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
    static std::shared_ptr<ExpressionValue> data(const std::string &identifier, std::shared_ptr<Expression> indexExpression, std::shared_ptr<Location> location);
    static std::shared_ptr<ExpressionValue> enumeration(const std::string &name, std::shared_ptr<Location> location);
    static std::shared_ptr<ExpressionValue> simple(const std::string &identifer, std::shared_ptr<Location> location);

    ExpressionValue(const std::string &identifier, std::shared_ptr<Location> location);

    ExpressionValueKind getValueKind() const;
    std::string getIdentifier() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::shared_ptr<Expression> getIndexExpression() const;

private:
    ExpressionValueKind valueKind;
    std::string identifier;
    std::string moduleName;
    std::shared_ptr<Expression> indexExpression;
};

#endif