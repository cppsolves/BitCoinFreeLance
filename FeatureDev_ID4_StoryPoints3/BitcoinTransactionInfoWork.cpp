#include "pch.h"

#include "db/BitcoinTransactionInfoWork.h"
#include "db/data/BitcoinTransactionInfo.h"
#include "db/data/Ranges.h"
#include "db/data/TransactionInfo.h"

namespace db
{

namespace table
{
static const std::string Name{ "bitcoin_transaction_info" };
}	 // namespace table

namespace column
{
const static std::string ExecutedAt{ "executed_at" };
const static std::string Hash{ "hash" };
const static std::string SenderWallet{ "sender_wallet" };
const static std::string TransacId{ "transaction_id" };
}	 // namespace column

BitcoinTransactionInfoWork::BitcoinTransactionInfoWork(Work w)
	: Work(std::move(w))
{
}

BitcoinTransactionInfoWork::BitcoinTransactionInfoWork(db::HandleShPtr handle)
	: Work{ std::move(handle) }
{
}

data::BitcoinTransactionInfo BitcoinTransactionInfoWork::ExtractBitcoinTransactionInfo(
	const ResultRow& row) const
{
	data::BitcoinTransactionInfo btcTrans{};

	btcTrans.transactionId_ = row[column::TransacId].as<data::Id>();

	if (!row[column::Hash].is_null())
	{
		btcTrans.hash_ = row[column::Hash].as<std::string>();
	}

	if (!row[column::SenderWallet].is_null())
	{
		btcTrans.senderWallet_ = row[column::SenderWallet].as<std::string>();
	}

	if (!row[column::ExecutedAt].is_null())
	{
		btcTrans.executedAt_ = row[column::ExecutedAt].as<data::Timestamp>();
	}

	return btcTrans;
}

data::Ids BitcoinTransactionInfoWork::GetIcTransactionIDs(const data::Transactions& icTrans) const
{
	// Represents the instacoins transactions.
	data::Ids icIds{};

	icIds.reserve(icTrans.size());

	for (const auto& currTrx : icTrans)
	{
		if (currTrx.id_.has_value())
		{
			icIds.emplace_back(currTrx.id_.value());
		}
	}

	return icIds;
}

bool BitcoinTransactionInfoWork::Delete(const data::Transactions& icTrans) const
{
	const auto icIds{ GetIcTransactionIDs(icTrans) };
	if (icIds.empty())
	{
		return {};
	}

	const auto sql{ fmt::format(
		"DELETE FROM {} "
		"WHERE {} IN ({}) "
		"RETURNING *",
		table::Name,
		column::TransacId,
		utils::JoinStrList(icIds, ",")) };

	return !Exec(sql).empty();
}

bool BitcoinTransactionInfoWork::Insert(const data::Transactions& icTrans) const
{
	std::vector<std::string> values{};
	values.reserve(icTrans.size());

	for (const auto& currTrans : icTrans)
	{
		values.emplace_back(fmt::format(
			"({}, {}, {}, {})",
			currTrans.id_.value(),
			(currTrans.hash_.has_value() ? utils::Quote(currTrans.hash_.value()) : "NULL"),
			(currTrans.senderWallet_.has_value() ? utils::Quote(currTrans.senderWallet_.value())
												 : "NULL"),
			(currTrans.executedAt_.has_value() ? utils::Quote(currTrans.executedAt_.value())
											   : "NULL")));
	}

	const auto sql{ fmt::format(
		"INSERT INTO {} ({}, {}, {}, {}) VALUES {} RETURNING *",
		table::Name,
		column::TransacId,
		column::Hash,
		column::SenderWallet,
		column::ExecutedAt,
		utils::JoinStrList(values, ", ")) };

	return !Exec(sql).empty();
}

std::map<data::Id, data::BitcoinTransactionInfo> BitcoinTransactionInfoWork::Select(
	const data::Transactions& icTrans) const
{
	const auto icIds{ GetIcTransactionIDs(icTrans) };
	if (icIds.empty())
	{
		return {};
	}

	const auto sql{ fmt::format(
		"SELECT     * "
		"FROM       {} "
		"WHERE      {} IN ({}) ",
		table::Name,
		column::TransacId,
		utils::JoinStrList(icIds, ",")) };

	const auto rows{ Exec(sql) };

	std::map<data::Id, data::BitcoinTransactionInfo> result{};
	for (const auto& currRow : rows)
	{
		const auto btcTransInfo{ ExtractBitcoinTransactionInfo(currRow) };
		result.insert({ btcTransInfo.transactionId_.value(), btcTransInfo });
	}

	return result;
}

}	 // namespace db
