-- migrate:up

ALTER TYPE user_role_type RENAME TO user_role_type_old;

CREATE TYPE user_role_type AS ENUM (
	'CLIENT',
	'ADMIN',
	'AFFILIATE',
	'SUPER_ADMIN'
);

ALTER TABLE user_account_info ALTER COLUMN "role" DROP DEFAULT;

ALTER TABLE user_account_info ALTER COLUMN "role"
	TYPE user_role_type USING "role"::TEXT::user_role_type;

ALTER TABLE user_account_info ALTER COLUMN "role" SET DEFAULT 'CLIENT';

DROP TYPE IF EXISTS user_role_type_old;

-- migrate:down

DROP TYPE IF EXISTS user_role_type;
