-- migrate:up

ALTER TYPE risk_level_type RENAME TO risk_level_type_old;

CREATE TYPE risk_level_type_tmp AS ENUM (
	'LO',
	'MED_LO',
	'MED-LO',
	'MED',
	'MED-HI',
	'MED_HI',
	'HI'
);

CREATE TYPE risk_level_type AS ENUM (
	'LO',
	'MED-LO',
	'MED',
	'MED-HI',
	'HI'
);

ALTER TABLE user_account_info ALTER COLUMN risk_level DROP DEFAULT;

ALTER TABLE user_account_info
	ALTER COLUMN risk_level TYPE risk_level_type_tmp USING risk_level::text::risk_level_type_tmp;
UPDATE user_account_info SET risk_level = 'MED-LO' WHERE risk_level = 'MED_LO';
UPDATE user_account_info SET risk_level = 'MED-HI' WHERE risk_level = 'MED_HI';

ALTER TABLE user_account_info
	ALTER COLUMN risk_level TYPE risk_level_type USING risk_level::text::risk_level_type;

ALTER TABLE user_account_info ALTER COLUMN risk_level SET DEFAULT 'LO';

DROP TYPE IF EXISTS risk_level_type_old;
DROP TYPE IF EXISTS risk_level_type_tmp;

-- migrate:down

ALTER TYPE risk_level_type RENAME TO risk_level_type_old;

CREATE TYPE risk_level_type_tmp AS ENUM (
	'LO',
	'MED_LO',
	'MED-LO',
	'MED',
	'MED-HI',
	'MED_HI',
	'HI'
);

CREATE TYPE risk_level_type AS ENUM (
	'LO',
	'MED_LO',
	'MED',
	'MED_HI',
	'HI'
);

ALTER TABLE user_account_info ALTER COLUMN risk_level DROP DEFAULT;

ALTER TABLE user_account_info
	ALTER COLUMN risk_level TYPE risk_level_type_tmp USING risk_level::text::risk_level_type_tmp;
UPDATE user_account_info SET risk_level = 'MED_LO' WHERE risk_level = 'MED-LO';
UPDATE user_account_info SET risk_level = 'MED_HI' WHERE risk_level = 'MED-HI';

ALTER TABLE user_account_info
	ALTER COLUMN risk_level TYPE risk_level_type USING risk_level::text::risk_level_type;

ALTER TABLE user_account_info ALTER COLUMN risk_level SET DEFAULT 'LO';

DROP TYPE IF EXISTS risk_level_type_old;
DROP TYPE IF EXISTS risk_level_type_tmp;

