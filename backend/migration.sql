-- ============================================================================
-- Government-Startup Procurement Ranking Platform
-- PostgreSQL Migration: Core Schema & Constraints
-- ============================================================================

CREATE TYPE user_role AS ENUM ('government', 'startup');
CREATE TYPE project_status AS ENUM ('DRAFT', 'OPEN', 'EVALUATION', 'CLOSED');
CREATE TYPE eligibility_status AS ENUM ('PENDING', 'ELIGIBLE', 'NOT_ELIGIBLE');

CREATE TABLE users (
    id             SERIAL PRIMARY KEY,
    name           VARCHAR(255)      NOT NULL,
    email          VARCHAR(255)      NOT NULL UNIQUE,
    password_hash  VARCHAR(255)      NOT NULL,
    role           user_role         NOT NULL,
    created_at     TIMESTAMPTZ       NOT NULL DEFAULT NOW()
);

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

CREATE TABLE project_requirements (
    id                SERIAL PRIMARY KEY,
    project_id        INTEGER      NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    requirement_text  VARCHAR(500) NOT NULL,
    is_mandatory      BOOLEAN      NOT NULL DEFAULT TRUE
);

CREATE TABLE evaluation_criteria (
    id             SERIAL PRIMARY KEY,
    project_id     INTEGER      NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    name           VARCHAR(100) NOT NULL,
    weight_percent NUMERIC(5, 2) NOT NULL CHECK (weight_percent > 0 AND weight_percent <= 100)
);

CREATE TABLE applications (
    id                   SERIAL PRIMARY KEY,
    project_id           INTEGER            NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    startup_id           INTEGER            NOT NULL REFERENCES users(id),
    eligibility_status   eligibility_status NOT NULL DEFAULT 'PENDING',
    quoted_cost          NUMERIC(14, 2)     NOT NULL,
    production_capacity  INTEGER            NOT NULL,
    technology_score     INTEGER            NOT NULL CHECK (technology_score BETWEEN 0 AND 100),
    certification_status BOOLEAN            NOT NULL DEFAULT FALSE,
    experience_years     INTEGER            NOT NULL,
    delivery_months      INTEGER            NOT NULL,
    created_at           TIMESTAMPTZ        NOT NULL DEFAULT NOW(),
    UNIQUE (project_id, startup_id)
);

CREATE TABLE application_scores (
    id              SERIAL PRIMARY KEY,
    application_id  INTEGER        NOT NULL REFERENCES applications(id) ON DELETE CASCADE,
    criterion_id    INTEGER        NOT NULL REFERENCES evaluation_criteria(id) ON DELETE CASCADE,
    raw_score       NUMERIC(5, 2)  NOT NULL CHECK (raw_score BETWEEN 0 AND 100),
    weighted_score  NUMERIC(5, 2)  NOT NULL,
    UNIQUE (application_id, criterion_id)
);

CREATE TABLE application_final_scores (
    id              SERIAL PRIMARY KEY,
    application_id  INTEGER       NOT NULL UNIQUE REFERENCES applications(id) ON DELETE CASCADE,
    final_score     NUMERIC(5, 2) NOT NULL,
    rank            INTEGER       NOT NULL
);

CREATE INDEX idx_projects_status            ON projects (status);
CREATE INDEX idx_applications_project_id    ON applications (project_id);
CREATE INDEX idx_applications_startup_id    ON applications (startup_id);
CREATE INDEX idx_criteria_project_id        ON evaluation_criteria (project_id);
CREATE INDEX idx_final_scores_application   ON application_final_scores (application_id);
