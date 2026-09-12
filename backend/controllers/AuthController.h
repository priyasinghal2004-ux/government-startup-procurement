#pragma once
#include <drogon/HttpController.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * Drogon uses compile-time CRTP (Curiously Recurring Template Pattern) for HttpControllers.
 * METHOD_LIST_BEGIN and ADD_METHOD_TO register URL paths directly to member functions
 * with exact HTTP verb constraints (drogon::Post).
 */
class AuthController : public drogon::HttpController<AuthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", drogon::Post);
    ADD_METHOD_TO(AuthController::login, "/api/auth/login", drogon::Post);
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void login(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
