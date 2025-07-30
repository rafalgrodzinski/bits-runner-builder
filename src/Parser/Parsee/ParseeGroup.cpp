#include "ParseeGroup.h"

#include "Parsee.h"

ParseeGroup::ParseeGroup(/*bool isRequired, */vector<Parsee> parsees, optional<ParseeGroup> repeatedGroup):
/*isRequired(isRequired), */parsees(parsees) {
    if (repeatedGroup) {
        this->repeatedGroup = *repeatedGroup;
    } else {
        this->repeatedGroup = {};
    }
 }

/*bool ParseeGroup::getIsRequired() {
    return isRequired;
}*/

vector<Parsee> ParseeGroup::getParsees() {
    return parsees;
}

optional<reference_wrapper<ParseeGroup>> ParseeGroup::getRepeatedGroup() {
    return repeatedGroup;
}