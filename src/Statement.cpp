#include "Statement.h"

Statement::Statement(Kind kind): kind(kind) {
}

string Statement::toString() {
    switch (kind) {
        case INVALID:
            return "INVALID";
    }
}