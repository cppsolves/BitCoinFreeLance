-- migrate:up

CREATE TABLE permission (
	id SERIAL PRIMARY KEY,
	action TEXT UNIQUE NOT NULL,
	name TEXT NOT NULL,
	description TEXT NOT NULL,
	role TEXT NOT NULL
);

CREATE TABLE role (
	id SERIAL PRIMARY KEY,
	name TEXT NOT NULL,
	description TEXT NOT NULL
);

CREATE TABLE role_permission (
	id SERIAL PRIMARY KEY,
	role_id INT REFERENCES role(id) ON DELETE CASCADE,
	permission_id INT REFERENCES permission(id) ON DELETE CASCADE
);

-- migrate:down

DROP TABLE role_permission CASCADE;
DROP TABLE role CASCADE;
DROP TABLE permission CASCADE;