-- ============================================================================
-- Government-Startup Procurement Ranking Platform
-- PostgreSQL Migration: Core Schema & Constraints
-- ============================================================================
-- INTERVIEW DEFENSE NOTE:
-- 1. Explicit Typed Columns: We deliberately avoided JSONB for ranking metrics.
--    This ensures direct indexed access, strict type safety, and makes query logic
--    trivial to explain ("SELECT quoted_cost, technology_score FROM applications").
-- 2. Single Users Table with ENUM Role: Simplifies authentication. One JWT verification
--    routine gates both roles without multi-table polymorphic joins.
-- 3. Separation of Eligibility vs Scoring: Applications record eligibility status
--    separately from scores. Ineligible candidates are gated before ranking.
-- ============================================================================

-- ---------------------------------------------------------
-- ENUM TYPES
-- ---------------------------------------------------------

CREATE TYPE user_role AS ENUM ('government', 'startup');

CREATE TYPE project_status AS ENUM ('DRAFT', 'OPEN', 'EVALUATION', 'CLOSED');

CREATE TYPE eligibility_status AS ENUM ('PENDING', 'ELIGIBLE', 'NOT_ELIGIBLE');

-- ---------------------------------------------------------
-- USERS
-- Both Government and Startup accounts live in one table,
-- distinguished by `role`. Keeps auth logic (login/JWT) simple:
-- one login endpoint, one users table, role read out of the JWT.
-- ---------------------------------------------------------

CREATE TABLE users (
    id             SERIAL PRIMARY KEY,
    name           VARCHAR(255)      NOT NULL,
    email          VARCHAR(255)      NOT NULL UNIQUE,
    password_hash  VARCHAR(255)      NOT NULL,
    role           user_role         NOT NULL,
    created_at     TIMESTAMPTZ       NOT NULL DEFAULT NOW()
);

-- ---------------------------------------------------------
-- PROJECTS
-- Created by a Government user. Starts as DRAFT, becomes OPEN
-- when published, moves to EVALUATION once government starts
-- reviewing rankings, and CLOSED once finished.
-- ---------------------------------------------------------

CREATE TABLE projects (
    id               SERIAL PRIMARY KEY,
    government_id    INTEGER           NOT NULL REFERENCES users(id),
    title            VARCHAR(255)      NOT NULL,
    description      TEXT,
    quantity         INTEGER           NOT NULL,
    budget           NUMERIC(14, 2)    NOT NULL,
    delivery_months  INTEGER           NOT NULL,
    status           project_status    NOT NULL DEFAULT 'DRAFT',
    created_at       TIMESTAMPTZ       NOT NULL DEFAULT NOW()
);

-- INTERVIEW DEFENSE:
-- Why a trigger instead of a foreign key table hierarchy?
-- A foreign key can only verify that `government_id` exists in `users`, but not
-- its `role` value. This lightweight PL/pgSQL trigger prevents invalid project
-- creation at the database level without requiring a separate `government_profiles` table.
CREATE OR REPLACE FUNCTION check_government_role()
RETURNS TRIGGER AS $$
BEGIN
    IF (SELECT role FROM users WHERE id = NEW.government_id) <> 'government' THEN
        RAISE EXCEPTION 'projects.government_id must reference a government user';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_projects_government_role
BEFORE INSERT OR UPDATE ON projects
FOR EACH ROW EXECUTE FUNCTION check_government_role();

-- ---------------------------------------------------------
-- PROJECT REQUIREMENTS
-- Free-form checklist shown to startups (e.g. "500m depth
-- rating", "Autonomous navigation"). `is_mandatory` feeds the
-- eligibility check — mandatory items must all be satisfied
-- for a startup to be ELIGIBLE.
-- ---------------------------------------------------------

CREATE TABLE project_requirements (
    id                SERIAL PRIMARY KEY,
    project_id        INTEGER      NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    requirement_text  VARCHAR(500) NOT NULL,
    is_mandatory      BOOLEAN      NOT NULL DEFAULT TRUE
);

-- ---------------------------------------------------------
-- EVALUATION CRITERIA
-- Weights are stored per-project (never hardcoded in code),
-- so the ranking formula reads them dynamically per procurement.
-- ---------------------------------------------------------

CREATE TABLE evaluation_criteria (
    id             SERIAL PRIMARY KEY,
    project_id     INTEGER      NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    name           VARCHAR(100) NOT NULL,   -- e.g. 'Technology', 'Cost', 'Capacity'
    weight_percent NUMERIC(5, 2) NOT NULL CHECK (weight_percent > 0 AND weight_percent <= 100)
);

-- INTERVIEW DEFENSE:
-- Why not a DB-level SUM constraint for 100% weights?
-- Enforcing SUM(weight_percent) = 100 across multiple rows in SQL requires deferred
-- triggers or table-level locks during incremental inserts. Validating this in the
-- backend API layer before project publication is clean, fast, and easier to debug.

-- ---------------------------------------------------------
-- APPLICATIONS
-- One row per startup proposal for a project.
-- Explicit typed columns (NOT JSONB) for every field the
-- ranking engine reads directly.
-- ---------------------------------------------------------

CREATE TABLE applications (
    id                   SERIAL PRIMARY KEY,
    project_id           INTEGER            NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    startup_id           INTEGER            NOT NULL REFERENCES users(id),
    eligibility_status   eligibility_status NOT NULL DEFAULT 'PENDING',

    -- Explicit typed fields read directly by the ranking engine:
    quoted_cost          NUMERIC(14, 2)     NOT NULL,
    production_capacity  INTEGER            NOT NULL,
    technology_score     INTEGER            NOT NULL CHECK (technology_score BETWEEN 0 AND 100),
    certification_status BOOLEAN            NOT NULL DEFAULT FALSE,
    experience_years     INTEGER            NOT NULL,
    delivery_months      INTEGER            NOT NULL,

    created_at           TIMESTAMPTZ        NOT NULL DEFAULT NOW(),

    -- Business Rule: A startup can only apply once per project
    UNIQUE (project_id, startup_id)
);

-- ---------------------------------------------------------
-- APPLICATION SCORES
-- Per-criterion breakdown for one application, computed by
-- the ranking engine. Powers the "Explainable Ranking" view:
-- raw_score (0-100) and weighted_score = raw_score * (weight / 100).
-- ---------------------------------------------------------

CREATE TABLE application_scores (
    id              SERIAL PRIMARY KEY,
    application_id  INTEGER        NOT NULL REFERENCES applications(id) ON DELETE CASCADE,
    criterion_id    INTEGER        NOT NULL REFERENCES evaluation_criteria(id) ON DELETE CASCADE,
    raw_score       NUMERIC(5, 2)  NOT NULL CHECK (raw_score BETWEEN 0 AND 100),
    weighted_score  NUMERIC(5, 2)  NOT NULL,

    UNIQUE (application_id, criterion_id)
);

-- ---------------------------------------------------------
-- APPLICATION FINAL SCORES
-- One row per application: the summed weighted score and its
-- rank within the project, written after the C++ ranking
-- engine sorts all eligible applications.
-- ---------------------------------------------------------

CREATE TABLE application_final_scores (
    id              SERIAL PRIMARY KEY,
    application_id  INTEGER       NOT NULL UNIQUE REFERENCES applications(id) ON DELETE CASCADE,
    final_score     NUMERIC(5, 2) NOT NULL,
    rank            INTEGER       NOT NULL
);

-- ---------------------------------------------------------
-- PERFORMANCE INDEXES
-- Targeted indexes for high-frequency lookup patterns:
-- - Filtering projects by lifecycle state (DRAFT/OPEN)
-- - Fetching all applications for a given project
-- - Fast JOINs between criteria and scores during ranking
-- ---------------------------------------------------------

CREATE INDEX idx_projects_status            ON projects (status);
CREATE INDEX idx_applications_project_id    ON applications (project_id);
CREATE INDEX idx_applications_startup_id    ON applications (startup_id);
CREATE INDEX idx_criteria_project_id        ON evaluation_criteria (project_id);
CREATE INDEX idx_final_scores_application   ON application_final_scores (application_id);
