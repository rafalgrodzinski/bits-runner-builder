#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <string>
#include <vector>

class Statement;

class Module {
public:
    Module(const std::string &name, const std::vector<std::shared_ptr<Statement>> &statements);

    std::string getName() const;
    std::vector<std::shared_ptr<Statement>> getStatements() const;

private:
    std::string name;
    std::vector<std::shared_ptr<Statement>> statements;
};

#endif