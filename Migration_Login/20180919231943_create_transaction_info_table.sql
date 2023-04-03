-- migrate:up

CREATE TABLE transaction_info (
	id SERIAL NOT NULL,
	user_id INT NOT NULL,
	performed_at TIMESTAMP NOT NULL DEFAULT NOW(),
	currency currency_type NOT NULL,
	price DOUBLE PRECISION NOT NULL,
	crypto_currency crypto_currency_type NOT NULL,
	crypto_price DOUBLE PRECISION NOT NULL,
	crypto_amount DOUBLE PRECISION NOT NULL,
	recipient_wallet TEXT NOT NULL,
	status transaction_status_type NOT NULL DEFAULT 'PENDING',
	payment_id TEXT NOT NULL,
	payment_code TEXT NULL,
	payment_description TEXT NULL,
	payment_ref TEXT NULL,
	FOREIGN KEY (user_id) REFERENCES user_account_info(id)
		ON DELETE CASCADE
);

-- migrate:down

DROP TABLE transaction_info CASCADE;
