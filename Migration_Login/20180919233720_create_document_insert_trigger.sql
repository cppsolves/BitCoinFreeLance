-- migrate:up

CREATE TRIGGER document_insert_trigger
	AFTER INSERT
	ON user_account_info
	FOR EACH ROW
	EXECUTE PROCEDURE create_document_records();

-- migrate:down

DROP TRIGGER document_insert_trigger
	ON user_account_info;
