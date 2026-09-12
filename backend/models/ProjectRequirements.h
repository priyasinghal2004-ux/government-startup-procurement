#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Maps to the 'project_requirements' table.
 * Used to store project specifications and checklists.
 */
namespace drogon_model
{
class ProjectRequirements
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_project_id = "project_id";
        static constexpr const char *_requirement_text = "requirement_text";
        static constexpr const char *_is_mandatory = "is_mandatory";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfProjectId() const { return projectId_; }
    void setProjectId(int pid) { projectId_ = pid; }

    const std::string &getValueOfRequirementText() const { return requirementText_; }
    void setRequirementText(const std::string &text) { requirementText_ = text; }

    bool getValueOfIsMandatory() const { return isMandatory_; }
    void setIsMandatory(bool mandatory) { isMandatory_ = mandatory; }

  private:
    int id_{0};
    int projectId_{0};
    std::string requirementText_;
    bool isMandatory_{true};
};
} // namespace drogon_model
