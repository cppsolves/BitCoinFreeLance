#include "pch.h"

#include "db/AttachmentsWork.h"
#include "db/Work.h"
#include "db/data/AttachmentInfo.h"
#include "db/data/Pagination.h"

namespace keys
{

static const std::string AttachmentsTable = "attachments";
static const std::string AttachmentId = "id";
static const std::string NoteId = "note_id";
static const std::string CreatedAt = "created_at";
static const std::string FileName = "file_name";
static const std::string FileNameEncoded = "file_name_encoded";
static const std::string Extension = "extension";
static const std::string IsDeleted = "is_deleted";

}	 // namespace keys

namespace db
{

AttachmentsWork::AttachmentsWork(Work w)
	: Work(w)
{
}

data::AttachmentInfo AttachmentsWork::Insert(const data::AttachmentInfo& attachment) const
{
	// Validate conditions for the insertion
	if (!(attachment.noteId_.has_value() && attachment.fileName_.has_value()
		  && !attachment.fileName_.value().empty() && attachment.fileNameEncoded_.has_value()
		  && !attachment.fileNameEncoded_.value().empty() && attachment.extension_.has_value()))
	{
		LERR_ << "AttachmentsWork::Insert - attachment: " << nl::json(attachment).dump();
		THROW(utils::BadRequest, "Invalid data for attachment insertion");
	}

	std::vector<std::string> columns, values;
	// note id
	columns.push_back(keys::NoteId);
	values.push_back(std::to_string(attachment.noteId_.value()));
	// file name
	columns.push_back(keys::FileName);
	values.push_back(fmt::format("'{}'", Esc(attachment.fileName_.value())));
	// file name encoded
	columns.push_back(keys::FileNameEncoded);
	values.push_back(fmt::format("'{}'", Esc(attachment.fileNameEncoded_.value())));
	// extension
	columns.push_back(keys::Extension);
	values.push_back(fmt::format("'{}'", Esc(attachment.extension_.value())));
	// is_deleted always FALSE at insertion
	columns.push_back(keys::IsDeleted);
	values.push_back("FALSE");

	const auto sql{ fmt::format(
		"INSERT INTO {} ({}) VALUES ({}) RETURNING *",
		keys::AttachmentsTable,
		utils::JoinStrList(columns, ", "),
		utils::JoinStrList(values, ", ")) };

	const auto result{ Exec(sql) };
	if (result.empty())
	{
		LERR_ << fmt::format(
			"AttachmentsWork::Insert error: unable to insert in {} table object: {}",
			keys::AttachmentsTable,
			nl::json(attachment).dump());
		THROW(utils::InternalServerError, "Unexpected error");
	}

	return ExtractAttachmentInfo(result[0]);
}

data::AttachmentInfos AttachmentsWork::Select(data::AttachmentInfo& filter) const
{
	// filter.isDeleted_ is always considered false (see BuildFilter function)
	// The soft deletion of attachments is not enabled anyone

	const auto sql{ fmt::format(
		"SELECT * FROM {} {} ORDER BY {} DESC",
		keys::AttachmentsTable,
		BuildFilter(filter),
		keys::AttachmentId) };

	const auto result{ Exec(sql) };

	data::AttachmentInfos attachments;
	for (const auto& row : result)
	{
		attachments.emplace_back(ExtractAttachmentInfo(row));
	}

	return attachments;
}

std::string AttachmentsWork::BuildFilter(const data::AttachmentInfo& filter) const
{
	std::vector<std::string> conditions{};

	if (filter.id_)
	{
		conditions.push_back(fmt::format("{} = {}", keys::AttachmentId, filter.id_.value()));
	}
	if (filter.noteId_)
	{
		conditions.push_back(fmt::format("{} = {}", keys::NoteId, filter.noteId_.value()));
	}
	// The field createdAt is not considered in the filter
	if (filter.fileName_)
	{
		conditions.push_back(fmt::format("{} = '{}'", keys::FileName, Esc(*filter.fileName_)));
	}
	if (filter.fileNameEncoded_)
	{
		conditions.push_back(
			fmt::format("{} = '{}'", keys::FileNameEncoded, Esc(*filter.fileNameEncoded_)));
	}
	if (filter.extension_)
	{
		conditions.push_back(fmt::format("{} = '{}'", keys::Extension, Esc(*filter.extension_)));
	}
	// Consider only not deleted attachments
	conditions.push_back(fmt::format("{} = {}", keys::IsDeleted, "FALSE"));
	if (conditions.empty())
	{
		return "";
	}
	return "WHERE " + utils::JoinStrList(conditions, " AND ");
}

data::AttachmentInfo AttachmentsWork::ExtractAttachmentInfo(const pqxx::row& row) const
{
	data::AttachmentInfo info;
	if (!row[keys::AttachmentId].is_null())
	{
		info.id_ = row[keys::AttachmentId].as<data::Id>();
	}
	if (!row[keys::NoteId].is_null())
	{
		info.noteId_ = row[keys::NoteId].as<data::Id>();
	}
	if (!row[keys::CreatedAt].is_null())
	{
		info.createdAt_ = row[keys::CreatedAt].as<data::Timestamp>();
	}
	if (!row[keys::FileName].is_null())
	{
		info.fileName_ = row[keys::FileName].as<std::string>();
	}
	if (!row[keys::FileNameEncoded].is_null())
	{
		info.fileNameEncoded_ = row[keys::FileNameEncoded].as<std::string>();
	}
	if (!row[keys::Extension].is_null())
	{
		info.extension_ = row[keys::Extension].as<std::string>();
	}
	if (!row[keys::IsDeleted].is_null())
	{
		info.isDeleted_ = row[keys::IsDeleted].as<bool>();
	}
	return info;
}

}	 // namespace db
