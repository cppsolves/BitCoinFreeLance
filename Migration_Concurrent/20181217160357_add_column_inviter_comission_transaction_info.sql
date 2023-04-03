-- migrate:up

ALTER TABLE transaction_info
	ADD COLUMN "inviter_commission" DOUBLE PRECISION DEFAULT NULL;

-- migrate:down

ALTER TABLE transaction_info
	DROP COLUMN "inviter_commission";
