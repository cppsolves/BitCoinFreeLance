-- migrate:up

ALTER TABLE user_account_info ADD COLUMN logging BOOLEAN NOT NULL DEFAULT FALSE;

-- migrate:down

ALTER TABLE user_account_info DROP COLUMN logging;

