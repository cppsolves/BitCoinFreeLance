-- migrate:up

CREATE TABLE click_info (
	id SERIAL NOT NULL,
	affiliate_id INT NOT NULL,
	performed_at TIMESTAMP NOT NULL DEFAULT 'NOW()',
	ip TEXT,
	PRIMARY KEY(id),
	FOREIGN KEY(affiliate_id) REFERENCES affiliate_info(id)
);

-- migrate:down

DROP TABLE IF EXISTS click_info;
