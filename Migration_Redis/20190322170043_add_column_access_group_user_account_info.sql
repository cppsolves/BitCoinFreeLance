-- migrate:up

ALTER TABLE
	user_account_info ADD COLUMN role_id INT REFERENCES role(id) ON DELETE SET NULL;

-- migrate:down

ALTER TABLE user_account_info DROP COLUMN role_id;

