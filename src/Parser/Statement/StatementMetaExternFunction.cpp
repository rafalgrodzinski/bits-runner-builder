#include "Parser/Statement/StatementMetaExternFunction.h"

static string valueTypeToString(ValueType valueType) {
    switch (valueType) {
        case ValueType::NONE:
            return "NONE";
        case ValueType::BOOL:
            return "BOOL";
        case ValueType::SINT32:
            return "SINT32";
        case ValueType::REAL32:
            return "REAL32";
    }
}

StatementMetaExternFunction::StatementMetaExternFunction(string name, vector<pair<string, ValueType>> arguments, ValueType returnValueType):
Statement(StatementKind::META_EXTERN_FUNCTION), name(name), arguments(arguments), returnValueType(returnValueType) { }

string StatementMetaExternFunction::getName() {
    return name;
}

vector<pair<string, ValueType>> StatementMetaExternFunction::getArguments() {
    return arguments;
}

ValueType StatementMetaExternFunction::getReturnValueType() {
    return returnValueType;
}

string StatementMetaExternFunction::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "EXTERN_FUN(";
    value += name + ", ";
    value += valueTypeToString(returnValueType);
    value += ")\n";
    return value;
}