#include "ParseeGroup.h"

#include "Parsee.h"

ParseeGroup::ParseeGroup(vector<Parsee> parsees):
parsees(parsees) { }

vector<Parsee> ParseeGroup::getParsees() {
    return parsees;
}
