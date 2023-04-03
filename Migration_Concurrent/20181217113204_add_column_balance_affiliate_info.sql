-- migrate:up

ALTER TABLE affiliate_info ADD COLUMN balance DOUBLE PRECISION NOT NULL DEFAULT 0.0;

-- migrate:down

ALTER TABLE affiliate_info DROP COLUMN balance;
