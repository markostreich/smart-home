# Deployment

Configure `DB_NAME`, `DB_USER`, `DB_PASSWORD`, and `CORS_ALLOWED_ORIGINS` in
`stack/.env`. Use `.env.example` as a template on another server. The real `.env`
is ignored by Git. Both PostgreSQL and the API use these database settings.

Build and publish the updated API image containing the PostgreSQL JDBC driver
before deploying. From this directory, run:

```sh
docker compose pull
docker compose up -d
```

PostgreSQL data lives in the `database-data` named volume. Existing H2 data is
not migrated automatically. Do not use `docker compose down -v` unless you intend
to delete the database volume.

PostgreSQL initialization settings apply when the volume is first created.
Changing `.env` later does not rename an existing database or user or change its
password; update those in PostgreSQL as well.
