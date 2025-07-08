#include "Utils.h"

optional<int> Utils::charStringToInt(string charString) {
    switch (charString.length()) {
        case 1:
            return charString[0];
        case 3:
            return charString[1];
        case 4:
            charString[0] = charString[1];
            charString[1] = charString[2];
        case 2:
            if (charString[0] != '\\')
                return {};
            switch (charString[1]) {
                case 'b':
                    return '\b';
                case 'n':
                    return '\n';
                case 't':
                    return '\t';
                case '\\':
                    return '\\';
                case '\'':
                    return '\'';
                case '\"':
                    return '\"';
                default:
                    return {};
            }
        default:
            return {};
    }
}