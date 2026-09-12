#include "Users.h"
#include "Projects.h"
#include "ProjectRequirements.h"
#include "EvaluationCriteria.h"
#include "Applications.h"
#include "ApplicationScores.h"
#include "ApplicationFinalScores.h"

namespace drogon_model
{
const std::string Users::primaryKeyName = "id";
const bool Users::hasPrimaryKey = true;
const std::string Users::tableName = "users";

const std::string Projects::primaryKeyName = "id";
const bool Projects::hasPrimaryKey = true;
const std::string Projects::tableName = "projects";

const std::string ProjectRequirements::primaryKeyName = "id";
const bool ProjectRequirements::hasPrimaryKey = true;
const std::string ProjectRequirements::tableName = "project_requirements";

const std::string EvaluationCriteria::primaryKeyName = "id";
const bool EvaluationCriteria::hasPrimaryKey = true;
const std::string EvaluationCriteria::tableName = "evaluation_criteria";

const std::string Applications::primaryKeyName = "id";
const bool Applications::hasPrimaryKey = true;
const std::string Applications::tableName = "applications";

const std::string ApplicationScores::primaryKeyName = "id";
const bool ApplicationScores::hasPrimaryKey = true;
const std::string ApplicationScores::tableName = "application_scores";

const std::string ApplicationFinalScores::primaryKeyName = "id";
const bool ApplicationFinalScores::hasPrimaryKey = true;
const std::string ApplicationFinalScores::tableName = "application_final_scores";
} // namespace drogon_model
