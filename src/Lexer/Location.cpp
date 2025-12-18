#include "Location.h"

Location::Location(string fileName, int line, int column):
fileName(fileName), line(line), column(column) { }

string Location::getFileName() {
    return fileName;
}

int Location::getLine() {
    return line;
}

int Location::getColumn() {
    return column;
}
