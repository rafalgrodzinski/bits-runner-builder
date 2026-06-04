#include "ParseeResultsGroup.h"

#include "ParseeResult.h"

ParseeResultsGroup ParseeResultsGroup::success(const vector<ParseeResult> &results) {
    ParseeResultsGroup resultsGroup;
    resultsGroup.kind = ParseeResultsGroupKind::SUCCESS;
    resultsGroup.results = std::move(results);
    return resultsGroup;
}

ParseeResultsGroup ParseeResultsGroup::noMatch() {
    ParseeResultsGroup resultsGroup;
    resultsGroup.kind = ParseeResultsGroupKind::NO_MATCH;
    return resultsGroup;
}

ParseeResultsGroup ParseeResultsGroup::failure() {
    ParseeResultsGroup resultsGroup;
    resultsGroup.kind = ParseeResultsGroupKind::FAILURE;
    return resultsGroup;
}

ParseeResultsGroupKind ParseeResultsGroup::getKind() const {
    return kind;
}

vector<ParseeResult> ParseeResultsGroup::getResults() const {
    return results;
}