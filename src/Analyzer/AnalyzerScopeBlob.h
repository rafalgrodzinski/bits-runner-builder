#ifndef ANALYZER_SCOPE_BLOB_H
#define ANALYZER_SCOPE_BLOB_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "AnalyzerScope.h"
#include "Parser/SymbolName.h"

using namespace std;

class AnalyzerScopeBlob {
public:
    AnalyzerScopeBlob(AnalyzerScope *parent);

    //optional<vector<string>> getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName);
    optional<vector<string>> getNamedValueTypeKeys(string symbolName);
    //void registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys);
    void registerNamedValueTypeKeys(string symbolName, const vector<string> &namedValueTypeKeys);

    optional<vector<shared_ptr<ValueType>>> getFieldVariableValueTypes(string symbolName);

private:
    AnalyzerScope *parent;
};

#endif