-- migrate:up

CREATE TYPE crypto_currency_type AS ENUM (
	'BTC'
);

CREATE TYPE currency_type AS ENUM (
	'EUR',
	'GBP',
	'USD'
);

CREATE TYPE document_status_type AS ENUM (
	'APPROVED',
	'DENIED',
	'MISSING',
	'PENDING'
);

CREATE TYPE document_type AS ENUM (
	'ADDRESS',
	'AVATAR',
	'OTHER',
	'PERSONAL'
);

CREATE TYPE gender_type AS ENUM (
	'MALE',
	'FEMALE'
);

CREATE TYPE msg_sender AS ENUM (
	'ADMIN',
	'SYSTEM'
);

CREATE TYPE read_state AS ENUM (
	'UNREAD',
	'READ'
);

CREATE TYPE support_ticket_status_type AS ENUM (
	'OPEN',
	'RESOLVED'
);

CREATE TYPE transaction_status_type AS ENUM (
	'FAILED',
	'FINISHED',
	'PENDING'
);

CREATE TYPE user_role_type AS ENUM (
	'CLIENT',
	'ADMIN',
	'SUPER_ADMIN'
);

CREATE TYPE user_status_type AS ENUM (
	'ACTIVE',
	'INACTIVE',
	'MISSING_DOCS',
	'PENDING',
	'ARCHIVED'
);

-- migrate:down

DROP TYPE crypto_currency_type CASCADE;
DROP TYPE currency_type CASCADE;
DROP TYPE document_status_type CASCADE;
DROP TYPE document_type CASCADE;
DROP TYPE gender_type CASCADE;
DROP TYPE msg_sender CASCADE;
DROP TYPE read_state CASCADE;
DROP TYPE support_ticket_status_type CASCADE;
DROP TYPE transaction_status_type CASCADE;
DROP TYPE user_role_type CASCADE;
DROP TYPE user_status_type CASCADE;
