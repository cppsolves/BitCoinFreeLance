-- migrate:up

ALTER TABLE user_address_info
	ADD "country_code" VARCHAR(4);

-- migrate:down

ALTER TABLE user_address_info
	DROP COLUMN "country_code";
