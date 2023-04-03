-- migrate:up

ALTER TABLE affiliate_info
	ADD COLUMN inviter_fee DOUBLE PRECISION NOT NULL DEFAULT 0.005,
	ADD COLUMN user_fee DOUBLE PRECISION NOT NULL DEFAULT 0.085;

-- migrate:down

ALTER TABLE user_account_info
	DROP COLUMN inviter_fee,
	DROP COLUMN user_fee;

