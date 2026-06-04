#ifndef LOCATION_H
#define LOCATION_H

#include <string>

using namespace std;

class Location {
private:
    string fileName;
    int line;
    int column;

public:
    Location(const string &fileName, int line, int column);
    string getFileName() const;
    int getLine() const;
    int getColumn() const;
};

#endif