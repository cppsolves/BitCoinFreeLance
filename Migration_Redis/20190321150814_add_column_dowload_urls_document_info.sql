-- migrate:up

ALTER TABLE document_info ADD COLUMN "download_urls" TEXT;

ALTER TABLE credit_card_info ADD COLUMN "download_urls" TEXT;

-- migrate:down

ALTER TABLE document_info DROP COLUMN "download_urls";

ALTER TABLE credit_card_info DROP COLUMN "download_urls";