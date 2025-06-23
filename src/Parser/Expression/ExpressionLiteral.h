#include "Parser/Expression.h"

#include <iostream>

using namespace std;

class ExpressionLiteral: public Expression {
private:
    bool boolValue;
    int32_t sint32Value;
    float real32Value;
    
    ExpressionLiteral();

public:
    static shared_ptr<ExpressionLiteral> none;

    ExpressionLiteral(shared_ptr<Token> token);
    bool getBoolValue();
    int32_t getSint32Value();
    float getReal32Value();
    string toString(int indent) override;
};