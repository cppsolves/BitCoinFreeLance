-- migrate:up

ALTER TABLE activities ADD COLUMN author TEXT;

-- migrate:down

ALTER TABLE activities DROP COLUMN author;

