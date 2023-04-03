-- migrate:up

CREATE FUNCTION update_account_progress()
	RETURNS trigger AS
$BODY$
DECLARE
	progress SMALLINT := 0;
BEGIN
	SELECT account_progress INTO progress FROM user_account_info WHERE id = NEW.id;
	IF progress < 50 THEN
		UPDATE user_account_info SET account_progress = 50 WHERE id = NEW.id;
	END IF;

	RETURN NEW;
END;
$BODY$
LANGUAGE plpgsql;

-- migrate:down

DROP FUNCTION update_account_progress;
