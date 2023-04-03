-- migrate:up

ALTER TABLE transaction_info
	ADD COLUMN "session_id" TEXT;

-- migrate:down

ALTER TABLE transaction_info
	DROP COLUMN "session_id";

