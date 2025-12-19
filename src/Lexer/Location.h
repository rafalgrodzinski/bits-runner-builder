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
    Location(string fileName, int line, int column);
    string getFileName();
    int getLine();
    int getColumn();
};

#endif