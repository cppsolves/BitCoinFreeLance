-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "payment_name" TEXT;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "payment_name";