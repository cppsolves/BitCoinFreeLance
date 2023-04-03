-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "updated_at" TIMESTAMP NULL;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "updated_at";