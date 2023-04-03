-- migrate:up

ALTER TABLE user_account_info ADD COLUMN affiliate_id INT DEFAULT NULL;

-- migrate:down

ALTER TABLE user_account_info DROP COLUMN affiliate_id;
