-- migrate:up

ALTER TABLE affiliate_payment_info ALTER COLUMN "status" TYPE TEXT;

UPDATE affiliate_payment_info SET "status" = 'APPROVED' WHERE "status" = 'FINISHED';

-- migrate:down

ALTER TABLE affiliate_payment_info ALTER COLUMN "status" TYPE transaction_status_type;

UPDATE affiliate_payment_info SET "status" = 'FINISHED' WHERE "status" = 'APPROVED';
