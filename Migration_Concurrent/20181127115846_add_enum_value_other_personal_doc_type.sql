-- migrate:up

ALTER TYPE personal_doc_type RENAME TO personal_doc_type_old;

CREATE TYPE personal_doc_type AS ENUM (
	'DRIVING_LICENSE',
	'ID_CARD',
	'OTHER',
	'PASSPORT'
);

ALTER TABLE document_info ALTER COLUMN "personal_doc_type"
	TYPE personal_doc_type USING "personal_doc_type"::TEXT::personal_doc_type;

DROP TYPE personal_doc_type_old;

-- migrate:down

DROP TYPE IF EXISTS personal_doc_type CASCADE;
