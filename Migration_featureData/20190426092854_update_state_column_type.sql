-- migrate:up

DROP VIEW IF EXISTS users_view;
ALTER TABLE user_address_info ALTER COLUMN state TYPE VARCHAR(10);

CREATE VIEW users_view AS
SELECT
	user_account_info.*,

	user_personal_info.first_name,
	user_personal_info.middle_name,
	user_personal_info.last_name,
	user_personal_info.birth_date,
	user_personal_info.gender,
	user_personal_info.phone,
	user_personal_info.nationality,

	user_address_info.country,
	user_address_info.city,
	user_address_info.street,
	user_address_info.zip,
	user_address_info.state,
	user_address_info.country_code,

	role_view.permissions,
	coalesce(role_view.rank, 'CLIENT') AS rank
FROM user_account_info
LEFT JOIN user_personal_info ON user_account_info.id = user_personal_info.id
LEFT JOIN user_address_info ON user_account_info.id = user_address_info.id
LEFT JOIN role_view ON user_account_info.role_id = role_view.id;

-- migrate:down

