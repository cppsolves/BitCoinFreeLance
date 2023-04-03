-- migrate:up

CREATE TABLE reason_templates (
	id SERIAL PRIMARY KEY,
	"text" TEXT NOT NULL,
	"type" TEXT NOT NULL
);

-- migrate:down

DROP TABLE IF EXISTS reason_templates CASCADE;
