-- migrate:up

ALTER TABLE system_settings ADD COLUMN wallet_active BOOLEAN NOT NULL DEFAULT TRUE;

-- migrate:down

ALTER TABLE system_settings DROP COLUMN wallet_active;

