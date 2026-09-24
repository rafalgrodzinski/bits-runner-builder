#ifndef EXPRESSION_CALL_H
#define EXPRESSION_CALL_H

#include <format>

#include "Expression.h"

class ExpressionCall: public Expression {
friend class Analyzer;

public:
    ExpressionCall(const std::string &name, const std::vector<std::shared_ptr<Expression>> &argumentExpressions, std::shared_ptr<Location> location);

    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);

    std::vector<std::shared_ptr<Expression>> getArgumentExpressions() const;

private:
    std::string name;
    std::string moduleName;
    std::vector<std::shared_ptr<Expression>> argumentExpressions;
};

#endif