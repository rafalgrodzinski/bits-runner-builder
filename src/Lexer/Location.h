#ifndef LOCATION_H
#define LOCATION_H

#include <string>

class Location {
public:
    Location(const std::string &fileName, int line, int column);

    std::string getFileName() const;
    int getLine() const;
    int getColumn() const;

private:
    std::string fileName;
    int line;
    int column;
};

#endif