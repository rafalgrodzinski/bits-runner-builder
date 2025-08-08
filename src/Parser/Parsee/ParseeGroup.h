#ifndef PARSEE_GROUP_H
#define PARSEE_GROUP_H

#include <vector>

class Parsee;

using namespace std;

class ParseeGroup {
private:
    vector<Parsee> parsees;

public:
    ParseeGroup(vector<Parsee> parsees);
    vector<Parsee> getParsees();
};

#endif