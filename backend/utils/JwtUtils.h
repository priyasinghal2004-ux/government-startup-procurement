#pragma once
#include <string>
#include <optional>

// ============================================================================
// INTERVIEW DEFENSE NOTE:
// Why a pair of free functions instead of a Singleton / AuthManager class?
// Token signing and decoding are stateless, pure mathematical operations.
// Wrapping them in an object adds unnecessary ceremony and hidden state.
// Free functions in a dedicated utility namespace are idiomatic C++, testable,
// and take zero cognitive load to defend during an interview.
// ============================================================================

struct DecodedToken
{
    int userId;
    std::string role;   // "government" or "startup"
};

// Signs a token containing userId + role. Expires in 24 hours.
std::string encodeToken(int userId, const std::string &role);

// Returns std::nullopt if the token is missing, malformed, expired,
// or has a bad signature — the filter treats all of those the same way.
std::optional<DecodedToken> decodeToken(const std::string &token);
