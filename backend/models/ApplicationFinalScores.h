#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Matches the drogon_ctl generated model for 'application_final_scores'.
 * Stores the summed final_score and the final integer rank assigned after sorting.
 */
namespace drogon_model
{
class ApplicationFinalScores
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_application_id = "application_id";
        static constexpr const char *_final_score = "final_score";
        static constexpr const char *_rank = "rank";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfApplicationId() const { return applicationId_; }
    void setApplicationId(int aid) { applicationId_ = aid; }

    double getValueOfFinalScore() const { return finalScore_; }
    void setFinalScore(double fs) { finalScore_ = fs; }

    int getValueOfRank() const { return rank_; }
    void setRank(int r) { rank_ = r; }

  private:
    int id_{0};
    int applicationId_{0};
    double finalScore_{0.0};
    int rank_{0};
};
} // namespace drogon_model
