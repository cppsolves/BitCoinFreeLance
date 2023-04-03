-- migrate:up

CREATE TABLE msg (
	id SERIAL NOT NULL,
	sender msg_sender NOT NULL,
	sender_id INT NOT NULL,
	recipient_id INT NOT NULL,
	text TEXT NOT NULL,
	state read_state DEFAULT 'UNREAD' NOT NULL,
	time TIMESTAMP DEFAULT NOW() NOT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY(sender_id) REFERENCES user_account_info(id),
	FOREIGN KEY(recipient_id) REFERENCES user_account_info(id)
);

-- migrate:down

DROP TABLE msg CASCADE;
