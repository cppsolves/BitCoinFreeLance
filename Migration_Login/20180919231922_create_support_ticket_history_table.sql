-- migrate:up

CREATE TABLE support_ticket_history (
	id SERIAL NOT NULL,
	ticket_id INT NOT NULL,
	sender_id INT NOT NULL,
	sent_at TIMESTAMP NOT NULL DEFAULT NOW(),
	state read_state DEFAULT 'UNREAD' NOT NULL,
	message TEXT NOT NULL,
	attachment TEXT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY(ticket_id) REFERENCES support_ticket_info(id)
		ON DELETE CASCADE,
	FOREIGN KEY(sender_id) REFERENCES user_account_info(id)
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE support_ticket_history CASCADE;
