#include "pch.h"

#include "db/AffiliateWork.h"
#include "db/data/Pagination.h"
#include "db/data/Ranges.h"
#include "db/data/Types.h"
#include "db/data/Affiliate.h"
#include "db/utils/CsvExportUtils.h"

namespace db
{
namespace table
{

static const std::string Name{ "affiliate_info" };

}

namespace column
{

static const std::string Id{ "id" };
static const std::string Email{ "email" };
static const std::string Password{ "password" };
static const std::string FirstName{ "first_name" };
static const std::string LastName{ "last_name" };
static const std::string Skype{ "skype" };
static const std::string Fee{ "fee" };
static const std::string InviterFee{ "inviter_fee" };
static const std::string UserFee{ "user_fee" };
static const std::string Wallet{ "wallet" };
static const std::string EmailVerification{ "email_verification" };
static const std::string Status2fa{ "status_2fa" };
static const std::string CreatedAt{ "created_at" };
static const std::string LastLoginAt{ "last_login" };
static const std::string Balance{ "balance" };
static const std::string Inviter{ "inviter" };
static const std::string IsAffiliate{ "is_affiliate" };
static const std::string IsInviter{ "is_inviter" };
static const std::string AffiliateId{ "affiliate_id" };
static const std::string Secret{ "secret" };
static const std::string FlowType{ "flow_type" };
static const std::string ApiKey{ "api_key" };
static const std::string RoleType{ "role_type" };
static const std::string Status{ "status" };

}	 // namespace column

enum TableColToCsvEnum
{
	TABLE_COL_ID = 0,
	TABLE_COL_FIRST_NAME = 1,
	TABLE_COL_LAST_NAME = 2,
	TABLE_COL_EMAIL = 3,
	TABLE_COL_EMAIL_VERIFICATION = 4,
	TABLE_COL_SKYPE = 5,
	TABLE_COL_WALLET = 6,
	TABLE_COL_COMMISSION_PERCENTAGE = 7,	// affiliate_info.fee
	TABLE_COL_MASTER_COMMISSION = 8,		// affiliate_info.inviter_fee
	TABLE_COL_BALANCE = 9,
	TABLE_COL_CREATED_AT_DEF_TZ = 10,
	TABLE_COL_CREATED_AT_TZ = 11,
	TABLE_COL_LAST_LOGIN_DEF_TZ = 12,
	TABLE_COL_LAST_LOGIN_TZ = 13
};

AffiliateWork::AffiliateWork(HandleShPtr handle)
	: Work{ handle }
{
}

AffiliateWork::AffiliateWork(Work w)
	: Work{ std::move(w) }
{
}

std::string AffiliateWork::BuildFilter(const data::AffiliateFilter& filter) const
{
	using namespace fmt;

	std::vector<std::string> sql;

	if (filter.id_)
	{
		sql.push_back(format("{} = {}", column::Id, *filter.id_));
	}
	if (filter.email_)
	{
		sql.push_back(format("{} ILIKE '%{}%'", column::Email, Esc(*filter.email_)));
	}
	if (filter.emailStrict_)
	{
		sql.push_back(format("{} = '{}'", column::Email, Esc(*filter.emailStrict_)));
	}
	if (filter.firstName_)
	{
		sql.push_back(format("{} ILIKE '%{}%'", column::FirstName, Esc(*filter.firstName_)));
	}
	if (filter.lastName_)
	{
		sql.push_back(format("{} ILIKE '%{}%'", column::LastName, Esc(*filter.lastName_)));
	}
	if (filter.skype_)
	{
		sql.push_back(format("{} ILIKE '%{}%'", column::Skype, Esc(*filter.skype_)));
	}
	if (filter.fee_)
	{
		if (filter.fee_->from_)
		{
			sql.push_back(format("{} >= {}", column::Fee, *filter.fee_->from_));
		}
		if (filter.fee_->to_)
		{
			sql.push_back(format("{} <= {}", column::Fee, *filter.fee_->to_));
		}
	}
	if (filter.inviterFee_)
	{
		if (filter.inviterFee_->from_)
		{
			sql.push_back(format("{} >= {}", column::InviterFee, *filter.inviterFee_->from_));
		}
		if (filter.inviterFee_->to_)
		{
			sql.push_back(format("{} <= {}", column::InviterFee, *filter.inviterFee_->to_));
		}
	}
	if (filter.wallet_)
	{
		sql.push_back(format("{} ILIKE '%{}%'", column::Wallet, Esc(*filter.wallet_)));
	}
	if (filter.emailVerification_)
	{
		sql.push_back(format("{} = {}", column::EmailVerification, *filter.emailVerification_));
	}
	if (filter.status2fa_)
	{
		sql.push_back(format("{} = {}", column::Status2fa, *filter.status2fa_));
	}
	if (filter.createdAt_)
	{
		if (filter.createdAt_->from_ && !filter.createdAt_->from_.value().empty())
		{
			sql.push_back(format("{} >= '{}'", column::CreatedAt, Esc(*filter.createdAt_->from_)));
		}
		if (filter.createdAt_->to_ && !filter.createdAt_->to_.value().empty())
		{
			sql.push_back(format("{} <= '{}'", column::CreatedAt, Esc(*filter.createdAt_->to_)));
		}
	}
	if (filter.lastLoginAt_)
	{
		if (filter.lastLoginAt_->from_ && !filter.lastLoginAt_->from_.value().empty())
		{
			sql.push_back(
				format("{} >= '{}'", column::LastLoginAt, Esc(*filter.lastLoginAt_->from_)));
		}
		if (filter.lastLoginAt_->to_ && !filter.lastLoginAt_->to_.value().empty())
		{
			sql.push_back(
				format("{} <= '{}'", column::LastLoginAt, Esc(*filter.lastLoginAt_->to_)));
		}
	}
	if (filter.balance_)
	{
		if (filter.balance_->from_)
		{
			sql.push_back(format("{} >= {}", column::Balance, *filter.balance_->from_));
		}
		if (filter.balance_->to_)
		{
			sql.push_back(format("{} <= {}", column::Balance, *filter.balance_->to_));
		}
	}
	if (filter.inviter_)
	{
		sql.push_back(format("{} = {}", column::Inviter, *filter.inviter_));
	}
	if (filter.inviterPart_)
	{
		sql.push_back(format("{}::text ILIKE '{}%'", column::Inviter, Esc(*filter.inviterPart_)));
	}
	if (filter.isAffiliate_)
	{
		sql.push_back(format("{} = {}", column::IsAffiliate, *filter.isAffiliate_));
	}
	if (filter.isInviter_)
	{
		sql.push_back(format("{} = {}", column::IsInviter, *filter.isInviter_));
	}
	if (filter.affiliateId_)
	{
		sql.push_back(format("{} = {}", column::AffiliateId, *filter.affiliateId_));
	}
	if (filter.affiliateIdPart_)
	{
		sql.push_back(
			format("{}::text ILIKE '{}%'", column::AffiliateId, Esc(*filter.affiliateIdPart_)));
	}
	if (filter.apiKey_)
	{
		sql.push_back(format("{} = '{}'", column::ApiKey, *filter.apiKey_));
	}
	if (filter.roleType_)
	{
		sql.push_back(
			format("{} = '{}'", column::RoleType, db::data::RoleType2Str(*filter.roleType_)));
	}
	if (filter.status_)
	{
		sql.push_back(
			format("{} = '{}'", column::Status, db::data::AffiliateStatus2Str(*filter.status_)));
	}
	if (sql.empty())
	{
		return "";
	}

	return "WHERE " + utils::JoinStrList(sql, " AND ");
}

size_t AffiliateWork::Count(const data::AffiliateFilter& filter) const
{
	const auto query{ fmt::format("SELECT COUNT(*) FROM {} {}", table::Name, BuildFilter(filter)) };

	const auto result{ Exec(query) };

	return result.begin()->begin()->as<size_t>(0);
}

std::string AffiliateWork::GetInsertionString(const data::Affiliate& account) const
{
	std::vector<std::string> columns, values;

	const auto push{ [&columns, &values](const auto& column, const auto& value) {
		columns.push_back(column);
		values.push_back(value);
	} };

	{
		using namespace fmt;

		if (account.id_)
		{
			push(column::Id, format("{}", *account.id_));
		}
		if (account.email_)
		{
			push(column::Email, format("'{}'", Esc(*account.email_)));
		}
		else
		{
			LERR_ << "AffiliateWork::GetInsertionString - account is " << nl::json(account).dump();
			THROW(utils::BadRequest, "Invalid data");
		}
		if (account.password_)
		{
			push(column::Password, format("'{}'", Esc(*account.password_)));
		}
		else
		{
			LERR_ << "AffiliateWork::GetInsertionString - account is " << nl::json(account).dump();
			THROW(utils::BadRequest, "Invalid data");
		}
		if (account.firstName_)
		{
			push(column::FirstName, format("'{}'", Esc(*account.firstName_)));
		}
		else
		{
			LERR_ << "AffiliateWork::GetInsertionString - account is " << nl::json(account).dump();
			THROW(utils::BadRequest, "Invalid data");
		}
		if (account.lastName_)
		{
			push(column::LastName, format("'{}'", Esc(*account.lastName_)));
		}
		else
		{
			LERR_ << "AffiliateWork::GetInsertionString - account is " << nl::json(account).dump();
			THROW(utils::BadRequest, "Invalid data");
		}
		if (account.skype_)
		{
			push(column::Skype, format("'{}'", Esc(*account.skype_)));
		}
		if (account.fee_)
		{
			push(column::Fee, format("{}", *account.fee_));
		}
		else
		{
			LERR_ << "AffiliateWork::GetInsertionString - account is " << nl::json(account).dump();
			THROW(utils::BadRequest, "Invalid data");
		}
		if (account.inviterFee_)
		{
			push(column::InviterFee, format("{}", *account.inviterFee_));
		}
		if (account.userFee_)
		{
			push(column::UserFee, format("{}", *account.userFee_));
		}
		if (account.wallet_)
		{
			push(column::Wallet, format("'{}'", Esc(*account.wallet_)));
		}
		if (account.emailVerification_)
		{
			push(column::EmailVerification, format("{}", *account.emailVerification_));
		}
		if (account.status2fa_)
		{
			push(column::Status2fa, format("{}", *account.status2fa_));
		}
		if (account.createdAt_)
		{
			push(column::CreatedAt, format("'{}'", Esc(*account.createdAt_)));
		}
		if (account.lastLoginAt_)
		{
			push(column::LastLoginAt, format("'{}'", Esc(*account.lastLoginAt_)));
		}
		if (account.inviter_)
		{
			push(column::Inviter, format("{}", *account.inviter_));
		}
		if (account.isAffiliate_)
		{
			push(column::IsAffiliate, format("{}", *account.isAffiliate_));
		}
		if (account.isInviter_)
		{
			push(column::IsInviter, format("{}", *account.isInviter_));
		}
		if (account.affiliateId_)
		{
			push(column::AffiliateId, format("{}", *account.affiliateId_));
		}
		if (account.secret_)
		{
			push(column::Secret, format("'{}'", Esc(*account.secret_)));
		}
		if (account.apiKey_)
		{
			push(column::ApiKey, format("'{}'", *account.apiKey_));
		}
		if (account.roleType_)
		{
			push(column::RoleType, format("'{}'", db::data::RoleType2Str(*account.roleType_)));
		}
		else
		{
			LERR_ << "Trying to insert account without role type to " + table::Name;
			THROW(utils::BadRequest, "Role Type field is required");
		}
		if (account.status_)
		{
			push(column::Status, format("'{}'", db::data::AffiliateStatus2Str(*account.status_)));
		}
	}

	return fmt::format(
		"INSERT INTO {}({}) VALUES({}) RETURNING *",
		table::Name,
		utils::JoinStrList(columns, ", "),
		utils::JoinStrList(values, ", "));
}

data::Affiliate AffiliateWork::Insert(data::Affiliate account) const
{
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> dis{ 1001, 9999 };

	// Number of attempts if generated affiliate id already exists in the database.
	constexpr uint8_t attempts{ 5 };
	for (uint8_t i = 0; i < attempts; ++i)
	{
		// Generate "unique" 4-digit id between 1001 and 9999.
		account.affiliateId_ = dis(gen);

		// Select and check if such id already exists.
		const auto sql = fmt::format("SELECT {} FROM {}", column::AffiliateId, table::Name);
		const auto result{ Exec(sql) };
		const bool exists{ std::any_of(result.begin(), result.end(), [&account](const auto& row) {
			data::Id id;
			row.begin()->to(id);

			return id == *account.affiliateId_;
		}) };

		if (!exists)
		{
			break;
		}
		if (i == attempts - 1)
		{
			LERR_ << "AffiliateWork::Insert -  on affiliate creation process, retry in 1 minute. "
					 "Sql is "
				  << sql;
			THROW(utils::BadRequest, "Invalid data");
		}
	}

	const auto query{ GetInsertionString(account) };

	const auto result{ Exec(query) };

	return ExtractAffiliate(*result.begin());
}

data::Affiliates AffiliateWork::Select(
	const data::AffiliateFilter& filter, const data::Pagination& pagination) const
{
	const auto query{ fmt::format(
		"SELECT * FROM {} {} ORDER BY {} DESC {}",
		table::Name,
		BuildFilter(filter),
		column::Id,
		data::PaginationToStr(pagination)) };

	return ExtractAffiliates(Exec(query));
}

std::string AffiliateWork::GenerateSqlQueryForCsvExport(const data::AffiliateFilter& filter) const
{
	static const std::string tz(utils::StatsConfig::Singleton().timeRangesConfig_.timezone_);

	static const std::string defaultTz(
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_);

	const std::string filterString = BuildFilter(filter);
	LDBG_ << fmt::format(
		"AffiliateWork::GenerateSqlQueryForCsvExport - Generating query with filter: {}",
		filterString);

	const std::string sqlStatement(fmt::format(
		R"__(
		SELECT
			ai.affiliate_id,
			ai.first_name,
			ai.last_name,
			ai.email,
			ai.email_verification,
			ai.skype,
			ai.wallet,
			ai.fee,
			ai.inviter_fee,
			ai.balance,
			date_trunc('second', ai.created_at) AT TIME ZONE '{defaultTz}' AS "created_at_{defaultTz}",
			date_trunc('second', ai.created_at) AT TIME ZONE '{tz}' AS "created_at_{tz}",
			date_trunc('second', ai.last_login) AT TIME ZONE '{defaultTz}' AS "last_login_{defaultTz}",
			date_trunc('second', ai.last_login) AT TIME ZONE '{tz}' AS "last_login_{tz}"
		FROM affiliate_info ai
		{filter}
		ORDER BY
			ai.affiliate_id
		DESC
		)__",
		fmt::arg("tz", tz),
		fmt::arg("defaultTz", defaultTz),
		fmt::arg("filter", filterString)));

	LDBG_ << fmt::format(
		"AffiliateWork::GenerateSqlQueryForCsvExport - SQL query generated: {}", sqlStatement);

	return sqlStatement;
}

std::string AffiliateWork::OutputAffiliatesCsvHeader()
{
	static const std::string createdAtHeader("Created At");
	static const std::string lastLoginHeader("Last Login");

	// clang-format off
	return fmt::format(
		"ID,"
		"First Name,"
		"Last Name,"
		"Email,"
		"Email Verification,"
		"Skype,"
		"Wallet,"
		"Commission Percentage,"
		"Master Commission,"
		"Balance,"
		"{createdAt_defaultTz},"
		"{createdAt_tz},"
		"{lastLogin_defaultTz},"
		"{lastLogin_tz},",
		fmt::arg("createdAt_defaultTz", utils::GetDefaultTzStr(createdAtHeader)),
		fmt::arg("createdAt_tz", utils::GetTzStr(createdAtHeader)),
		fmt::arg("lastLogin_defaultTz", utils::GetDefaultTzStr(lastLoginHeader)),
		fmt::arg("lastLogin_tz", utils::GetTzStr(lastLoginHeader)));
	// clang-format on
}

void AffiliateWork::OutputAffiliatesRowToCsv(const ResultRow& row, std::ostream& outputStream)
{
	if (!outputStream.good())
	{
		LERR_ << "AffiliateWork::OutputAffiliatesRowToCsv - The output stream is not in a valid "
				 "state";
		THROW(utils::BadRequest, "Could not export data");
	}

	// clang-format off
	outputStream <<
				 "\"" << utils::ParseColumn<int32_t>(row, TABLE_COL_ID)                        << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_FIRST_NAME)            << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_LAST_NAME)             << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_EMAIL)                 << "\"," <<
				 "\"" << utils::ParseColumn<bool>(row, TABLE_COL_EMAIL_VERIFICATION)           << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_SKYPE)                 << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_WALLET)                << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_COMMISSION_PERCENTAGE) << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_MASTER_COMMISSION)     << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_BALANCE)               << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_CREATED_AT_DEF_TZ)     << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_CREATED_AT_TZ)         << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_LAST_LOGIN_DEF_TZ)     << "\"," <<
				 "\"" << utils::ParseColumn<std::string>(row, TABLE_COL_LAST_LOGIN_TZ)         << "\"\n";
	// clang-format on
}

data::Affiliate AffiliateWork::Get(const data::Id affiliateId) const
{
	db::data::AffiliateFilter affiliateFilter;
	affiliateFilter.affiliateId_ = affiliateId;
	db::data::Affiliates affiliates = Select(affiliateFilter);

	THROW_IF_FALSE(
		!affiliates.empty(),
		utils::DataNotFound,
		fmt::format("Affiliate {} not found", affiliateId));

	THROW_IF_FALSE(
		affiliates.size() == 1,
		utils::InternalServerError,
		fmt::format("Select returned {} rows for affiliateId {}", affiliates.size(), affiliateId));

	return affiliates.front();
}

data::Affiliate AffiliateWork::Update(const data::Affiliate& account) const
{
	if (!account.id_)
	{
		THROW(utils::BadRequest, "Invalid affiliate id, update is not possible");
	}

	std::vector<std::string> values;

	{
		using namespace fmt;

		if (account.id_)
		{
			values.push_back(format("{} = {}", column::Id, *account.id_));
		}
		if (account.email_)
		{
			values.push_back(format("{} = '{}'", column::Email, Esc(*account.email_)));
		}
		if (account.password_)
		{
			values.push_back(format("{} = '{}'", column::Password, Esc(*account.password_)));
		}
		if (account.firstName_)
		{
			values.push_back(format("{} = '{}'", column::FirstName, Esc(*account.firstName_)));
		}
		if (account.lastName_)
		{
			values.push_back(format("{} = '{}'", column::LastName, Esc(*account.lastName_)));
		}
		if (account.skype_)
		{
			values.push_back(format("{} = '{}'", column::Skype, Esc(*account.skype_)));
		}
		if (account.fee_)
		{
			values.push_back(format("{} = {}", column::Fee, *account.fee_));
		}
		if (account.inviterFee_)
		{
			values.push_back(format("{} = {}", column::InviterFee, *account.fee_));
		}
		if (account.userFee_)
		{
			values.push_back(format("{} = {}", column::UserFee, *account.fee_));
		}
		if (account.wallet_)
		{
			values.push_back(format("{} = '{}'", column::Wallet, Esc(*account.wallet_)));
		}
		if (account.emailVerification_)
		{
			values.push_back(
				format("{} = {}", column::EmailVerification, *account.emailVerification_));
		}
		if (account.status2fa_)
		{
			values.push_back(format("{} = {}", column::Status2fa, *account.status2fa_));
		}
		if (account.createdAt_)
		{
			values.push_back(format("{} = '{}'", column::CreatedAt, Esc(*account.createdAt_)));
		}
		if (account.lastLoginAt_)
		{
			values.push_back(format("{} = '{}'", column::LastLoginAt, Esc(*account.lastLoginAt_)));
		}
		if (account.inviter_)
		{
			values.push_back(format("{} = '{}'", column::Inviter, *account.inviter_));
		}
		if (account.isAffiliate_)
		{
			values.push_back(format("{} = {}", column::IsAffiliate, *account.isAffiliate_));
		}
		if (account.isInviter_)
		{
			values.push_back(format("{} = {}", column::IsInviter, *account.isInviter_));
		}
		if (account.affiliateId_)
		{
			values.push_back(format("{} = {}", column::AffiliateId, *account.affiliateId_));
		}
		if (account.roleType_)
		{
			values.push_back(
				format("{} = '{}'", column::RoleType, db::data::RoleType2Str(*account.roleType_)));
		}
		if (account.secret_)
		{
			if (account.secret_.value().empty())
			{
				values.push_back(format("{} = NULL", column::Secret));
			}
			else
			{
				values.push_back(format("{} = '{}'", column::Secret, Esc(*account.secret_)));
			}
		}
		if (account.status_)
		{
			values.push_back(format("{} = '{}'",
				column::Status, db::data::AffiliateStatus2Str(*account.status_)));
		}
	}

	const auto query{ fmt::format(
		"UPDATE {} SET {} WHERE {} = {} RETURNING *",
		table::Name,
		utils::JoinStrList(values, ", "),
		column::Id,
		account.id_.value()) };

	const auto result{ Exec(query) };

	return ExtractAffiliate(*result.begin());
}

data::Affiliate AffiliateWork::UpdateBalance(const data::Id id, const double amount) const
{
	data::Affiliate account;
	account.id_ = id;

	const auto query{ fmt::format(
		"UPDATE {} SET {} = {} + {} WHERE {} = {} RETURNING *",
		table::Name,
		column::Balance,
		column::Balance,
		amount,
		column::Id,
		account.id_.value()) };

	const auto result{ Exec(query) };

	return ExtractAffiliate(*result.begin());
}

data::Affiliate AffiliateWork::ExtractAffiliate(const pqxx::row& row) const
{
	data::Affiliate account;

	try
	{
		account.id_ = row[column::Id].as<data::Id>();
		account.email_ = row[column::Email].as<std::string>();
		account.password_ = row[column::Password].as<std::string>();
		account.firstName_ = row[column::FirstName].as<std::string>();
		account.lastName_ = row[column::LastName].as<std::string>();
		if (!row[column::Skype].is_null())
		{
			account.skype_ = row[column::Skype].as<std::string>();
		}
		account.fee_ = row[column::Fee].as<double>();
		account.inviterFee_ = row[column::InviterFee].as<double>();
		account.userFee_ = row[column::UserFee].as<double>();
		if (!row[column::Wallet].is_null())
		{
			account.wallet_ = row[column::Wallet].as<std::string>();
		}
		account.emailVerification_ = row[column::EmailVerification].as<bool>();
		account.status2fa_ = row[column::Status2fa].as<bool>();
		account.createdAt_ = row[column::CreatedAt].as<std::string>();
		if (!row[column::LastLoginAt].is_null())
		{
			account.lastLoginAt_ = row[column::LastLoginAt].as<std::string>();
		}
		account.balance_ = row[column::Balance].as<double>();
		if (!row[column::Inviter].is_null())
		{
			account.inviter_ = row[column::Inviter].as<data::Id>();
		}
		account.isAffiliate_ = row[column::IsAffiliate].as<bool>();
		account.isInviter_ = row[column::IsInviter].as<bool>();
		account.affiliateId_ = row[column::AffiliateId].as<data::Id>();
		if (!row[column::Secret].is_null())
		{
			account.secret_ = row[column::Secret].as<std::string>();
		}
		if (!row[column::ApiKey].is_null())
		{
			account.apiKey_ = row[column::ApiKey].as<std::string>();
		}
		account.roleType_ = db::data::Str2RoleType(row[column::RoleType].as<std::string>());
		account.status_ = db::data::Str2AffiliateStatus(row[column::Status].as<std::string>());
	}
	catch (const pqxx::conversion_error& ex)
	{
		LERR_ << "Extracting affiliate data from db response failed. Reason: " << ex.what();
		THROW(utils::BadRequest, "Account data has been corrupted");
	}

	return account;
}

data::Affiliates AffiliateWork::ExtractAffiliates(pqxx::result&& result) const
{
	data::Affiliates accounts;
	{
		accounts.reserve(result.size());
	}

	for (auto&& row : result)
	{
		accounts.push_back(ExtractAffiliate(std::move(row)));
	}

	return accounts;
}

}	 // namespace db
