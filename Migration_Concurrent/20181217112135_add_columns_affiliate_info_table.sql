-- migrate:up

ALTER TABLE affiliate_info
	ADD COLUMN "inviter" INT;

ALTER TABLE affiliate_info
	ADD COLUMN "is_affiliate" BOOLEAN NOT NULL DEFAULT TRUE;

ALTER TABLE affiliate_info
	ADD COLUMN "is_inviter" BOOLEAN NOT NULL DEFAULT TRUE;

-- migrate:down

ALTER TABLE affiliate_info
	DROP COLUMN "inviter";

ALTER TABLE affiliate_info
	DROP COLUMN "is_affiliate";

ALTER TABLE affiliate_info
	DROP COLUMN "is_inviter";
