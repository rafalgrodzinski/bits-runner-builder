#include "Parser/Statement/StatementMetaExternFunction.h"

StatementMetaExternFunction::StatementMetaExternFunction(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType):
Statement(StatementKind::META_EXTERN_FUNCTION), name(name), arguments(arguments), returnValueType(returnValueType) { }

string StatementMetaExternFunction::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementMetaExternFunction::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementMetaExternFunction::getReturnValueType() {
    return returnValueType;
}
