#pragma once
#include <drogon/HttpFilter.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * Drogon HttpFilters intercept requests BEFORE they reach controller methods.
 * Calling fccb() passes control down the pipeline to the controller.
 * Calling fcb(response) short-circuits the pipeline (e.g. returning 401 Unauthorized).
 */
class JwtFilter : public drogon::HttpFilter<JwtFilter>
{
  public:
    void doFilter(const drogon::HttpRequestPtr &req,
                  drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&fccb) override;
};
