-- migrate:up

CREATE TABLE affiliate_info (
	id SERIAL NOT NULL,
	email TEXT NOT NULL,
	password TEXT NOT NULL,
	first_name TEXT NOT NULL,
	last_name TEXT NOT NULL,
	skype TEXT,
	fee REAL NOT NULL,
	wallet TEXT,
	email_verification BOOLEAN NOT NULL DEFAULT FALSE,
	status_2fa BOOLEAN NOT NULL DEFAULT FALSE,
	created_at TIMESTAMP NOT NULL DEFAULT NOW(),
	last_login TIMESTAMP,
	PRIMARY KEY(id),
	UNIQUE(email)
);

--
-- Change autoincrement value
--
ALTER SEQUENCE affiliate_info_id_seq RESTART WITH 1000;

-- migrate:down

DROP TABLE affiliate_info CASCADE;
