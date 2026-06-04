#include "StatementModule.h"

StatementModule::StatementModule(const string & name, shared_ptr<Location> location):
Statement(StatementKind::MODULE, location), name(std::move(name)) { }

string StatementModule::getName() const {
    return name;
}
