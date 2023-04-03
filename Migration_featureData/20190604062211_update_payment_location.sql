-- migrate:up

UPDATE payment_location SET payment = 'SECURETRADING' WHERE payment = 'FIBONATIX';


-- migrate:down

UPDATE payment_location SET payment = 'FIBONATIX' WHERE payment = 'SECURETRADING';