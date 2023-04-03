-- migrate:up

ALTER TYPE support_ticket_status_type RENAME TO support_ticket_status_type_old;

ALTER TABLE support_ticket_info ALTER COLUMN status DROP DEFAULT;

CREATE TYPE support_ticket_status_type AS ENUM (
	'OPEN',
	'HAS_ADMIN_MSG',
	'HAS_USER_MSG',
	'RESOLVED'
);

ALTER TABLE support_ticket_info ALTER COLUMN "status"
	TYPE support_ticket_status_type USING "status"::TEXT::support_ticket_status_type;

ALTER TABLE support_ticket_info ALTER COLUMN status SET DEFAULT 'OPEN';

DROP TYPE support_ticket_status_type_old;

-- migrate:down

DROP TYPE IF EXISTS support_ticket_status_type CASCADE;
