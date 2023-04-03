-- migrate:up

ALTER TABLE document_info
	ADD COLUMN issued_at DATE;

-- migrate:down

ALTER TABLE document_info
	DROP COLUMN issued_at;
