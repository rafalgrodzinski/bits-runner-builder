#include "Parser/Statement/Statement.h"

class Token;

class StatementInvalid: public Statement {
private:
    shared_ptr<Token> token;
    string message;

public:
    StatementInvalid(shared_ptr<Token> token, string message);
    string toString(int indent) override;
    string getMessage();
};
