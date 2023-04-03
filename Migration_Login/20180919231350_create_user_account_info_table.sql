-- migrate:up

CREATE TABLE user_account_info (
	id SERIAL NOT NULL,
	email TEXT NOT NULL,
	email_verification BOOLEAN NOT NULL DEFAULT FALSE,
	password TEXT NOT NULL,
	role user_role_type NOT NULL DEFAULT 'CLIENT',
	preferred_currency currency_type NOT NULL DEFAULT 'USD',
	last_login TIMESTAMP,
	created_at TIMESTAMP NOT NULL DEFAULT NOW(),
	status_2fa BOOLEAN NOT NULL DEFAULT FALSE,
	secret TEXT,
	account_progress SMALLINT NOT NULL DEFAULT 25,
	status user_status_type NOT NULL DEFAULT 'MISSING_DOCS',
	PRIMARY KEY(id),
	UNIQUE(email)
);

-- migrate:down

DROP TABLE user_account_info CASCADE;
