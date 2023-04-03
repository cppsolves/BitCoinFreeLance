-- migrate:up

ALTER TABLE transaction_info
	ADD COLUMN "commission_amount" DOUBLE PRECISION DEFAULT NULL;

-- migrate:down

ALTER TABLE transaction_info
	DROP COLUMN "commission_amount";
