#include "ParseeGroup.h"

#include "Parsee.h"

ParseeGroup::ParseeGroup(vector<Parsee> parsees)://, optional<ParseeGroup> repeatedGroup):
parsees(parsees) {
    /*if (repeatedGroup) {
        this->repeatedGroup = *repeatedGroup;
    } else {
        this->repeatedGroup = {};
    }*/
 }

vector<Parsee> ParseeGroup::getParsees() {
    return parsees;
}

/*optional<reference_wrapper<ParseeGroup>> ParseeGroup::getRepeatedGroup() {
    return repeatedGroup;
}*/