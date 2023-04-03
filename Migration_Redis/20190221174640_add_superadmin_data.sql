-- migrate:up

--
-- Insert test admins.
--

--Super Admin

INSERT INTO user_account_info(
	id,
	email,
	email_verification,
	password,
	role,
	account_progress,
	status)
VALUES(
	666,
	'super@mail.com',
	TRUE,
	'77fb18517be84571c9243fca46ad1e54186a660d1055c8e81fa5f73474cd20c4',
	'SUPER_ADMIN',
	100,
	'ACTIVE');

INSERT INTO user_personal_info(
	id,
	first_name,
	last_name,
	gender)
VALUES(
	666,
	'Super',
	'Admin',
	'MALE');

INSERT INTO user_address_info(
	id,
	country,
	country_code
	)
VALUES(
	666,
	'Ukraine',
	'UA');

-- migrate:down

DELETE FROM user_account_info WHERE id = 666;

