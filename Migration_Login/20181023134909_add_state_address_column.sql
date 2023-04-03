-- migrate:up

ALTER TABLE user_address_info
	ADD "state" VARCHAR(4);

-- migrate:down

ALTER TABLE user_address_info
	DROP COLUMN "state";
