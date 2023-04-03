-- migrate:up

CREATE TYPE risk_level_type AS ENUM (
	'LO',
	'MED_LO',
	'MED',
	'MED_HI',
	'HI'
);

ALTER TABLE user_account_info ADD COLUMN risk_level risk_level_type NOT NULL DEFAULT 'LO';
ALTER TABLE user_account_info ADD COLUMN ongoing_monitoring BOOLEAN NOT NULL DEFAULT FALSE;
ALTER TABLE user_account_info ADD COLUMN blacklist BOOLEAN NOT NULL DEFAULT FALSE;
ALTER TABLE user_account_info ADD COLUMN pep BOOLEAN NOT NULL DEFAULT FALSE;

-- migrate:down

ALTER TABLE user_account_info DROP COLUMN risk_level;
ALTER TABLE user_account_info DROP COLUMN ongoing_monitoring;
ALTER TABLE user_account_info DROP COLUMN blacklist;
ALTER TABLE user_account_info DROP COLUMN pep;

DROP TYPE risk_level_type CASCADE;
