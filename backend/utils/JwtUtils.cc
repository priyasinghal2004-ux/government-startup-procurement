#include "JwtUtils.h"
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <chrono>

// INTERVIEW DEFENSE NOTE:
// In production, secrets MUST come from environment variables (e.g., JWT_SECRET).
// Hardcoding secrets in source control is a major security vulnerability.
// We supply a fallback default only to facilitate local development/demo runs.
static std::string getSecret()
{
    if (const char *env = std::getenv("JWT_SECRET"))
        return std::string(env);
    return "dev-only-secret-change-me";
}

std::string encodeToken(int userId, const std::string &role)
{
    auto token = jwt::create()
        .set_type("JWS")
        .set_issued_now()
        .set_expires_in(std::chrono::hours(24))
        .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
        .set_payload_claim("role", jwt::claim(role))
        .sign(jwt::algorithm::hs256{getSecret()});

    return token;
}

std::optional<DecodedToken> decodeToken(const std::string &token)
{
    try
    {
        auto decoded = jwt::decode(token);

        // Validates both cryptographic HS256 HMAC signature and the expiration time
        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{getSecret()})
            .verify(decoded);  // throws if signature or expiry is invalid

        int userId = std::stoi(decoded.get_payload_claim("userId").as_string());
        std::string role = decoded.get_payload_claim("role").as_string();

        return DecodedToken{userId, role};
    }
    catch (...)
    {
        // Any parsing/verification failure = treat as unauthenticated
        return std::nullopt;
    }
}
