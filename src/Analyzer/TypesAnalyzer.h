#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>

class StatementModule;

using namespace std;

class TypesAnalyzer {
public:
    void checkModule(shared_ptr<StatementModule> module);
};

#endif