#include "ApplicationController.h"
#include "../models/Applications.h"
#include "../models/Projects.h"
#include "../models/ApplicationScores.h"
#include "../models/ApplicationFinalScores.h"
#include "../models/EvaluationCriteria.h"
#include <drogon/orm/Mapper.h>
#include <json/json.h>

using namespace drogon::orm;
using namespace drogon_model;

/**
 * INTERVIEW DEFENSE NOTE:
 * Separation of Concerns: Eligibility vs Ranking
 * "Can this startup participate?" (Eligibility) vs "Who is better?" (Ranking)
 *
 * The three mandatory eligibility checks:
 * 1. Certification: Application must possess required certification (certification_status == true).
 * 2. Capacity: Production capacity must meet or exceed the project's requested quantity.
 * 3. Technology Baseline: Technology score must meet a minimum threshold (MIN_TECHNOLOGY_SCORE = 50).
 *
 * Why evaluate at submission time?
 * - Fails fast: The startup receives instantaneous feedback.
 * - Performance: The ranking engine never wastes CPU cycles re-validating baseline gates.
 */
static const int MIN_TECHNOLOGY_SCORE = 50;

static bool checkEligibility(const Applications &app, const Projects &project)
{
    bool certificationOk = app.getValueOfCertificationStatus();
    bool capacityOk = app.getValueOfProductionCapacity() >= project.getValueOfQuantity();
    bool technologyOk = app.getValueOfTechnologyScore() >= MIN_TECHNOLOGY_SCORE;

    return certificationOk && capacityOk && technologyOk;
}

void ApplicationController::submitApplication(const drogon::HttpRequestPtr &req,
                                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                               int projectId)
{
    auto role = req->attributes()->get<std::string>("role");
    if (role != "startup")
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k403Forbidden);
        resp->setBody("Only startup accounts can apply to projects");
        callback(resp);
        return;
    }

    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    int startupId = req->attributes()->get<int>("userId");
    auto dbClient = drogon::app().getDbClient();

    // Need the project first (to check it's OPEN and to read its
    // required quantity for the eligibility check below).
    Mapper<Projects> projectMapper(dbClient);
    projectMapper.findByPrimaryKey(
        projectId,
        [callback, json, dbClient, projectId, startupId](Projects project) {
            if (project.getValueOfStatus() != "OPEN")
            {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k400BadRequest);
                resp->setBody("This project is not open for applications");
                callback(resp);
                return;
            }

            Applications app;
            app.setProjectId(projectId);
            app.setStartupId(startupId);
            app.setQuotedCost((*json)["quoted_cost"].asDouble());
            app.setProductionCapacity((*json)["production_capacity"].asInt());
            app.setTechnologyScore((*json)["technology_score"].asInt());
            app.setCertificationStatus((*json)["certification_status"].asBool());
            app.setExperienceYears((*json)["experience_years"].asInt());
            app.setDeliveryMonths((*json)["delivery_months"].asInt());

            // Run the eligibility check now, at submission time — the
            // startup finds out immediately, and the ranking engine
            // (Part 7) never has to re-derive this later.
            bool eligible = checkEligibility(app, project);
            app.setEligibilityStatus(eligible ? "ELIGIBLE" : "NOT_ELIGIBLE");

            Mapper<Applications> appMapper(dbClient);
            appMapper.insert(
                app,
                [callback, eligible](Applications inserted) {
                    Json::Value result;
                    result["id"] = inserted.getValueOfId();
                    result["eligibility_status"] = inserted.getValueOfEligibilityStatus();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(result);
                    resp->setStatusCode(drogon::k201Created);
                    callback(resp);
                },
                [callback](const DrogonDbException &e) {
                    // Most likely cause: UNIQUE(project_id, startup_id) violation
                    // — a startup trying to apply twice to the same project.
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k409Conflict);
                    resp->setBody("You have already applied to this project");
                    callback(resp);
                });
        },
        [callback](const DrogonDbException &) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k404NotFound);
            callback(resp);
        });
}

void ApplicationController::listApplications(const drogon::HttpRequestPtr &req,
                                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                              int projectId)
{
    // Government-only: reviewing applications to their own project.
    auto role = req->attributes()->get<std::string>("role");
    if (role != "government")
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k403Forbidden);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    Mapper<Applications> mapper(dbClient);

    mapper.findBy(
        Criteria(Applications::Cols::_project_id, CompareOperator::EQ, projectId),
        [callback](std::vector<Applications> apps) {
            Json::Value result(Json::arrayValue);
            for (auto &a : apps)
            {
                Json::Value item;
                item["id"] = a.getValueOfId();
                item["startup_id"] = a.getValueOfStartupId();
                item["eligibility_status"] = a.getValueOfEligibilityStatus();
                item["quoted_cost"] = a.getValueOfQuotedCost();
                item["production_capacity"] = a.getValueOfProductionCapacity();
                item["technology_score"] = a.getValueOfTechnologyScore();
                item["certification_status"] = a.getValueOfCertificationStatus();
                item["experience_years"] = a.getValueOfExperienceYears();
                item["delivery_months"] = a.getValueOfDeliveryMonths();
                result.append(item);
            }
            callback(drogon::HttpResponse::newHttpJsonResponse(result));
        },
        [callback](const DrogonDbException &) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        });
}

void ApplicationController::getBreakdown(const drogon::HttpRequestPtr &req,
                                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                          int applicationId)
{
    auto dbClient = drogon::app().getDbClient();
    Mapper<ApplicationScores> scoreMapper(dbClient);
    Mapper<ApplicationFinalScores> finalMapper(dbClient);
    Mapper<EvaluationCriteria> critMapper(dbClient);

    scoreMapper.findBy(
        Criteria(ApplicationScores::Cols::_application_id, CompareOperator::EQ, applicationId),
        [callback, dbClient, applicationId, finalMapper, critMapper](std::vector<ApplicationScores> scores) mutable {
            Json::Value breakdown(Json::arrayValue);
            for (auto &s : scores)
            {
                // Look up the criterion name for readability in the response.
                auto crit = critMapper.findByPrimaryKey(s.getValueOfCriterionId());
                Json::Value item;
                item["criterion"] = crit.getValueOfName();
                item["raw_score"] = s.getValueOfRawScore();
                item["weighted_score"] = s.getValueOfWeightedScore();
                item["max_weighted_score"] = crit.getValueOfWeightPercent();
                breakdown.append(item);
            }

            finalMapper.findBy(
                Criteria(ApplicationFinalScores::Cols::_application_id, CompareOperator::EQ, applicationId),
                [callback, breakdown](std::vector<ApplicationFinalScores> finals) mutable {
                    Json::Value result;
                    result["breakdown"] = breakdown;
                    if (!finals.empty())
                    {
                        result["final_score"] = finals[0].getValueOfFinalScore();
                        result["rank"] = finals[0].getValueOfRank();
                    }
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                },
                [callback](const drogon::orm::DrogonDbException &) {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k500InternalServerError);
                    callback(resp);
                });
        },
        [callback](const drogon::orm::DrogonDbException &) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        });
}
