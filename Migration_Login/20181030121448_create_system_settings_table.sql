-- migrate:up

CREATE TYPE api_status_type AS ENUM (
	'ON',
	'OFF'
);

CREATE TABLE system_settings (
	api_status api_status_type NOT NULL DEFAULT 'ON'
);

INSERT INTO system_settings VALUES('ON');

-- migrate:down

DROP TABLE system_settings CASCADE;
DROP TYPE api_status_type CASCADE;