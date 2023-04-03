-- migrate:up

CREATE TYPE reject_reason_type AS ENUM (
	'PROOF_OF_IDENTIFICATION',
	'PROOF_OF_ADDRESS',
	'CREDIT_CARD_DETAILS'
);

-- migrate:down

DROP TYPE reject_reason_type CASCADE;
