-- migrate:up

ALTER TABLE affiliate_payment_info ADD COLUMN "status" transaction_status_type NOT NULL DEFAULT 'PENDING';
ALTER TABLE affiliate_payment_info ADD COLUMN "currency" currency_type NOT NULL DEFAULT 'USD';
ALTER TABLE affiliate_payment_info DROP COLUMN "admin_id";
ALTER TABLE affiliate_payment_info DROP COLUMN "affiliate_id";

ALTER TABLE affiliate_payment_info ADD COLUMN "admin_id" INT NULL;
ALTER TABLE affiliate_payment_info ADD COLUMN "affiliate_id" INT NOT NULL;
ALTER TABLE affiliate_payment_info ADD COLUMN "closed_at" TIMESTAMP NOT NULL DEFAULT 'NOW()';

-- migrate:down

ALTER TABLE affiliate_payment_info DROP COLUMN "status";
ALTER TABLE affiliate_payment_info DROP COLUMN "currency";
ALTER TABLE affiliate_payment_info DROP COLUMN "closed_at";
