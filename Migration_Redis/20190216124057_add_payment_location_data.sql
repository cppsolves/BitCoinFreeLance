-- migrate:up

INSERT INTO payment_location (location, payment) VALUES
('AT', 'TRUEVO'),
('BE', 'TRUEVO'),
('BG', 'TRUEVO'),
('HR', 'TRUEVO'),
('CY', 'TRUEVO'),
('CZ', 'TRUEVO'),
('DK', 'TRUEVO'),
('FI', 'TRUEVO'),
('FR', 'TRUEVO'),
('DE', 'TRUEVO'),
('GR', 'TRUEVO'),
('IE', 'TRUEVO'),
('IT', 'TRUEVO'),
('LV', 'TRUEVO'),
('LI', 'TRUEVO'),
('LT', 'TRUEVO'),
('MT', 'TRUEVO'),
('NL', 'TRUEVO'),
('PL', 'TRUEVO'),
('PT', 'TRUEVO'),
('RO', 'TRUEVO'),
('SK', 'TRUEVO'),
('SI', 'TRUEVO'),
('ES', 'TRUEVO'),
('SE', 'TRUEVO'),
('US', 'TRUEVO');

-- migrate:down

DELETE FROM payment_location WHERE payment = 'TRUEVO';

