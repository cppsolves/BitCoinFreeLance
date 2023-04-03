-- migrate:up

CREATE TABLE user_address_info (
	id INT NOT NULL,
	country TEXT NULL,
	city TEXT NULL,
	street TEXT NULL,
	zip TEXT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY (id) REFERENCES user_account_info(id)
		ON UPDATE CASCADE
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE user_address_info CASCADE;
