-- migrate:up

ALTER TABLE transaction_info ADD COLUMN processing BOOLEAN NOT NULL DEFAULT FALSE;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN processing;