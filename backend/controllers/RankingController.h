#pragma once
#include <drogon/HttpController.h>

class RankingController : public drogon::HttpController<RankingController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RankingController::getRankings, "/api/projects/{1}/rankings", drogon::Get, "JwtFilter");
    METHOD_LIST_END

    void getRankings(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      int projectId);
};
