-- migrate:up

ALTER TABLE transaction_info
	ADD COLUMN "partner_id" INT;

-- migrate:down

ALTER TABLE transaction_info
	DROP COLUMN "partner_id";

