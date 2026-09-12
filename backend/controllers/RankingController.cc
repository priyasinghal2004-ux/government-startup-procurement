#include "RankingController.h"
#include "../ranking/RankingEngine.h"
#include <json/json.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * getRankings triggers the transparent deterministic ranking calculation.
 * 1. Executes RankingEngine::computeAndPersistRankings synchronously for the project.
 * 2. Filters only ELIGIBLE startups.
 * 3. Normalizes metrics (best cost gets 100, best delivery gets 100).
 * 4. Multiplies by dynamic project weights and sorts with std::sort descending.
 * 5. Returns the ranked leaderboard with ranks, scores, and IDs.
 */
void RankingController::getRankings(const drogon::HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                     int projectId)
{
    auto dbClient = drogon::app().getDbClient();

    try
    {
        auto rankings = RankingEngine::computeAndPersistRankings(dbClient, projectId);

        Json::Value result(Json::arrayValue);
        for (auto &sr : rankings)
        {
            Json::Value item;
            item["rank"] = sr.rank;
            item["application_id"] = sr.applicationId;
            item["startup_id"] = sr.startupId;
            item["final_score"] = sr.finalScore;
            result.append(item);
        }
        callback(drogon::HttpResponse::newHttpJsonResponse(result));
    }
    catch (const std::exception &e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody(std::string("Ranking failed: ") + e.what());
        callback(resp);
    }
}
