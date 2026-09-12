# Database Models (Drogon ORM)

This folder is intentionally empty right now.

Drogon's built-in ORM generates these model classes automatically from
the database schema — we don't hand-write them, and we don't pull in
an extra library (like libpqxx) to talk to Postgres.

Once `migration.sql` (Part 2) has been applied to a running Postgres
database, generate the models with:

```bash
drogon_ctl create model models -c ../db-model.json
```

This produces one `.h`/`.cc` pair per table:
- `Users`
- `Projects`
- `ProjectRequirements`
- `EvaluationCriteria`
- `Applications`
- `ApplicationScores`
- `ApplicationFinalScores`

Each generated class provides typed getters/setters matching the explicit columns in our migration.
