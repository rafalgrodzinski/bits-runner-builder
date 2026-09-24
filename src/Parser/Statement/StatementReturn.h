#ifndef STATEMENT_RETURN
#define STATEMENT_RETURN

#include "Parser/Statement/Statement.h"

class Expression;

class StatementReturn: public Statement {
friend class Analyzer;

public:
    StatementReturn(std::shared_ptr<Expression> expression, std::shared_ptr<Location> location);

    std::shared_ptr<Expression> getExpression() const;

private:
    std::shared_ptr<Expression> expression;
};

#endif