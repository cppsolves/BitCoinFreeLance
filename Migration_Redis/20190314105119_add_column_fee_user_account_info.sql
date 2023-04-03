-- migrate:up

ALTER TABLE user_account_info ADD COLUMN fee DOUBLE PRECISION NOT NULL DEFAULT 0.085;

-- migrate:down

ALTER TABLE user_account_info DROP COLUMN fee;

