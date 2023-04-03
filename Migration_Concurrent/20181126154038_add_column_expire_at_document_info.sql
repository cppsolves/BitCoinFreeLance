-- migrate:up

ALTER TABLE document_info
	ADD COLUMN expire_at DATE DEFAULT NULL;

-- migrate:down

ALTER TABLE document_info
	DROP COLUMN expire_at;
