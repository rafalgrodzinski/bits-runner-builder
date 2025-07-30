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
    static ParseeResultsGroup success(vector<ParseeResult> results);
    static ParseeResultsGroup noMatch();
    static ParseeResultsGroup failure();

    ParseeResultsGroupKind getKind();
    vector<ParseeResult> getResults();
};

#endif