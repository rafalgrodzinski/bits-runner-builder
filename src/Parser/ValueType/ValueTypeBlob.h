#ifndef VALUE_TYPE_BLOB_H
#define VALUE_TYPE_BLOB_H

#include "ValueType.h"

#include <format>
#include <optional>
#include <string>
#include <vector>

using namespace std;

class ValueTypeBlob: public ValueType {
private:
    string name;
    string moduleName;
    optional<vector<shared_ptr<ValueType>>> namedTypeValues;

public:
    ValueTypeBlob(string name, optional<vector<shared_ptr<ValueType>>> namedTypeValues = {});

    string getName() const override;
    string getGlobalName() const override;
    string getModuleName() const;
    void setModuleName(string moduleName);

    bool isEqual(shared_ptr<ValueType> other) const override;
};

#endif