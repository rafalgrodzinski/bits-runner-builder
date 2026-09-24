#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <string>
#include <vector>

class Statement;

class Module {
public:
    Module(const std::string &name, const std::vector<std::shared_ptr<Statement>> &headerStatements, const std::vector<std::shared_ptr<Statement>> &bodyStatements);

    std::string getName() const;
    std::vector<std::shared_ptr<Statement>> getHeaderStatements() const;
    std::vector<std::shared_ptr<Statement>> getBodyStatements() const;

private:
    std::string name;
    std::vector<std::shared_ptr<Statement>> headerStatements;
    std::vector<std::shared_ptr<Statement>> bodyStatements;
};

#endif