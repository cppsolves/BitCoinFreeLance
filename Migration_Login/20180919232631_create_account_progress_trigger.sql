-- migrate:up

CREATE TRIGGER account_progress_trigger
	AFTER UPDATE
	ON user_personal_info
	FOR EACH ROW
	EXECUTE PROCEDURE update_account_progress();

-- migrate:down

DROP TRIGGER account_progress_trigger
	ON user_personal_info;
