#include "pch.h"

#include "db/BlacklistedIpInfoWork.h"
#include "db/data/BlacklistedIpInfo.h"
#include "db/data/Pagination.h"
#include "db/utils/InsertionQueryBuilder.h"

namespace table
{

static const std::string Name{ "ip_blacklist" };

}	 // namespace table

namespace column
{

static const std::string Active{ "active" };
static const std::string AddedAt{ "added_at" };
static const std::string AddedByAdmin{ "added_by_admin" };
static const std::string BlackListName{ "blacklist_name" };
static const std::string Id{ "id" };
static const std::string IpAddress{ "ip_address" };
static const std::string UserId{ "user_id" };
static const std::string UserIdActivity{ "user_id_activity" };

}	 // namespace column

namespace db
{

BlacklistedIpInfoWork::BlacklistedIpInfoWork(HandleShPtr handle)
	: Work{ handle }
{
}

BlacklistedIpInfoWork::BlacklistedIpInfoWork(Work w)
	: Work{ std::move(w) }
{
}

std::string BlacklistedIpInfoWork::BuildFilterStr(const data::BlacklistedIpInfoFilter& filter) const
{
	std::vector<std::string> conditions;
	if (filter.id_)
	{
		conditions.push_back(fmt::format("{} = {}", column::Id, *filter.id_));
	}
	if (!filter.idList_.empty())
	{
		conditions.push_back(
			fmt::format("{} IN ({})", column::Id, Esc(utils::JoinStrList(filter.idList_, ", "))));
	}
	if (filter.listName_)
	{
		conditions.push_back(
			fmt::format("{} = '{}'", column::BlackListName, Esc(*filter.listName_)));
	}
	if (filter.ipAddress_)
	{
		conditions.push_back(
			fmt::format("{} ILIKE '{}%'", column::IpAddress, Esc(*filter.ipAddress_)));
	}
	if (!filter.ipAddressList_.empty())
	{
		conditions.push_back(fmt::format(
			"{} IN ({})",
			column::IpAddress,
			utils::TransformAndJoinList(
				filter.ipAddressList_.begin(), filter.ipAddressList_.end(), utils::Quote, ", ")));
	}
	if (filter.addedAtRange_)
	{
		if (filter.addedAtRange_->from_)
		{
			conditions.push_back(
				fmt::format("{} >= '{}'", column::AddedAt, Esc(filter.addedAtRange_->from_.value())));
		}
		if (filter.addedAtRange_->to_)
		{
			conditions.push_back(
				fmt::format("{} <= '{}'", column::AddedAt, Esc(filter.addedAtRange_->to_.value())));
		}
	}
	if (filter.addedByAdmin_)
	{
		conditions.push_back(fmt::format("{} = {}", column::AddedByAdmin, *filter.addedByAdmin_));
	}
	if (filter.active_)
	{
		conditions.push_back(fmt::format("{} = {}", column::Active, *filter.active_));
	}
	if (filter.blacklistedForEveryUser_ && *filter.blacklistedForEveryUser_)
	{
		conditions.push_back(fmt::format("{} IS NULL", column::UserId));
	}
	if (filter.userId_)
	{
		conditions.push_back(fmt::format("{} = {}", column::UserId, *filter.userId_));
	}
	if (filter.userIdActivity_)
	{
		conditions.push_back(
			fmt::format("{} = {}", column::UserIdActivity, *filter.userIdActivity_));
	}
	if (conditions.empty())
	{
		return "";
	}

	return "WHERE " + utils::JoinStrList(conditions, " AND ");
}

size_t BlacklistedIpInfoWork::Count(const data::BlacklistedIpInfoFilter& filter) const
{
	const auto query{ fmt::format(
		"SELECT COUNT(*) FROM {} {}", table::Name, BuildFilterStr(filter)) };

	const auto result{ Exec(query) };

	return result.begin()->begin()->as<size_t>(0);
}

void BlacklistedIpInfoWork::Delete(const data::BlacklistedIpInfoFilter& filter) const
{
	const auto filterCondition{ BuildFilterStr(filter) };
	if (filterCondition.empty())
	{
		THROW(utils::BadRequest, "Filter for deleting records from the DB is empty");
	}

	const auto query{ fmt::format("DELETE FROM {} {}", table::Name, filterCondition) };
	Exec(query);
}

data::BlacklistedIpInfo BlacklistedIpInfoWork::ExtractBlacklistedIpInfo(const ResultRow& row) const
{
	db::data::BlacklistedIpInfo info;

	info.id_ = row[column::Id].as<data::Id>();
	info.addedByAdmin_ = row[column::AddedByAdmin].as<data::Id>();
	info.ipAddress_ = row[column::IpAddress].as<std::string>();
	info.listName_ = row[column::BlackListName].as<std::string>();
	info.addedAt_ = row[column::AddedAt].as<data::Timestamp>();
	if (!row[column::UserId].is_null())
	{
		info.userId_ = row[column::UserId].as<data::Id>();
	}
	if (!row[column::UserIdActivity].is_null())
	{
		info.userIdActivity_ = row[column::UserIdActivity].as<data::Id>();
	}
	info.active_ = row[column::Active].as<bool>();

	return info;
}

data::BlacklistedIpList BlacklistedIpInfoWork::ExtractBlacklistedIpInfo(const Result& rows) const
{
	data::BlacklistedIpList blacklistedIps{};
	blacklistedIps.reserve(rows.size());

	for (const auto& row : rows)
	{
		blacklistedIps.emplace_back(ExtractBlacklistedIpInfo(row));
	}

	return blacklistedIps;
}

std::vector<std::string> BlacklistedIpInfoWork::GetBlacklistNames() const
{
	const auto query{ fmt::format(
		"SELECT {0} FROM {1} GROUP BY {0}", column::BlackListName, table::Name) };

	const auto result{ Exec(query) };
	std::vector<std::string> blacklists;
	blacklists.reserve(result.size());
	for (const auto& row : result)
	{
		blacklists.push_back(row.begin()->as<std::string>());
	}

	return blacklists;
}

void BlacklistedIpInfoWork::Insert(data::BlacklistedIpInfo& info) const
{
	db::InsertionQueryBuilder builder;

	if (info.id_)
	{
		builder.AddColumn(column::Id, std::to_string(*info.id_));
	}
	if (info.listName_)
	{
		builder.AddColumn(column::BlackListName, fmt::format("'{}'", Esc(*info.listName_)));
	}
	if (info.ipAddress_)
	{
		builder.AddColumn(column::IpAddress, fmt::format("'{}'", Esc(*info.ipAddress_)));
	}
	if (info.addedAt_)
	{
		builder.AddColumn(column::AddedAt, fmt::format("'{}'", Esc(*info.addedAt_)));
	}
	if (info.addedByAdmin_)
	{
		builder.AddColumn(column::AddedByAdmin, std::to_string(*info.addedByAdmin_));
	}
	if (info.active_)
	{
		builder.AddColumn(column::Active, (*info.active_ ? "TRUE" : "FALSE"));
	}
	if (info.userId_)
	{
		builder.AddColumn(column::UserId, fmt::format("'{}'", *info.userId_));
	}
	if (info.userIdActivity_)
	{
		builder.AddColumn(column::UserIdActivity, fmt::format("'{}'", *info.userIdActivity_));
	}

	try
	{
		const auto result{ Exec(builder.GetQuery(table::Name, { "*" })) };

		info = ExtractBlacklistedIpInfo(result.begin());
	}
	catch (const pqxx::unique_violation& e)
	{
		std::string ip;
		if (info.ipAddress_)
		{
			ip = *info.ipAddress_;
		}
		THROW(
			utils::IpAlreadyBlacklisted,
			fmt::format("IP address {} is already blacklisted", ip),
			ip);
	}
	catch (const pqxx::foreign_key_violation& e)
	{
		THROW(utils::BadRequest, fmt::format("User {} does not exist", *info.userId_));
	}
}

bool BlacklistedIpInfoWork::IsIpBlacklisted(
	const std::string& ipAddress, const db::data::IdOpt& userId) const
{
	data::BlacklistedIpInfoFilter filter{};
	filter.active_ = true;
	filter.ipAddress_ = ipAddress;

	const auto userIdCondition{
		userId.has_value()
			? (fmt::format("({0} = {1}) OR ({0} IS NULL)", column::UserId, userId.value()))
			: (fmt::format("({} IS NULL)", column::UserId))
	};

	const auto query{ fmt::format(
		"SELECT * FROM {} {} AND ({})", table::Name, BuildFilterStr(filter), userIdCondition) };

	const auto result{ Exec(query) };
	return (!result.empty());
}

data::BlacklistedIpList BlacklistedIpInfoWork::Select(
	const data::BlacklistedIpInfoFilter& filter, const data::Pagination& pag) const
{
	const auto query{ fmt::format(
		"SELECT * FROM {} {} ORDER BY {} DESC {}",
		table::Name,
		BuildFilterStr(filter),
		column::Id,
		data::PaginationToStr(pag)) };

	const auto result{ Exec(query) };
	data::BlacklistedIpList ipList;
	ipList.reserve(result.size());
	for (const auto& row : result)
	{
		ipList.push_back(ExtractBlacklistedIpInfo(row));
	}

	return ipList;
}

data::BlacklistedIpList BlacklistedIpInfoWork::Update(
	const data::BlacklistedIpInfo& info, const data::BlacklistedIpInfoFilter& filter) const
{
	if (!info.active_.has_value())
	{
		LERR_ << "BlacklistedIpInfoWork::Update error - info: " << nl::json(info).dump()
			  << ", filter:" << nl::json(filter).dump();
		THROW(utils::BadRequest, "Cannot de-/activate without the active field");
	}

	if (info.active_.value())
	{
		if (!info.addedByAdmin_)
		{
			LERR_ << "BlacklistedIpInfoWork::Update error - info: " << nl::json(info).dump()
				  << ", filter:" << nl::json(filter).dump();
			THROW(utils::BadRequest, "Cannot activate without the addedByAdmin field");
		}

		if (!info.listName_)
		{
			LERR_ << "BlacklistedIpInfoWork::Update error - info: " << nl::json(info).dump()
				  << ", filter:" << nl::json(filter).dump();
			THROW(utils::BadRequest, "Cannot activate without the listName field");
		}
	}

	std::string setClause{};

	if (info.active_.value())
	{
		// Activate.
		setClause = fmt::format(
			"{} = TRUE, {} = NOW(), {} = {}, {} = '{}'",
			column::Active,
			column::AddedAt,
			column::AddedByAdmin,
			info.addedByAdmin_.value(),
			column::BlackListName,
			Esc(info.listName_.value()));
	}
	else
	{
		// De-activate.
		setClause = fmt::format("{} = FALSE", column::Active);
	}

	const auto sql{ fmt::format(
		"UPDATE {} SET {} {} RETURNING *", table::Name, setClause, BuildFilterStr(filter)) };

	return ExtractBlacklistedIpInfo(Exec(sql));
}

}	 // namespace db
