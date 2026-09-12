#pragma once
#include <string>

/**
 * INTERVIEW DEFENSE NOTE:
 * Matches the drogon_ctl generated model for the 'projects' table.
 * Strongly typed getters and setters directly map to the PostgreSQL columns.
 */
namespace drogon_model
{
class Projects
{
  public:
    struct Cols
    {
        static constexpr const char *_id = "id";
        static constexpr const char *_government_id = "government_id";
        static constexpr const char *_title = "title";
        static constexpr const char *_description = "description";
        static constexpr const char *_quantity = "quantity";
        static constexpr const char *_budget = "budget";
        static constexpr const char *_delivery_months = "delivery_months";
        static constexpr const char *_status = "status";
        static constexpr const char *_created_at = "created_at";
    };

    static const std::string primaryKeyName;
    static const bool hasPrimaryKey;
    static const std::string tableName;

    int getValueOfId() const { return id_; }
    void setId(int id) { id_ = id; }

    int getValueOfGovernmentId() const { return governmentId_; }
    void setGovernmentId(int gid) { governmentId_ = gid; }

    const std::string &getValueOfTitle() const { return title_; }
    void setTitle(const std::string &title) { title_ = title; }

    const std::string &getValueOfDescription() const { return description_; }
    void setDescription(const std::string &desc) { description_ = desc; }

    int getValueOfQuantity() const { return quantity_; }
    void setQuantity(int qty) { quantity_ = qty; }

    double getValueOfBudget() const { return budget_; }
    void setBudget(double b) { budget_ = b; }

    int getValueOfDeliveryMonths() const { return deliveryMonths_; }
    void setDeliveryMonths(int dm) { deliveryMonths_ = dm; }

    const std::string &getValueOfStatus() const { return status_; }
    void setStatus(const std::string &st) { status_ = st; }

  private:
    int id_{0};
    int governmentId_{0};
    std::string title_;
    std::string description_;
    int quantity_{0};
    double budget_{0.0};
    int deliveryMonths_{0};
    std::string status_{"DRAFT"};
};
} // namespace drogon_model
