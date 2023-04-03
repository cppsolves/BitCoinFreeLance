-- migrate:up
INSERT INTO permission
	(id, action, text, description, rank)
VALUES
	(19,  'GET_AFFILIATE_DATA',        'Get Affiliate Data',           'Can get affiliate data',            'ADMIN'       ),
	(20,  'VIEW_AFFILIATES',           'Affiliate View',               'Can get affiliate list',            'ADMIN'       ),
	(21,  'SEND_AFFILIATE_EARNINGS',   'Send Earnings To Affiliates',  'Can send earnings to affiliates',   'SUPER_ADMIN' );

INSERT INTO role_permission
	(id, role_id, permission_id)
VALUES
	(27, 1, 19),
	(28, 1, 20),
	(29, 2, 19),
	(30, 2, 20),
	(31, 2, 21);

-- migrate:down

DELETE FROM role_permission WHERE id IN
	(27, 28, 29, 30, 31);

DELETE FROM permission WHERE id IN
	(19, 20, 21);
