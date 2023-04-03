-- migrate:up

CREATE TABLE activities (
	id SERIAL NOT NULL,
	time TIMESTAMP NOT NULL DEFAULT NOW(),
	action TEXT NOT NULL,
	ip TEXT,
	user_id INT NOT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY(user_id) REFERENCES user_account_info(id)
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE activities CASCADE;
