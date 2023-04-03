-- migrate:up

ALTER TABLE support_ticket_info
	ADD COLUMN index TEXT,
	ADD UNIQUE(index);

ALTER TABLE support_ticket_history
	DROP COLUMN ticket_id,
	ADD COLUMN ticket_id TEXT;

ALTER TABLE support_ticket_history
	ADD CONSTRAINT support_ticket_history_ticket_id_fkey
	FOREIGN KEY(ticket_id) REFERENCES "support_ticket_info"("index")
		ON DELETE CASCADE;

-- migrate:down
