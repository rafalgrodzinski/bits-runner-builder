#ifndef PARSEE_GROUP_H
#define PARSEE_GROUP_H

#include <vector>
#include <optional>

class Parsee;

using namespace std;

class ParseeGroup {
private:
    vector<Parsee> parsees;
    optional<reference_wrapper<ParseeGroup>> repeatedGroup;

public:
    ParseeGroup(vector<Parsee> parsees, optional<ParseeGroup> repeatedGroup);
    vector<Parsee> getParsees();
    optional<reference_wrapper<ParseeGroup>> getRepeatedGroup();
};

#endif