-- migrate:up

CREATE TABLE support_ticket_info (
	id SERIAL NOT NULL,
	user_id INT NOT NULL,
	started_at TIMESTAMP NOT NULL DEFAULT NOW(),
	subject TEXT NOT NULL,
	status support_ticket_status_type NOT NULL DEFAULT 'OPEN',
	PRIMARY KEY(id),
	FOREIGN KEY(user_id) REFERENCES user_account_info(id)
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE support_ticket_info CASCADE;
