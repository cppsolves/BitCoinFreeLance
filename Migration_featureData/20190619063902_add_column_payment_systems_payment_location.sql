-- migrate:up

DELETE FROM payment_location WHERE location IN ('LU', 'EE');
ALTER TABLE payment_location DROP COLUMN "payment";
ALTER TABLE payment_location ADD COLUMN "payment_systems" TEXT [];
INSERT INTO payment_location
    (location, payment_systems)
VALUES
    ('US', '{TRUEVO, SECURETRADING}'),
    ('EE', '{SECURETRADING}'),
    ('LU', '{SECURETRADING}');

-- migrate:down

ALTER TABLE payment_location DROP COLUMN "payment_systems";
ALTER TABLE payment_location ADD COLUMN "payment" TEXT NOT NULL DEFAULT 'TRUEVO';
INSERT INTO payment_location
    (location, payment)
VALUES
    ('EE', 'SECURETRADING'),
    ('LU', 'SECURETRADING');