#ifndef PARSEE_RESULTS_GROUP_H
#define PARSEE_RESULTS_GROUP_H

#include <vector>

class ParseeResult;

using namespace std;

enum class ParseeResultsGroupKind {
    SUCCESS,
    NO_MATCH,
    FAILURE
};

class ParseeResultsGroup {
private:
    ParseeResultsGroupKind kind;
    vector<ParseeResult> results;

public:
    static ParseeResultsGroup success(const vector<ParseeResult> &results);
    static ParseeResultsGroup noMatch();
    static ParseeResultsGroup failure();

    ParseeResultsGroupKind getKind() const;
    vector<ParseeResult> getResults() const;
};

#endif