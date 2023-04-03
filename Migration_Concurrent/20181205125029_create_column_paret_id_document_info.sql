-- migrate:up

ALTER TABLE document_info
	ADD COLUMN parent_id INT DEFAULT NULL;

ALTER TABLE document_info
	ADD FOREIGN KEY (parent_id) REFERENCES document_info(id);

-- migrate:down

ALTER TABLE document_info
	DROP COLUMN parent_id;
