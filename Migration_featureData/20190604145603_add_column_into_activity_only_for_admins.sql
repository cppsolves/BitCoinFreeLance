-- migrate:up

ALTER TABLE activities
	ADD COLUMN only_for_admins
	BOOLEAN NOT NULL DEFAULT FALSE;

-- migrate:down

ALTER TABLE activities
	DROP COLUMN IF EXISTS only_for_admins;
