-- ============================================================================
-- Part 9: Hand-Verified Seed Data (Matches Part 7 Ranking Formula)
--
-- Passwords for all 5 accounts: "password123"
-- Bcrypt hash with cost 12: $2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu
--
-- Verified Rankings (AquaTech Anchor = 93.00):
--   Rank 1: AquaTech   (Final Score: 93.00)
--   Rank 2: DeepSea    (Final Score: 90.50)
--   Rank 3: OceanX     (Final Score: 85.18)
--   Rank 4: RoboMarine (Final Score: 81.36)
-- ============================================================================

-- 1. Users
INSERT INTO users (name, email, password_hash, role) VALUES
('Ministry of Ocean Tech', 'gov@example.gov.in',
 '$2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu', 'government'),
('AquaTech',   'aquatech@example.com',
 '$2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu', 'startup'),
('DeepSea',    'deepsea@example.com',
 '$2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu', 'startup'),
('OceanX',     'oceanx@example.com',
 '$2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu', 'startup'),
('RoboMarine', 'robomarine@example.com',
 '$2b$12$dFF5cDuWoEP2wF4zYY/vmeEmcyjqqXGv8VD1INfVEI/F7r8sDjPlu', 'startup')
ON CONFLICT (email) DO UPDATE SET password_hash = EXCLUDED.password_hash;

-- 2. Project
-- government_id = 1 (Ministry of Ocean Tech)
INSERT INTO projects (government_id, title, description, quantity, budget, delivery_months, status) VALUES
(1, 'Underwater Inspection Robot',
 'Autonomous robots for underwater pipeline and hull inspection.',
 100, 100000000.00, 12, 'OPEN')
ON CONFLICT DO NOTHING;

-- 3. Project Requirements
INSERT INTO project_requirements (project_id, requirement_text, is_mandatory) VALUES
(1, 'Autonomous navigation', TRUE),
(1, 'Underwater imaging', TRUE),
(1, 'Minimum operating depth: 500m', TRUE),
(1, 'Relevant certification', TRUE)
ON CONFLICT DO NOTHING;

-- 4. Evaluation Criteria (Dynamic weights summing to 100%)
INSERT INTO evaluation_criteria (project_id, name, weight_percent) VALUES
(1, 'Technology', 25.00),
(1, 'Cost', 20.00),
(1, 'Capacity', 20.00),
(1, 'Certification', 15.00),
(1, 'Experience', 10.00),
(1, 'Delivery', 10.00)
ON CONFLICT DO NOTHING;

-- 5. Applications
-- startup_id: AquaTech=2, DeepSea=3, OceanX=4, RoboMarine=5
-- Eligibility rule: cert=true AND capacity>=100 AND technology_score>=50 (all four pass)
INSERT INTO applications
  (project_id, startup_id, eligibility_status, quoted_cost, production_capacity,
   technology_score, certification_status, experience_years, delivery_months)
VALUES
(1, 2, 'ELIGIBLE', 85000000.00, 100, 92, TRUE, 8, 10),  -- AquaTech: Final = 93.00
(1, 3, 'ELIGIBLE', 76500000.00, 120, 78, TRUE, 6, 9),   -- DeepSea: Final = 90.50 (lowest cost, fastest delivery)
(1, 4, 'ELIGIBLE', 90000000.00, 110, 80, TRUE, 5, 11),  -- OceanX: Final = 85.18
(1, 5, 'ELIGIBLE', 95000000.00, 105, 75, TRUE, 4, 12)   -- RoboMarine: Final = 81.36
ON CONFLICT (project_id, startup_id) DO UPDATE SET
  quoted_cost = EXCLUDED.quoted_cost,
  production_capacity = EXCLUDED.production_capacity,
  technology_score = EXCLUDED.technology_score,
  certification_status = EXCLUDED.certification_status,
  experience_years = EXCLUDED.experience_years,
  delivery_months = EXCLUDED.delivery_months,
  eligibility_status = EXCLUDED.eligibility_status;
