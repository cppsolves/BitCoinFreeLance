-- migrate:up

ALTER TABLE document_info ADD COLUMN "extension" TEXT;

-- Drops all but the last one of old records of the specified user and specified document type.
CREATE OR REPLACE FUNCTION DeleteRecords(userId NUMERIC, docType document_type) RETURNS VOID AS
$BODY$
DECLARE 
	row document_info%rowtype;
BEGIN
	FOR row IN SELECT "id" FROM document_info WHERE document_info.user_id = userId AND document_info.type = docType ORDER BY document_info.id DESC OFFSET 1 LOOP
	DELETE FROM document_info WHERE document_info.id = row.id;
	END LOOP;
END
$BODY$
LANGUAGE 'plpgsql';

-- Drops old records of every document type for every user.
CREATE OR REPLACE FUNCTION DeleteOldRecordsUser(userId NUMERIC) RETURNS VOID AS
$BODY$
DECLARE 
	row document_info%rowtype;
BEGIN
	PERFORM DeleteRecords(userId, 'ADDRESS');
	PERFORM DeleteRecords(userId, 'ADDRESS_ADDITIONAL');
	PERFORM DeleteRecords(userId, 'AVATAR');
	PERFORM DeleteRecords(userId, 'OTHER');
	PERFORM DeleteRecords(userId, 'PERSONAL');
	PERFORM DeleteRecords(userId, 'PERSONAL_ADDITIONAL');
END
$BODY$
LANGUAGE 'plpgsql';

-- Selects docs for every user and drops old records.
CREATE OR REPLACE FUNCTION ClearTable() RETURNS VOID AS
$BODY$
DECLARE 
	row user_account_info%rowtype;
BEGIN
	FOR row IN SELECT "id" FROM user_account_info LOOP
		PERFORM DeleteOldRecordsUser(row.id);
	END LOOP;
END
$BODY$
LANGUAGE 'plpgsql';

-- Selects extensions from filenames and stores them in another column.
CREATE OR REPLACE FUNCTION MoveFileExtensions() RETURNS VOID AS
$BODY$
DECLARE 
	row document_info%rowtype;
BEGIN
	FOR row IN SELECT * FROM document_info LOOP
		UPDATE document_info SET "extension" = SUBSTRING(row.filename FROM '\.[a-zA-Z]{3,4}$') WHERE document_info.id = row.id;
	END LOOP;
END
$BODY$
LANGUAGE 'plpgsql';

-- Sets filenames for every record according to its document type.
CREATE OR REPLACE FUNCTION RenameFiles() RETURNS VOID AS
$BODY$
DECLARE 
	row document_info%rowtype;
BEGIN
	FOR row IN SELECT * FROM document_info LOOP
		UPDATE document_info SET "filename" = row.type::TEXT WHERE document_info.id = row.id;
	END LOOP;
END
$BODY$
LANGUAGE 'plpgsql';

SELECT ClearTable();
SELECT MoveFileExtensions();
SELECT RenameFiles();

DROP FUNCTION IF EXISTS RenameFiles;
DROP FUNCTION IF EXISTS MoveFileExtensions;
DROP FUNCTION IF EXISTS ClearTable;
DROP FUNCTION IF EXISTS DeleteOldRecordsUser;
DROP FUNCTION IF EXISTS DeleteRecords;

-- migrate:down
