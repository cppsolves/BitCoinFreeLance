-- migrate:up

DELETE FROM permission WHERE action IN 
(
	'GET_USER_DATA', 
	'SET_USER_DATA',
	'CREATE_NOTE',
	'VIEW_NOTES',
	'DELETE_NOTE',
	'VIEW_PERMISSIONS',
	'CREATE_PERMISSION',
	'VIEW_CLIENTS',
	'VIEW_ADMINS'
);

INSERT INTO permission
	(id, action, text, description, rank)
VALUES
	(1,  'GET_USER_DATA',        'Get User Data',        'Can get user data',        'ADMIN'      ),
	(2,  'SET_USER_DATA',        'Set User Data',        'Can set user data',        'ADMIN'      ),
	(3,  'CREATE_NOTE',          'Create Note',          'Can create note',          'ADMIN'      ),
	(4,  'VIEW_NOTES',           'View Notes',           'Can view notes',           'ADMIN'      ),
	(5,  'DELETE_NOTE',          'Delete Note',          'Can delete note',          'ADMIN'      ),
	(6,  'VIEW_CLIENTS',         'Client View',          'Can view client list',     'ADMIN'      ),
	(7,  'VIEW_ACTIVITY',        'Activity View',        'Can see own logs',         'ADMIN'      ),
	(8,  'VIEW_USER_ACTIVITY',   'User Activity View',   'Can see user activity',    'ADMIN'      ),
	(9,  'VIEW_ADMINS',          'Admin View',           'Can view admin list',      'SUPER_ADMIN'),
	(10, 'ROLE_CREATE',          'Create Role',          'Can create role',          'SUPER_ADMIN'),
	(11, 'ROLE_DELETE',          'Delete Role',          'Can delete role',          'SUPER_ADMIN'),
	(12, 'VIEW_ROLES',           'Roles View',           'Can view roles',           'SUPER_ADMIN'),
	(13, 'ROLE_UPDATE',          'Update Role',          'Can update role',          'SUPER_ADMIN'),
	(14, 'VIEW_PERMISSIONS',     'Permissions View',     'Can view permissions',     'SUPER_ADMIN'),
	(15, 'UPDATE_PERMISSION',    'Update Permission',    'Can update permission',    'SUPER_ADMIN'),
	(16, 'GET_SUPER_ADMIN_DATA', 'Get super admin Data', 'Can get super admin data', 'SUPER_ADMIN'),
	(17, 'SET_SUPER_ADMIN_DATA', 'Set super admin Data', 'Can set super admin data', 'SUPER_ADMIN'),
	(18, 'VIEW_SUPER_ADMINS',    'Super Admin View',     'Can view super admins',    'SUPER_ADMIN');

INSERT INTO role
	(id, text, description)
VALUES
	(1, 'Basic admin',       'Provides all admin possibilities'      ),
	(2, 'Basic super admin', 'Provides all super admin possibilities');

INSERT INTO role_permission
	(id, role_id, permission_id)
VALUES
	(1,  1, 1 ),
	(2,  1, 2 ),
	(3,  1, 3 ),
	(4,  1, 4 ),
	(5,  1, 5 ),
	(6,  1, 6 ),
	(7,  1, 7 ),
	(8,  1, 8 ),
	(9,  2, 1 ),
	(10, 2, 2 ),
	(11, 2, 3 ),
	(12, 2, 4 ),
	(13, 2, 5 ),
	(14, 2, 6 ),
	(15, 2, 7 ),
	(16, 2, 8 ),
	(17, 2, 9 ),
	(18, 2, 10),
	(19, 2, 11),
	(20, 2, 12),
	(21, 2, 13),
	(22, 2, 14),
	(23, 2, 15),
	(24, 2, 16),
	(25, 2, 17),
	(26, 2, 18);


UPDATE user_account_info SET role_id = 1 WHERE id = 2;
UPDATE user_account_info SET role_id = 1 WHERE id = 100;
UPDATE user_account_info SET role_id = 1 WHERE id = 101;
UPDATE user_account_info SET role_id = 2 WHERE id = 666;

-- migrate:down

UPDATE user_account_info SET role_id = NULL WHERE id = 2;
UPDATE user_account_info SET role_id = NULL WHERE id = 666;
UPDATE user_account_info SET role_id = NULL WHERE id = 100;
UPDATE user_account_info SET role_id = NULL WHERE id = 101;

DELETE FROM role_permission WHERE id IN
	(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26);

DELETE FROM role WHERE id IN (1, 2);

DELETE FROM permission WHERE id IN
	(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
