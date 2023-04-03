-- migrate:up

CREATE TABLE user_personal_info (
	id INT NOT NULL,
	first_name TEXT NOT NULL,
	middle_name TEXT NULL,
	last_name TEXT NOT NULL,
	birth_date DATE NULL,
	gender gender_type NULL,
	phone TEXT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY (id) REFERENCES user_account_info(id)
		ON UPDATE CASCADE
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE user_personal_info CASCADE;
