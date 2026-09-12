#include "JwtFilter.h"
#include "../utils/JwtUtils.h"

/**
 * INTERVIEW DEFENSE NOTE:
 * Why an HttpFilter instead of decoding tokens manually inside each controller?
 * 1. Single Responsibility: Authentication is handled once in the middleware layer.
 * 2. Fail-Fast: Requests with missing, expired, or tampered tokens are rejected before
 *    allocating resources or executing database queries.
 * 3. Request Attribute Injection: The verified userId and role are attached to
 *    req->attributes() so controllers can read them safely without re-parsing.
 */
void JwtFilter::doFilter(const drogon::HttpRequestPtr &req,
                          drogon::FilterCallback &&fcb,
                          drogon::FilterChainCallback &&fccb)
{
    auto authHeader = req->getHeader("Authorization");
    const std::string prefix = "Bearer ";

    if (authHeader.rfind(prefix, 0) != 0)  // doesn't start with "Bearer "
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setBody("Missing or malformed Authorization header");
        fcb(resp);
        return;
    }

    std::string token = authHeader.substr(prefix.size());
    auto decoded = decodeToken(token);

    if (!decoded.has_value())
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setBody("Invalid or expired token");
        fcb(resp);
        return;
    }

    req->attributes()->insert("userId", decoded->userId);
    req->attributes()->insert("role", decoded->role);

    fccb();  // token is valid — let the request continue to the controller
}
