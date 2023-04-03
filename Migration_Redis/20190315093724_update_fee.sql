-- migrate:up


ALTER TABLE affiliate_info ALTER COLUMN user_fee SET DEFAULT 0.09;
ALTER TABLE user_account_info ALTER COLUMN fee SET DEFAULT 0.09;

UPDATE affiliate_info SET user_fee = 0.09;
UPDATE user_account_info SET fee = 0.09;

-- migrate:down

ALTER TABLE affiliate_info ALTER COLUMN user_fee SET DEFAULT 0.085;
ALTER TABLE user_account_info ALTER COLUMN fee SET DEFAULT 0.085;

UPDATE affiliate_info SET user_fee = 0.085;
UPDATE user_account_info SET fee = 0.085;
