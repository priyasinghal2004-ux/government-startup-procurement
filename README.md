# Government-Startup Procurement & Ranking Platform

A high-performance procurement evaluation platform built with a **C++ (Drogon) backend**, **PostgreSQL database**, and **React + TypeScript + Vite frontend**.

Features deterministic, transparent, and explainable multi-criteria decision ranking for defense and government solicitations.

---

## 1. Prerequisites

- **PostgreSQL** (v14+ recommended)
- **C++17 Compiler** (`g++` or `clang++`), `cmake` (3.16+), `make`
- **Drogon C++ Web Framework**, `jwt-cpp`, `bcrypt`
- **Node.js** (v18+) and `npm`

---

## 2. Database Setup & Migrations

### Create the Database
```bash
# Log in to PostgreSQL CLI
psql -U postgres

# Create database
CREATE DATABASE gov_startup_ranking;
\q
```

### Run Schema Migrations
```bash
psql -U postgres -d gov_startup_ranking -f backend/migration.sql
```

### Run Seed Data
```bash
psql -U postgres -d gov_startup_ranking -f seed.sql
```

---

## 3. Backend Setup & Run (C++ / Drogon)

### Configure Database Credentials
Edit `backend/config.json` if your PostgreSQL host, username, or password differ:
```json
{
  "listeners": [{ "address": "0.0.0.0", "port": 8080, "https": false }],
  "db_clients": [
    {
      "name": "default",
      "rdbms": "postgresql",
      "host": "127.0.0.1",
      "port": 5432,
      "dbname": "gov_startup_ranking",
      "user": "postgres",
      "passwd": "postgres"
    }
  ]
}
```

### Build & Run
```bash
cd backend
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Start backend server on port 8080
./gov_startup_backend
```

The backend starts listening on `http://0.0.0.0:8080`.

---

## 4. Frontend Setup & Run (React + TypeScript + Vite)

In a separate terminal:
```bash
# Navigate to project root
npm install

# Start Vite dev server on port 3000
npm run dev
```

Open `http://localhost:3000` in your browser.

---

## 5. Seeded Login Credentials

All seeded accounts use password: `password123`

### Government Account:
- **Email:** `gov@example.gov.in`
- **Password:** `password123`
- **Role:** `government`
- **Capabilities:** Create solicitations, set evaluation criteria weights (summing to 100%), publish solicitations, view applications, and inspect the ranked leaderboard & explainable comparison matrix.

### Seeded Startup Accounts:
- **AquaTech:** `aquatech@example.com` / `password123` (Rank #1, Final Score: 93.00)
- **DeepSea:** `deepsea@example.com` / `password123` (Rank #2, Final Score: 90.50)
- **OceanX:** `oceanx@example.com` / `password123` (Rank #3, Final Score: 85.18)
- **RoboMarine:** `robomarine@example.com` / `password123` (Rank #4, Final Score: 81.36)
- **Capabilities:** View open government projects, verify mandatory requirements, submit bids with cost, capacity, technology score, certifications, experience, and delivery schedule with immediate eligibility feedback.

---

## 6. Transparent Scoring & Normalization Formula

The C++ Ranking Engine computes scores dynamically based on the project's evaluation criteria:
- **Technology (0-100):** Self-reported / validated score used directly.
- **Cost (Best-gets-100):** `(lowest_cost / quoted_cost) * 100`
- **Capacity (Target Bar):** `min((production_capacity / required_quantity) * 100, 100)`
- **Certification (Binary):** `100` if certified, `0` if not.
- **Experience (Cap at 10 yrs):** `min((experience_years / 10) * 100, 100)`
- **Delivery (Best-gets-100):** `(fastest_delivery / delivery_months) * 100`

`final_score = Σ (raw_score * (weight_percent / 100))`
All applications are ranked strictly descending by `final_score`.
