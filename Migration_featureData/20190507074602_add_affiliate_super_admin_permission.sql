-- migrate:up
INSERT INTO permission
	(id, action, text, description, rank)
VALUES
	(22,  'GET_AFFILIATE_PAYMENT',   'Get withdrawal of affiliates',  'Can get withdrawal of affiliates',   'SUPER_ADMIN' );

INSERT INTO role_permission
	(id, role_id, permission_id)
VALUES
	(32, 2, 22);

-- migrate:down

DELETE FROM role_permission WHERE id = 32;

DELETE FROM permission WHERE id = 22;
