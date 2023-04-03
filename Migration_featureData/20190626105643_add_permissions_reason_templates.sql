-- migrate:up

INSERT INTO permission
	(id, action, text, description, rank)
VALUES
	(25,  'CREATE_REASON_TEMPLATE',   'Create a new reason template',    'Can create a new reason template',    'SUPER_ADMIN' ),
	(26,  'DELETE_REASON_TEMPLATE',   'Delete reason templates',         'Can delete reason templates',   'SUPER_ADMIN' ),
	(27,  'VIEW_REASON_TEMPLATES',    'Get a list of reason templates',  'Can get a list of reason templates',   'ADMIN'       );

INSERT INTO role_permission
	(id, role_id, permission_id)
VALUES
	(35, 2, 25),
	(36, 2, 26),
	(37, 2, 27),
	(38, 1, 27);

-- migrate:down

DELETE FROM role_permission WHERE id IN
	(35, 36, 37, 38);

DELETE FROM permission WHERE id IN
	(25, 26, 27);
