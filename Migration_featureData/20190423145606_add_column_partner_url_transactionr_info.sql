-- migrate:up

ALTER TABLE transaction_info ADD COLUMN "partner_url" TEXT;

-- migrate:down

ALTER TABLE transaction_info DROP COLUMN "partner_url";