-- migrate:up

CREATE TABLE affiliate_payment_info (
	id SERIAL NOT NULL,
	affiliate_id INT NOT NULL,
	admin_id INT NOT NULL,
	recipient_wallet TEXT NOT NULL,
	performed_at TIMESTAMP NOT NULL DEFAULT 'NOW()',
	crypto_rate DOUBLE PRECISION NOT NULL,
	crypto_amount DOUBLE PRECISION NOT NULL,
	price DOUBLE PRECISION NOT NULL,
	PRIMARY KEY(id),
	FOREIGN KEY(admin_id) REFERENCES user_account_info(id),
	FOREIGN KEY(affiliate_id) REFERENCES affiliate_info(id)
);

-- migrate:down

DROP TABLE IF EXISTS affiliate_payment_info;
