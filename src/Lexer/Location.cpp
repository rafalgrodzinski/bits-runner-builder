#include "Location.h"

Location::Location(string fileName, int line, int column):
fileName(std::move(fileName)), line(line), column(column) { }

string Location::getFileName() const {
    return fileName;
}

int Location::getLine() const {
    return line;
}

int Location::getColumn() const {
    return column;
}
