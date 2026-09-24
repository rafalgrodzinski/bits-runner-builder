#include "StatementModule.h"

// MARK: - Public

StatementModule::StatementModule(const string &name, shared_ptr<Location> location):
Statement(StatementKind::MODULE, location), name(name) { }

string StatementModule::getName() const {
    return name;
}
