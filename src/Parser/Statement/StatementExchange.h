#ifndef STATEMENT_EXCHANGE_H
#define STATEMENT_EXCHANGE_H

#include "Statement.h"

class Expression;

class StatementExchange: public Statement {
friend class Analyzer;

public:
    StatementExchange(
        std::shared_ptr<Expression> leftExpression,
        std::shared_ptr<Expression> rightExpression,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<Expression> getLeftExpression();
    std::shared_ptr<Expression> getRightExpression();

private:
    std::shared_ptr<Expression> leftExpression;
    std::shared_ptr<Expression> rightExpression;
};

#endif