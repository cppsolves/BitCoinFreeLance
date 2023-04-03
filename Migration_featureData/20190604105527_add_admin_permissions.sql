-- migrate:up
INSERT INTO permission
	(id, action, text, description, rank)
VALUES
	(23,  'GET_ADMIN_DATA',   'Get admin data',  'Can get information about admins',   'SUPER_ADMIN' ),
	(24,  'SET_ADMIN_DATA',   'Set admin data',  'Can set information about admins',   'SUPER_ADMIN');

INSERT INTO role_permission
	(id, role_id, permission_id)
VALUES
	(33, 2, 23),
	(34, 2, 24);

-- migrate:down

DELETE FROM role_permission WHERE id IN
	(33, 34);

DELETE FROM permission WHERE id IN
	(23, 24);
