#include "RankingEngine.h"
#include "../models/Applications.h"
#include "../models/Projects.h"
#include "../models/EvaluationCriteria.h"
#include "../models/ApplicationScores.h"
#include "../models/ApplicationFinalScores.h"
#include <drogon/orm/Mapper.h>
#include <algorithm>
#include <cctype>

using namespace drogon::orm;
using namespace drogon_model;

// Experience caps out at 100 points once a startup has this many years —
// an explicit, simple bar, easy to defend as "a reasonable demo cutoff"
// rather than something derived from data.
static const int MAX_EXPERIENCE_YEARS_FOR_FULL_SCORE = 10;

static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

double RankingEngine::scoreForCriterion(const std::string &criterionName,
                                         double technologyScore,
                                         double quotedCost,
                                         double lowestCostAmongEligible,
                                         int productionCapacity,
                                         int requiredQuantity,
                                         bool certificationStatus,
                                         int experienceYears,
                                         int deliveryMonths,
                                         int fastestDeliveryAmongEligible)
{
    std::string name = toLower(criterionName);

    if (name == "technology")
    {
        // Already a 0-100 self-reported/validated score — used as-is.
        return technologyScore;
    }

    if (name == "cost")
    {
        // Lower cost is better. The cheapest eligible bid scores 100;
        // everyone else scores proportionally lower. Simple, standard
        // "best-gets-100" normalization — easy to defend and to
        // recompute by hand for the worked example.
        if (quotedCost <= 0) return 0;
        double score = (lowestCostAmongEligible / quotedCost) * 100.0;
        return std::min(score, 100.0);
    }

    if (name == "capacity")
    {
        // Meeting or exceeding the required quantity is a full 100;
        // below that, scored proportionally.
        if (requiredQuantity <= 0) return 100.0;
        double score = (static_cast<double>(productionCapacity) / requiredQuantity) * 100.0;
        return std::min(score, 100.0);
    }

    if (name == "certification")
    {
        // Binary requirement — either they have it or they don't.
        return certificationStatus ? 100.0 : 0.0;
    }

    if (name == "experience")
    {
        double score = (static_cast<double>(experienceYears) / MAX_EXPERIENCE_YEARS_FOR_FULL_SCORE) * 100.0;
        return std::min(score, 100.0);
    }

    if (name == "delivery")
    {
        // Faster delivery is better — same "best-gets-100" pattern as cost.
        if (deliveryMonths <= 0) return 100.0;
        double score = (static_cast<double>(fastestDeliveryAmongEligible) / deliveryMonths) * 100.0;
        return std::min(score, 100.0);
    }

    // A criterion name outside the fixed six is a configuration error,
    // not a runtime edge case a user can trigger — fail loudly rather
    // than silently scoring it as 0, so it's caught during setup.
    throw std::runtime_error("Unrecognized evaluation criterion: " + criterionName);
}

std::vector<StartupRanking> RankingEngine::computeAndPersistRankings(
    const drogon::orm::DbClientPtr &dbClient, int projectId)
{
    Mapper<Applications> appMapper(dbClient);
    Mapper<Projects> projectMapper(dbClient);
    Mapper<EvaluationCriteria> critMapper(dbClient);

    // --- 1. Load everything the formula needs ---------------------
    Projects project = projectMapper.findByPrimaryKey(projectId);

    auto eligibleApps = appMapper.findBy(
        Criteria(Applications::Cols::_project_id, CompareOperator::EQ, projectId) &&
        Criteria(Applications::Cols::_eligibility_status, CompareOperator::EQ, std::string("ELIGIBLE")));

    auto criteria = critMapper.findBy(
        Criteria(EvaluationCriteria::Cols::_project_id, CompareOperator::EQ, projectId));

    std::vector<StartupRanking> rankings;
    if (eligibleApps.empty() || criteria.empty())
        return rankings;  // nothing to rank yet

    // --- 2. Find the normalization anchors (Step 10/11 inputs) -----
    // Lowest cost and fastest delivery among *eligible* startups only —
    // ranking only ever compares startups against each other, never
    // against ones that already failed eligibility.
    double lowestCost = eligibleApps.front().getValueOfQuotedCost();
    int fastestDelivery = eligibleApps.front().getValueOfDeliveryMonths();
    for (auto &app : eligibleApps)
    {
        lowestCost = std::min(lowestCost, app.getValueOfQuotedCost());
        fastestDelivery = std::min(fastestDelivery, app.getValueOfDeliveryMonths());
    }

    // --- 3. Score + weight each application (Step 10/11) -----------
    for (auto &app : eligibleApps)
    {
        StartupRanking sr;
        sr.applicationId = app.getValueOfId();
        sr.startupId = app.getValueOfStartupId();
        sr.finalScore = 0.0;

        for (auto &crit : criteria)
        {
            double raw = scoreForCriterion(
                crit.getValueOfName(),
                app.getValueOfTechnologyScore(),
                app.getValueOfQuotedCost(), lowestCost,
                app.getValueOfProductionCapacity(), project.getValueOfQuantity(),
                app.getValueOfCertificationStatus(),
                app.getValueOfExperienceYears(),
                app.getValueOfDeliveryMonths(), fastestDelivery);

            double weighted = raw * (crit.getValueOfWeightPercent() / 100.0);

            sr.breakdown.push_back(CriterionScore{
                crit.getValueOfId(), crit.getValueOfName(), raw,
                crit.getValueOfWeightPercent(), weighted});

            sr.finalScore += weighted;
        }

        rankings.push_back(sr);
    }

    // --- 4. Sort descending by final score (Step 12) ----------------
    std::sort(rankings.begin(), rankings.end(),
              [](const StartupRanking &a, const StartupRanking &b) {
                  return a.finalScore > b.finalScore;
              });

    for (size_t i = 0; i < rankings.size(); ++i)
        rankings[i].rank = static_cast<int>(i) + 1;

    // --- 5. Persist (overwrite any previous run for this project) --
    // A project can be re-ranked (e.g. after a late application),
    // so we clear old rows for these applications before writing new
    // ones — same "replace the whole set" pattern used for criteria
    // in Part 5, for the same reason: these rows are only ever valid
    // as a complete set, not edited individually.
    Mapper<ApplicationScores> scoreMapper(dbClient);
    Mapper<ApplicationFinalScores> finalMapper(dbClient);

    for (auto &sr : rankings)
    {
        scoreMapper.deleteBy(
            Criteria(ApplicationScores::Cols::_application_id, CompareOperator::EQ, sr.applicationId));
        finalMapper.deleteBy(
            Criteria(ApplicationFinalScores::Cols::_application_id, CompareOperator::EQ, sr.applicationId));

        for (auto &cs : sr.breakdown)
        {
            ApplicationScores row;
            row.setApplicationId(sr.applicationId);
            row.setCriterionId(cs.criterionId);
            row.setRawScore(cs.rawScore);
            row.setWeightedScore(cs.weightedScore);
            scoreMapper.insert(row);
        }

        ApplicationFinalScores finalRow;
        finalRow.setApplicationId(sr.applicationId);
        finalRow.setFinalScore(sr.finalScore);
        finalRow.setRank(sr.rank);
        finalMapper.insert(finalRow);
    }

    return rankings;
}
