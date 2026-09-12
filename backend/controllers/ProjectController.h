#pragma once
#include <drogon/HttpController.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * ProjectController uses Drogon's filter binding in ADD_METHOD_TO.
 * "JwtFilter" ensures only authenticated requests reach these methods.
 * Mutating endpoints inspect the user's role from req->attributes() to enforce RBAC.
 */
class ProjectController : public drogon::HttpController<ProjectController>
{
  public:
    METHOD_LIST_BEGIN
    // Listing/viewing needs a logged-in user (either role) but no ownership check.
    ADD_METHOD_TO(ProjectController::listProjects, "/api/projects", drogon::Get, "JwtFilter");
    ADD_METHOD_TO(ProjectController::getProject, "/api/projects/{1}", drogon::Get, "JwtFilter");

    // Mutating endpoints need login; ownership/role is checked inside the handler
    // (kept there, not in a separate filter, since it needs the :id from the path).
    ADD_METHOD_TO(ProjectController::createProject, "/api/projects", drogon::Post, "JwtFilter");
    ADD_METHOD_TO(ProjectController::setCriteria, "/api/projects/{1}/criteria", drogon::Patch, "JwtFilter");
    ADD_METHOD_TO(ProjectController::publishProject, "/api/projects/{1}/publish", drogon::Post, "JwtFilter");
    METHOD_LIST_END

    void listProjects(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void createProject(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getProject(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     int projectId);

    void setCriteria(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      int projectId);

    void publishProject(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         int projectId);
};
