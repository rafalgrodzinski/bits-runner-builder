#ifndef PARSEE_RESULTS_GROUP_H
#define PARSEE_RESULTS_GROUP_H

#include <vector>

class ParseeResult;

enum class ParseeResultsGroupKind {
    SUCCESS,
    NO_MATCH,
    FAILURE
};

class ParseeResultsGroup {
public:
    static ParseeResultsGroup success(const std::vector<ParseeResult> &results);
    static ParseeResultsGroup noMatch();
    static ParseeResultsGroup failure();

    ParseeResultsGroupKind getKind() const;
    std::vector<ParseeResult> getResults() const;

private:
    ParseeResultsGroupKind kind;
    std::vector<ParseeResult> results;
};

#endif