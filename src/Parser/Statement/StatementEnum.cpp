#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    shared_ptr<Location> Location
):
Statement(StatementKind::ENUM, Location),
shouldExport(shouldExport) {
    
}