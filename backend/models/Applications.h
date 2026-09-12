#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Maps to the 'applications' table.
 * All ranking metrics (quoted_cost, production_capacity, technology_score,
 * certification_status, experience_years, delivery_months) are explicit typed
 * columns — NOT JSON blobs — so the ranking engine can query them directly.
 */
namespace drogon_model
{
class Applications
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_project_id = "project_id";
        static constexpr const char *_startup_id = "startup_id";
        static constexpr const char *_eligibility_status = "eligibility_status";
        static constexpr const char *_quoted_cost = "quoted_cost";
        static constexpr const char *_production_capacity = "production_capacity";
        static constexpr const char *_technology_score = "technology_score";
        static constexpr const char *_certification_status = "certification_status";
        static constexpr const char *_experience_years = "experience_years";
        static constexpr const char *_delivery_months = "delivery_months";
        static constexpr const char *_created_at = "created_at";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfProjectId() const { return projectId_; }
    void setProjectId(int pid) { projectId_ = pid; }

    int getValueOfStartupId() const { return startupId_; }
    void setStartupId(int sid) { startupId_ = sid; }

    const std::string &getValueOfEligibilityStatus() const { return eligibilityStatus_; }
    void setEligibilityStatus(const std::string &status) { eligibilityStatus_ = status; }

    double getValueOfQuotedCost() const { return quotedCost_; }
    void setQuotedCost(double cost) { quotedCost_ = cost; }

    int getValueOfProductionCapacity() const { return productionCapacity_; }
    void setProductionCapacity(int cap) { productionCapacity_ = cap; }

    int getValueOfTechnologyScore() const { return technologyScore_; }
    void setTechnologyScore(int score) { technologyScore_ = score; }

    bool getValueOfCertificationStatus() const { return certificationStatus_; }
    void setCertificationStatus(bool cert) { certificationStatus_ = cert; }

    int getValueOfExperienceYears() const { return experienceYears_; }
    void setExperienceYears(int exp) { experienceYears_ = exp; }

    int getValueOfDeliveryMonths() const { return deliveryMonths_; }
    void setDeliveryMonths(int dm) { deliveryMonths_ = dm; }

  private:
    int id_{0};
    int projectId_{0};
    int startupId_{0};
    std::string eligibilityStatus_{"PENDING"};
    double quotedCost_{0.0};
    int productionCapacity_{0};
    int technologyScore_{0};
    bool certificationStatus_{false};
    int experienceYears_{0};
    int deliveryMonths_{0};
};
} // namespace drogon_model
