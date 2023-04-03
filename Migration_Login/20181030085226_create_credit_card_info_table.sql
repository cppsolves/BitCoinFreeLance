-- migrate:up

CREATE TABLE credit_card_info (
	id SERIAL NOT NULL,
	user_id INT NOT NULL,
	uploaded_at TIMESTAMP NOT NULL DEFAULT NOW(),
	filename TEXT NOT NULL,
	last_digits CHAR(4),
	status document_status_type NOT NULL DEFAULT 'MISSING',
	reason TEXT DEFAULT '',
	PRIMARY KEY(id),
	FOREIGN KEY (user_id) REFERENCES user_account_info(id)
		ON UPDATE CASCADE
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE credit_card_info CASCADE;
