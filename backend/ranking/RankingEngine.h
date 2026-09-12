#pragma once
#include <drogon/drogon.h>
#include <string>
#include <vector>

// Per-criterion breakdown for one startup's application — this is what
// powers the "Explainable Ranking" view (Part 1, Step 13).
struct CriterionScore
{
    int criterionId;
    std::string name;
    double rawScore;       // 0-100
    double weightPercent;  // e.g. 25.0
    double weightedScore;  // rawScore * weightPercent / 100
};

// One eligible startup's full result for a project.
struct StartupRanking
{
    int applicationId;
    int startupId;
    double finalScore;
    int rank;                              // assigned after sorting
    std::vector<CriterionScore> breakdown;
};

// Stateless by design — every method takes the data it needs and
// returns a result. No member variables, so there's nothing to explain
// about object lifetime or ordering of calls; each call is self-contained.
class RankingEngine
{
  public:
    // Computes, sorts, and persists rankings for every ELIGIBLE
    // application under `projectId`. Returns the sorted result so the
    // controller can respond immediately without a second DB round-trip.
    //
    // Runs synchronously (uses Drogon's sync Mapper calls, not
    // callbacks) — ranking is a batch computation over a small,
    // bounded set of eligible startups for one project, so a blocking
    // call here is simpler to read and reason about than nesting five
    // levels of async callbacks would be.
    static std::vector<StartupRanking> computeAndPersistRankings(
        const drogon::orm::DbClientPtr &dbClient, int projectId);

  private:
    // Converts one raw application field into a 0-100 score for one
    // named criterion. Only recognizes the six fixed criterion names
    // from the locked spec (Technology, Cost, Capacity, Certification,
    // Experience, Delivery) — this project has a fixed criteria set,
    // so a generic/pluggable scorer would be unneeded complexity.
    static double scoreForCriterion(const std::string &criterionName,
                                     double technologyScore,
                                     double quotedCost,
                                     double lowestCostAmongEligible,
                                     int productionCapacity,
                                     int requiredQuantity,
                                     bool certificationStatus,
                                     int experienceYears,
                                     int deliveryMonths,
                                     int fastestDeliveryAmongEligible);
};
