-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "failure_url" TEXT;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "failure_url";