#include "pch.h"

#include "db/AffiliateTransactionWork.h"
#include "db/data/Pagination.h"
#include "db/data/Ranges.h"
#include "db/data/TransactionInfo.h"
#include "db/utils/WhereClauseBuilder.h"

namespace db
{

AffiliateTransactionWork::AffiliateTransactionWork(Work w)
	: TransactionInfoWork(w)
{
}

AffiliateTransactionWork::AffiliateTransactionWork(db::HandleShPtr handle)
	: TransactionInfoWork{ handle }
{
}

size_t AffiliateTransactionWork::Count(const data::TransactionInfoFilter& filter) const
{
	const auto sql{ fmt::format("SELECT COUNT(*) FROM affiliate_tx_view ") };
	const auto result{ Exec(GetSelectQuery(sql, filter)) };

	return result[0][0].as<size_t>(0);
}

std::string AffiliateTransactionWork::GetSelectQuery(
	const std::string& query, const data::TransactionInfoFilter& filter) const
{
	// NOTE: This function uses affiliate_tx_view (useView=true)
	utils::Require(filter.transactionInfo_.user_.affiliateId_, "affiliateId");
	const auto affiliateId{ *filter.transactionInfo_.user_.affiliateId_ };

	db::WhereClauseBuilder builder{ GetWork() };
	BuildFilter(builder, filter, "affiliate_tx_view");
	const auto filterStr{ builder.GetString() };

	// Returns a list of affiliate IDs that have been invited by a specific affiliate
	// including his own ID.
	const std::string affiliateIdSelect{ fmt::format(
		"SELECT affiliate_id FROM affiliate_info WHERE"
		" inviter = {} OR affiliate_id = {}",
		affiliateId,
		affiliateId) };

	return fmt::format(
		"{}{}{} "
		"("
		"(affiliate_id IN ({})) OR "
		"((affiliate_id IS NULL) AND "
		"(user_id IN (SELECT id FROM user_account_info WHERE affiliate_id IN ({}))))"
		")",
		query,
		filterStr,
		filterStr.empty() ? " WHERE " : " AND ",
		affiliateIdSelect,
		affiliateIdSelect);
}

data::Transactions AffiliateTransactionWork::Select(
	data::Pagination& pagination, const data::TransactionInfoFilter& filter) const
{
	pagination.count_ = Count(filter);
	if (*pagination.count_ == 0)
	{
		return data::Transactions{};
	}

	auto sql{ GetSelectQuery(fmt::format("SELECT * FROM affiliate_tx_view "), filter) };
	sql += " ORDER BY id DESC";
	sql += " " + data::PaginationToStr(pagination);

	const auto result{ Exec(sql) };

	return FillTransactions(result);
}

data::Transactions AffiliateTransactionWork::FillTransactions(const Result& result) const
{
	data::Transactions transactions;
	transactions.reserve(result.size());
	for (const auto& row : result)
	{
		transactions.push_back(FillTransaction(row));
	}
	return transactions;
}

data::TransactionInfo AffiliateTransactionWork::FillTransaction(
	const pqxx::const_result_iterator::reference row) const
{
	return ExtractTransactionInfo(row);
}

}	 // namespace db
