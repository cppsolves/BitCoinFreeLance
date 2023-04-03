-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "description" TEXT;

UPDATE transaction_info SET description = payment_description;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "description";
