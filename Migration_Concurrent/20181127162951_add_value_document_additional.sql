-- migrate:up

ALTER TYPE document_type RENAME TO document_type_old;

CREATE TYPE document_type AS ENUM (
	'ADDRESS',
	'ADDRESS_ADDITIONAL',
	'AVATAR',
	'OTHER',
	'PERSONAL',
	'PERSONAL_ADDITIONAL'
);

ALTER TABLE document_info
	ALTER COLUMN "type" TYPE document_type USING "type"::TEXT::document_type;

DROP TRIGGER IF EXISTS document_insert_trigger ON user_account_info;

DROP FUNCTION IF EXISTS create_document_records;

-- every user should have at least one record of each document type
CREATE OR REPLACE FUNCTION InsertMissingDocs() RETURNS VOID AS
$BODY$
DECLARE 
	row user_account_info%rowtype;
BEGIN
	FOR row in SELECT * FROM user_account_info LOOP
		INSERT INTO document_info(
			user_id,
			"type",
			filename)
		VALUES(
			row.id,
			'ADDRESS_ADDITIONAL',
			'');
		INSERT INTO document_info(
			user_id,
			"type",
			filename)
		VALUES(
			row.id,
			'PERSONAL_ADDITIONAL',
			'');
	END LOOP;
END
$BODY$
LANGUAGE 'plpgsql';

SELECT InsertMissingDocs();

-- migrate:down

DROP TYPE IF EXISTS document_type;
