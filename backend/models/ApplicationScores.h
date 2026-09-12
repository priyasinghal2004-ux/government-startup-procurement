#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Matches the drogon_ctl generated model for 'application_scores'.
 * Stores per-criterion raw score (0-100) and weighted score.
 * Directly powers the "Explainable Ranking" view.
 */
namespace drogon_model
{
class ApplicationScores
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_application_id = "application_id";
        static constexpr const char *_criterion_id = "criterion_id";
        static constexpr const char *_raw_score = "raw_score";
        static constexpr const char *_weighted_score = "weighted_score";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfApplicationId() const { return applicationId_; }
    void setApplicationId(int aid) { applicationId_ = aid; }

    int getValueOfCriterionId() const { return criterionId_; }
    void setCriterionId(int cid) { criterionId_ = cid; }

    double getValueOfRawScore() const { return rawScore_; }
    void setRawScore(double rs) { rawScore_ = rs; }

    double getValueOfWeightedScore() const { return weightedScore_; }
    void setWeightedScore(double ws) { weightedScore_ = ws; }

  private:
    int id_{0};
    int applicationId_{0};
    int criterionId_{0};
    double rawScore_{0.0};
    double weightedScore_{0.0};
};
} // namespace drogon_model
