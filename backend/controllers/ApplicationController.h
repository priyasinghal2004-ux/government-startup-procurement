#pragma once
#include <drogon/HttpController.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * ApplicationController binds "JwtFilter" to all routes.
 * Role check:
 * - submitApplication: Startup accounts only.
 * - listApplications: Government accounts only.
 * - getBreakdown: Both roles can view explainable score breakdowns.
 */
class ApplicationController : public drogon::HttpController<ApplicationController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ApplicationController::submitApplication,
                  "/api/projects/{1}/applications", drogon::Post, "JwtFilter");
    ADD_METHOD_TO(ApplicationController::listApplications,
                  "/api/projects/{1}/applications", drogon::Get, "JwtFilter");
    ADD_METHOD_TO(ApplicationController::getBreakdown,
                  "/api/applications/{1}/breakdown", drogon::Get, "JwtFilter");
    METHOD_LIST_END

    void submitApplication(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            int projectId);

    void listApplications(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                           int projectId);

    void getBreakdown(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       int applicationId);
};
