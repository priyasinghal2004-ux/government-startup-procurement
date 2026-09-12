#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Maps to the 'evaluation_criteria' table.
 * Weights are stored dynamically per project, never hardcoded.
 */
namespace drogon_model
{
class EvaluationCriteria
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_project_id = "project_id";
        static constexpr const char *_name = "name";
        static constexpr const char *_weight_percent = "weight_percent";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfProjectId() const { return projectId_; }
    void setProjectId(int pid) { projectId_ = pid; }

    const std::string &getValueOfName() const { return name_; }
    void setName(const std::string &name) { name_ = name; }

    double getValueOfWeightPercent() const { return weightPercent_; }
    void setWeightPercent(double wp) { weightPercent_ = wp; }

  private:
    int id_{0};
    int projectId_{0};
    std::string name_;
    double weightPercent_{0.0};
};
} // namespace drogon_model
