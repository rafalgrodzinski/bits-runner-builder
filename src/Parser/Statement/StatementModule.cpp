#include "StatementModule.h"

StatementModule::StatementModule(string name, shared_ptr<Location> location) :
Statement(StatementKind::MODULE, location), name(name) { }

string StatementModule::getName() {
    return name;
}
