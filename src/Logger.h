#ifndef LOGGER_H
#define LOGGER_H

#include <vector>

class Token;

using namespace std;

class Logger {
private:
    static string toString(shared_ptr<Token> token);

public:
    static void print(vector<shared_ptr<Token>> tokens);
};

#endif