-- migrate:up

ALTER TABLE user_personal_info ADD COLUMN nationality TEXT DEFAULT NULL;

-- migrate:down

ALTER TABLE user_personal_info DROP COLUMN nationality;
