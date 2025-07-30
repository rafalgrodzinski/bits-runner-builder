#include "ParseeResultsGroup.h"

#include "ParseeResult.h"

ParseeResultsGroup ParseeResultsGroup::success(vector<ParseeResult> results) {
    ParseeResultsGroup resultsGroup;
    resultsGroup.kind = ParseeResultsGroupKind::SUCCESS;
    resultsGroup.results = results;
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

ParseeResultsGroupKind ParseeResultsGroup::getKind() {
    return kind;
}

vector<ParseeResult> ParseeResultsGroup::getResults() {
    return results;
}