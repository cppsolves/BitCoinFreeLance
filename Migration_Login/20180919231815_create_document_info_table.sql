-- migrate:up

CREATE TABLE document_info (
	id SERIAL NOT NULL,
	user_id INT NOT NULL,
	type document_type NOT NULL,
	filename TEXT NOT NULL,
	uploaded_at TIMESTAMP NOT NULL DEFAULT NOW(),
	status document_status_type NOT NULL DEFAULT 'MISSING',
	reason TEXT DEFAULT '',
	PRIMARY KEY(id),
	FOREIGN KEY (user_id) REFERENCES user_account_info(id)
		ON UPDATE CASCADE
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE document_info CASCADE;
