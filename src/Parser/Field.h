#ifndef FIELD_H
#define FIELD_H

#include <string>

#include "ValueType.h"

using namespace std;

struct Field {
public:
    string name;
    shared_ptr<ValueType> valueType;
};

#endif