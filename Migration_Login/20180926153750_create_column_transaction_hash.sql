-- migrate:up

ALTER TABLE transaction_info
    ADD COLUMN hash TEXT;

-- migrate:down

ALTER TABLE transaction_info 
    DROP COLUMN hash;
