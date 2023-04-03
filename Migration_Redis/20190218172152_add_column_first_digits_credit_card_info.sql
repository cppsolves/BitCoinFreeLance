-- migrate:up

ALTER TABLE credit_card_info
	ADD COLUMN "first_digits" CHAR(6);

-- migrate:down

ALTER TABLE credit_card_info
	DROP COLUMN "first_digits";

