-- migrate:up

-- Inserts document records when new user is created.
CREATE FUNCTION create_document_records()
	RETURNS TRIGGER AS
$BODY$
BEGIN
	INSERT INTO document_info(
		user_id,
		type,
		filename)
	VALUES(
		NEW.id,
		'ADDRESS',
		'');

	INSERT INTO document_info(
		user_id,
		type,
		filename)
	VALUES(
		NEW.id,
		'AVATAR',
		'');

	INSERT INTO document_info(
		user_id,
		type,
		filename)
	VALUES(
		NEW.id,
		'OTHER',
		'');

	INSERT INTO document_info(
		user_id,
		type,
		filename)
	VALUES(
		NEW.id,
		'PERSONAL',
		'');

	RETURN NEW;
END;
$BODY$
LANGUAGE plpgsql;

-- migrate:down

DROP FUNCTION create_document_records;
