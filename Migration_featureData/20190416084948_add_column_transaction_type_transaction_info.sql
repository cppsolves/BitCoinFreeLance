-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "type" TEXT;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "type";