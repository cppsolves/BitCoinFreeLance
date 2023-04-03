-- migrate:up

ALTER TABLE transaction_info
	ADD COLUMN "credit_card" CHAR(4);

-- migrate:down

ALTER TABLE transaction_info
	DROP COLUMN "credit_card";

