-- migrate:up

CREATE TABLE reject_reasons (
	id SERIAL NOT NULL,
    	reason TEXT NOT NULL,
	type reject_reason_type NOT NULL,
    	PRIMARY KEY(id)
);

-- migrate:down

DROP TABLE reject_reasons CASCADE;
