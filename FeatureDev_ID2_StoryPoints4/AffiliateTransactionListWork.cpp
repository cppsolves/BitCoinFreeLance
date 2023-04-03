#include "pch.h"

#include "db/AffiliateTransactionListWork.h"
#include "db/SumAffiliateTransactions.h"
#include "db/utils/BuildDateRangeFilterForTimeZone.h"

#include "db/data/AffiliateTransaction.h"
#include "db/data/Pagination.h"
#include "db/data/Ranges.h"
#include "db/data/Types.h"

#include "db/utils/WhereClauseBuilder.h"

#include "utils/StatsConfig.h"

namespace db
{
namespace table
{

static const std::string AffiliateInfo{ "affiliate_info" };
static const std::string TransactionInfo{ "transaction_info" };
static const std::string UserAccountInfo{ "user_account_info" };

}	 // namespace table

namespace column
{

static const std::string AffiliateId{ "affiliate_id" };
static const std::string AffiliateName{ "affiliate_name" };
static const std::string AffiliateFirstName{ "first_name" };
static const std::string AffiliateLastName{ "last_name" };
static const std::string CreatedAt{ "created_at" };
static const std::string PerformedAt{ "performed_at" };
static const std::string SignUps{ "count" };
static const std::string Status{ "status" };

}	 // namespace column

namespace sql
{

static const std::string SelectTransactionInCurrencyWithStatus{ R"(
SELECT     transaction_info.id, 
           ( ( transaction_info.price / oexrate.rate ) * cexrate.rate ) price 
FROM       transaction_info transaction_info 
INNER JOIN exchange_rate oexrate 
ON         ( 
                      transaction_info.performed_at :: date {0} ) :: date = oexrate.day
AND        transaction_info.currency = oexrate.currency 
INNER JOIN exchange_rate cexrate 
ON         ( 
                      transaction_info.performed_at :: date {0} ) :: date = cexrate.day
AND        cexrate.currency = '{1}' 
WHERE      transaction_info.status IN ( {2} )
)" };

static const std::string SelectFinishedTransactionInCurrencyWithStatus{ R"(
SELECT     transaction_info.id, 
           ( ( transaction_info.price / oexrate.rate ) * cexrate.rate ) price 
FROM       transaction_info transaction_info 
INNER JOIN exchange_rate oexrate 
ON         ( 
                      transaction_info.executed_at :: date {0} ) :: date = oexrate.day
AND        transaction_info.currency = oexrate.currency 
INNER JOIN exchange_rate cexrate 
ON         ( 
                      transaction_info.executed_at :: date {0} ) :: date = cexrate.day
AND        cexrate.currency = '{1}' 
WHERE      transaction_info.status IN ( {2} )
)" };

// Placeholders with table with success/failed/pending transactions
const std::string SelectGrid{
	R"(
SELECT          affiliate_info.affiliate_id                                                      AS affiliate_id,
                CONCAT(affiliate_info.first_name, ' ', affiliate_info.last_name)                 AS affiliate_name,
                COALESCE(Count(success), 0)                                                      AS success_count,
                Round(COALESCE(Sum(success.price), 0.0)::numeric, 2)                             AS success_price,
                COALESCE(Count(failed), 0)                                                       AS failed_count,
                Round(COALESCE(Sum(failed.price), 0.0)::numeric, 2)                              AS failed_price,
                COALESCE(Count(pending), 0)                                                      AS pending_count,
                Round(COALESCE(Sum(pending.price), 0.0)::numeric, 2)                             AS pending_price
FROM            affiliate_info affiliate_info 
INNER JOIN      transaction_info transaction_info 
ON              transaction_info.affiliate_id = affiliate_info.affiliate_id 
LEFT OUTER JOIN ({}) success 
ON              success.id = transaction_info.id 
LEFT OUTER JOIN ({}) failed 
ON              failed.id = transaction_info.id 
LEFT OUTER JOIN ({}) pending 
ON              pending.id = transaction_info.id 
{} 
GROUP BY        affiliate_info.affiliate_id , 
                CONCAT(affiliate_info.first_name, ' ', affiliate_info.last_name)
ORDER BY        success_price DESC {}
)"
};

const std::string SelectSignUps{
	R"(
SELECT     user_account_info.affiliate_id AS affiliate_id, 
           affiliate_name, 
           Count(user_account_info.*) AS count 
FROM       user_account_info user_account_info 
INNER JOIN 
           ( 
                  SELECT affiliate_id, 
                                CONCAT(first_name, ' ', last_name ) affiliate_name 
                  FROM   affiliate_info) affiliate_info 
ON         user_account_info.affiliate_id = affiliate_info.affiliate_id {} 
GROUP BY   user_account_info.affiliate_id, 
           affiliate_info.affiliate_name 
ORDER BY   user_account_info.affiliate_id ;
)"
};

// Placeholders with table with success/failed/pending transactions
const std::string SelectNonAffiliates{
	R"(
SELECT          COALESCE(Count(success), 0)                          AS success_count, 
                Round(COALESCE(Sum(success.price), 0.0)::numeric, 2) AS success_price, 
                COALESCE(Count(failed), 0)                           AS failed_count, 
                Round(COALESCE(Sum(failed.price), 0.0)::numeric, 2)  AS failed_price, 
                COALESCE(Count(pending), 0)                          AS pending_count, 
                Round(COALESCE(Sum(pending.price), 0.0)::numeric, 2) AS pending_price 
FROM            transaction_info transaction_info 
LEFT OUTER JOIN ({}) success 
ON              success.id = transaction_info.id 
LEFT OUTER JOIN ({}) failed 
ON              failed.id = transaction_info.id 
LEFT OUTER JOIN ({}) pending 
ON              pending.id = transaction_info.id 
{}
)"
};

const std::string CountSignUps{
	R"(
SELECT    Count(*) 
FROM      user_account_info 
LEFT JOIN affiliate_info 
ON        affiliate_info.affiliate_id = user_account_info.affiliate_id 
{}
)"
};

const std::string ChartQuery{
	R"(
SELECT    (transaction_info.performed_at {0} )::date AS performed_at, 
          round(COALESCE(sum(failed.price), 0.0)::numeric, 2)                 price_failed, 
          COALESCE(count(failed.price), 0)                                    count_failed, 
          round(COALESCE(sum(pending.price), 0.0)::numeric, 2)                price_pending, 
          COALESCE(count(pending.price), 0)                                   count_pending 
FROM      transaction_info transaction_info 
LEFT JOIN ({1}) failed 
ON        failed.id = transaction_info.id 
LEFT JOIN ({2}) pending 
ON        pending.id = transaction_info.id 
{3} 
GROUP BY  (transaction_info.performed_at {0})::date
)"
};

const std::string FinishedTransactionsChartQuery{
	R"(
SELECT   (transaction_info.executed_at {0} )::date AS executed_at, 
	round(COALESCE(sum(finished.price), 0.0)::numeric, 2)               price_finished
FROM      transaction_info transaction_info 
LEFT JOIN (
SELECT     transaction_info.id, 
	 ( ( transaction_info.price / oexrate.rate ) * cexrate.rate ) price 
FROM       transaction_info transaction_info 
INNER JOIN exchange_rate oexrate 
ON         ( 
				transaction_info.executed_at :: date {0} ) :: date = oexrate.day
AND        transaction_info.currency = oexrate.currency 
INNER JOIN exchange_rate cexrate 
ON         ( 
				transaction_info.executed_at :: date {0} ) :: date = cexrate.day
AND        cexrate.currency = '{1}'
WHERE      transaction_info.status IN ( 'FINISHED' )
) finished 
ON        finished.id = transaction_info.id 
{2}
GROUP BY (transaction_info.executed_at {0})::date
	)"
};

}	 // namespace sql

AffiliateTransactionListWork::AffiliateTransactionListWork(HandleShPtr handle)
	: Work{ std::move(handle) }
{
}

std::vector<data::AffiliateTransaction> AffiliateTransactionListWork::ExtractRows(
	const db::Result& result, const std::map<data::Id, std::uint32_t>& signups) const
{
	std::vector<data::AffiliateTransaction> transactions{};

	transactions.reserve(result.size());

	for (const auto& row : result)
	{
		data::AffiliateTransaction transaction{};
		transaction.affiliateId_ = row[column::AffiliateId].as<data::Id>();

		std::uint32_t signUps{ 0 };

		if (transaction.affiliateId_)
		{
			const auto signUpIt{ signups.find(transaction.affiliateId_.value()) };
			signUps = signUpIt->second;
		}

		transaction.affiliateName_ = row[column::AffiliateName].as<std::string>();
		transaction.stats_ = data::AffiliateTransactionStats{
			signUps,
			data::AffiliateTransactionSum{ row["pending_count"].as<std::uint32_t>(),
										   row["pending_price"].as<double>() },
			data::AffiliateTransactionSum{ row["success_count"].as<std::uint32_t>(),
										   row["success_price"].as<double>() },
			data::AffiliateTransactionSum{ row["failed_count"].as<std::uint32_t>(),
										   row["failed_price"].as<double>() }
		};

		transactions.push_back(std::move(transaction));
	}

	return transactions;
}

std::string AffiliateTransactionListWork::BuildWhereClause(
	const data::AffiliateTransactionFilter& filter) const
{
	db::WhereClauseBuilder builder{ GetWork() };
	builder.AddCondition(
		fmt::format("{}.{}", table::AffiliateInfo, column::AffiliateId),
		filter.affiliateId_,
		WhereClauseBuilder::Comparison::EQ,
		"'");
	builder.AddNotNull(fmt::format("{}.{}", table::TransactionInfo, column::AffiliateId));

	if (filter.affiliateName_)
	{
		builder.AddRawCondition(
			fmt::format(
				"CONCAT ({0}.{1}, ' ', {0}.{2})",
				table::AffiliateInfo,
				column::AffiliateFirstName,
				column::AffiliateLastName),
			fmt::format("'%{}%'", Esc(filter.affiliateName_.value())),
			WhereClauseBuilder::Comparison::ILIKE);
	}

	const auto stringWhereClause{ builder.GetString() };

	const auto timeRangeClause{ fmt::format(
		"(({} AND {}.status <> 'FINISHED') OR ({} AND {}.status = 'FINISHED'))",
		BuildDateRangeFilterForTimeZone(column::PerformedAt, filter.performedAt_),
		table::TransactionInfo,
		BuildDateRangeFilterForTimeZone("executed_at", filter.performedAt_),
		table::TransactionInfo) };

	return stringWhereClause + " AND " + timeRangeClause;
}

std::string AffiliateTransactionListWork::PrepareSelectTransactionsQuery(
	const data::AffiliateTransactionFilter& filter,
	const db::data::Currency currency,
	data::Pagination& pagination) const
{
	const std::string query{ fmt::format(
		sql::SelectGrid,
		SelectFinishedTransactionsQuery(currency),
		SelectFailedTransactionsQuery(currency),
		SelectPendingTransactionsQuery(currency),
		BuildWhereClause(filter),
		data::PaginationToStr(pagination)) };

	return query;
}

db::WhereClauseBuilder AffiliateTransactionListWork::GetSignUpsWhereClause(
	const data::AffiliateTransactionFilter& filter) const
{
	db::WhereClauseBuilder builder{ GetWork() };
	builder.AddCondition(
		fmt::format("{}.{}", table::AffiliateInfo, column::AffiliateId),
		filter.affiliateId_,
		WhereClauseBuilder::Comparison::EQ,
		"'");
	builder.AddDateRangeConditionWithTimeZone(
		fmt::format("{}.{}", table::UserAccountInfo, column::CreatedAt), filter.performedAt_);

	builder.AddNotNull(fmt::format("{}.{}", table::UserAccountInfo, column::AffiliateId));

	std::string active{ "ACTIVE" };
	builder.AddCondition(
		fmt::format("{}.{}", table::UserAccountInfo, column::Status),
		active,
		WhereClauseBuilder::Comparison::EQ,
		"'");
	return builder;
}

std::string AffiliateTransactionListWork::GetSignUpsQuery(
	const data::AffiliateTransactionFilter& filter) const
{
	db::WhereClauseBuilder builder{ GetSignUpsWhereClause(filter) };

	if (filter.affiliateName_)
	{
		builder.AddRawCondition(
			fmt::format("{}.{}", table::AffiliateInfo, column::AffiliateName),
			fmt::format("'%{}%'", Esc(filter.affiliateName_.value())),
			WhereClauseBuilder::Comparison::ILIKE);
	};

	return fmt::format(sql::SelectSignUps, builder.GetString());
}

std::map<data::Id, std::uint32_t> AffiliateTransactionListWork::SelectSignUpsForGrid(
	const data::AffiliateTransactionFilter& filter) const
{
	const auto getSignUpsQuery{ GetSignUpsQuery(filter) };
	const auto signups = Exec(getSignUpsQuery);
	std::map<data::Id, std::uint32_t> signUpsMap{};

	for (const auto row : signups)
	{
		const auto affiliateId = row[column::AffiliateId];
		const auto signUps = row[column::SignUps];

		if (affiliateId.is_null() || signUps.is_null())
		{
			LERR_ << "Empty record received from db!";
			return {};
		}

		signUpsMap.emplace(affiliateId.as<data::Id>(), signUps.as<std::uint32_t>());
	}

	return signUpsMap;
}

std::string AffiliateTransactionListWork::PrepareNonAffiliateQuery(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	WhereClauseBuilder builder{ GetWork() };
	builder.AddNull(fmt::format("{}.{}", table::TransactionInfo, column::AffiliateId));
	const auto stringWhereClause{ builder.GetString() };

	const auto timeRangeClause{ fmt::format(
		"(({} AND {}.status <> 'FINISHED') OR ({} AND {}.status = 'FINISHED'))",
		BuildDateRangeFilterForTimeZone(column::PerformedAt, filter.performedAt_),
		table::TransactionInfo,
		BuildDateRangeFilterForTimeZone("executed_at", filter.performedAt_),
		table::TransactionInfo) };

	const auto whereClause{ stringWhereClause + " AND " + timeRangeClause };

	return { fmt::format(
		sql::SelectNonAffiliates,
		SelectFinishedTransactionsQuery(currency),
		SelectFailedTransactionsQuery(currency),
		SelectPendingTransactionsQuery(currency),
		whereClause) };
}

data::AffiliateTransaction AffiliateTransactionListWork::SelectNonAffiliates(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	const std::string NonAffiliateName{ "Non-Affiliated Traffic" };
	const data::Id NonAffiliateId{ 0 };
	const std::uint32_t ZeroSignUps{ 0 };

	const auto row = Exec(PrepareNonAffiliateQuery(filter, currency))[0];
	data::AffiliateTransaction transaction{};
	transaction.affiliateId_ = NonAffiliateId;
	transaction.affiliateName_ = NonAffiliateName;
	transaction.stats_ = data::AffiliateTransactionStats{
		ZeroSignUps,
		data::AffiliateTransactionSum{ row["pending_count"].as<std::uint32_t>(),
									   row["pending_price"].as<double>() },
		data::AffiliateTransactionSum{ row["success_count"].as<std::uint32_t>(),
									   row["success_price"].as<double>() },
		data::AffiliateTransactionSum{ row["failed_count"].as<std::uint32_t>(),
									   row["failed_price"].as<double>() }
	};
	return transaction;
}

std::vector<data::AffiliateTransaction> AffiliateTransactionListWork::SelectGrid(
	const data::AffiliateTransactionFilter& filter,
	const db::data::Currency currency,
	data::Pagination& pagination) const
{
	const auto selectTransactionsQuery{ PrepareSelectTransactionsQuery(
		filter, currency, pagination) };
	const auto transactionRows = Exec(selectTransactionsQuery);

	const auto signUps{ SelectSignUpsForGrid(filter) };

	std::vector<data::AffiliateTransaction> allTransactions{ SelectNonAffiliates(
		filter, currency) };

	const auto transactions{ ExtractRows(transactionRows, signUps) };
	std::move(transactions.begin(), transactions.end(), std::back_inserter(allTransactions));

	return allTransactions;
}

Result AffiliateTransactionListWork::CountSignUps(
	const data::AffiliateTransactionFilter& filter) const
{
	db::WhereClauseBuilder builder{ GetSignUpsWhereClause(filter) };

	if (filter.affiliateName_)
	{
		builder.AddRawCondition(
			fmt::format(
				"CONCAT ({}.{},' ',{}.{})",
				table::AffiliateInfo,
				column::AffiliateFirstName,
				table::AffiliateInfo,
				column::AffiliateLastName),
			fmt::format("'%{}%'", filter.affiliateName_.value()),
			WhereClauseBuilder::Comparison::ILIKE);
	}

	const std::string countSignUpsSql{ fmt::format(sql::CountSignUps, builder.GetString()) };

	const auto signups = Exec(countSignUpsSql);

	return signups;
}

data::AffiliateTransactionStats AffiliateTransactionListWork::SelectTotals(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	const query::SumAffiliateTransactions queryBuilder{ GetWork() };
	const auto sumFinishedSql{ queryBuilder.SumTotalFinished(filter, currency) };
	const auto sumFailedSql{ queryBuilder.SumTotalFailed(filter, currency) };
	const auto sumPendingSql{ queryBuilder.SumTotalPending(filter, currency) };
	const auto signups{ CountSignUps(filter) };

	if (signups[0][column::SignUps].is_null())
	{
		LERR_ << "SignUps count is null!";
		return {};
	}

	return data::AffiliateTransactionStats{ signups[0][column::SignUps].as<std::uint32_t>(),
											GetSum(sumPendingSql),
											GetSum(sumFinishedSql),
											GetSum(sumFailedSql) };
}

data::AffiliateTransactionSum AffiliateTransactionListWork::GetSum(
	const std::string& sqlQuery) const
{
	const auto result = Exec(sqlQuery);

	const auto count{ result[0][query::column::Count].is_null()
						  ? 0
						  : result[0][query::column::Count].as<std::uint32_t>() };
	const auto totalAmount{ result[0][query::column::TotalAmount].is_null()
								? 0
								: result[0][query::column::TotalAmount].as<double>() };

	return { count, totalAmount };
}

data::DateRange AffiliateTransactionListWork::SelectTimeRange() const
{
	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};

	const std::string query{ fmt::format(
		R"(
				SELECT 
				  (MIN(performed_at) at time zone '{0}' at time zone '{1}')::date as FROM,
				  (NOW() at time zone '{0}' at time zone '{1}')::date AS TO
				FROM transaction_info;
				)",
		defaultTimeZone,
		timeZone) };

	const auto result = Exec(query);
	const auto from{ result[0]["FROM"].as<data::Date>() };
	const auto to{ result[0]["TO"].as<data::Date>() };

	return data::DateRange{ from, to };
}

void AffiliateTransactionListWork::FillRecordsWithEmptyDatesForRange(
	data::AffiliateChartRecords& records, Poco::DateTime from, const Poco::DateTime& to) const
{
	while (from < to)
	{
		data::AffiliateChartRecord record{ Poco::DateTimeFormatter::format(from, "%Y-%m-%d"), 0.0 };
		records.push_back(std::move(record));
		from += Poco::Timespan::DAYS;
	}
}

std::string AffiliateTransactionListWork::TransformToString(
	const std::vector<data::TransactionStatus>& transactionStatuses) const
{
	const auto ToStrWithQuotes{ [](const auto& status) {
		return ::utils::Quote(data::TransactionStatus2Str(status));
	} };

	return ::utils::TransformAndJoinList(
		transactionStatuses.begin(), transactionStatuses.end(), ToStrWithQuotes, ",");
}

std::string AffiliateTransactionListWork::SelectFinishedTransactionsQuery(
	const db::data::Currency currency) const
{
	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};
	const auto timeZoneExpression{ fmt::format(
		"at time zone '{}' at time zone '{}'", defaultTimeZone, timeZone) };

	return fmt::format(
		sql::SelectFinishedTransactionInCurrencyWithStatus,
		timeZoneExpression,
		data::Currency2Str(currency),
		TransformToString({ data::TransactionStatus::FINISHED }));
}

std::string AffiliateTransactionListWork::SelectPendingTransactionsQuery(
	const db::data::Currency currency) const
{
	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};
	const auto timeZoneExpression{ fmt::format(
		"at time zone '{}' at time zone '{}'", defaultTimeZone, timeZone) };

	return fmt::format(
		sql::SelectTransactionInCurrencyWithStatus,
		timeZoneExpression,
		data::Currency2Str(currency),
		TransformToString({ data::TransactionStatus::PENDING,
							data::TransactionStatus::PREAUTH_FINISHED,
							data::TransactionStatus::PREAUTH_STARTED }));
}

std::string AffiliateTransactionListWork::SelectFailedTransactionsQuery(
	const db::data::Currency currency) const
{
	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};
	const auto timeZoneExpression{ fmt::format(
		"at time zone '{}' at time zone '{}'", defaultTimeZone, timeZone) };

	return fmt::format(
		sql::SelectTransactionInCurrencyWithStatus,
		timeZoneExpression,
		data::Currency2Str(currency),
		TransformToString(
			{ data::TransactionStatus::FAILED, data::TransactionStatus::PREAUTH_CANCELLED }));
}

std::string AffiliateTransactionListWork::PrepareChartQueryForPendingAndCancelled(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	WhereClauseBuilder builder{ GetWork() };
	builder.AddDateRangeConditionWithTimeZone(column::PerformedAt, filter.performedAt_);
	builder.AddNotNull(fmt::format("{}.{}", table::TransactionInfo, column::AffiliateId));

	if (!filter.performedAt_ || !filter.performedAt_->from_ || !filter.performedAt_->to_)
	{
		LERR_ << "Missing date range! Can't fill chart.";
		return {};
	}

	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};
	const auto timeZoneExpression{ fmt::format(
		"at time zone '{}' at time zone '{}'", defaultTimeZone, timeZone) };

	return fmt::format(
		sql::ChartQuery,
		timeZoneExpression,
		SelectFailedTransactionsQuery(currency),
		SelectPendingTransactionsQuery(currency),
		builder.GetString());
}

std::string AffiliateTransactionListWork::PrepareChartQueryForFinished(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	WhereClauseBuilder builder{ GetWork() };
	builder.AddDateRangeConditionWithTimeZone("executed_at", filter.performedAt_);
	builder.AddNotNull(fmt::format("{}.{}", table::TransactionInfo, column::AffiliateId));

	if (!filter.performedAt_ || !filter.performedAt_->from_ || !filter.performedAt_->to_)
	{
		LERR_ << "Missing date range! Can't fill chart.";
		return {};
	}

	const auto timeZone{ utils::StatsConfig::Singleton().timeRangesConfig_.timezone_ };
	const auto defaultTimeZone{
		utils::StatsConfig::Singleton().timeRangesConfig_.defaultTimezone_
	};
	const auto timeZoneExpression{ fmt::format(
		"at time zone '{}' at time zone '{}'", defaultTimeZone, timeZone) };

	return fmt::format(
		sql::FinishedTransactionsChartQuery,
		timeZoneExpression,
		data::Currency2Str(currency),
		builder.GetString());
}

void AffiliateTransactionListWork::PushBackRecord(
	data::AffiliateChartRecords& records,
	const double amount,
	const Poco::DateTime& dateIteratorFrom,
	const Poco::DateTime& date) const
{
	FillRecordsWithEmptyDatesForRange(records, dateIteratorFrom, date);
	data::AffiliateChartRecord record = { Poco::DateTimeFormatter::format(date, "%Y-%m-%d"),
										  amount };
	records.push_back(std::move(record));
}

data::AffiliateChart AffiliateTransactionListWork::SelectChart(
	const data::AffiliateTransactionFilter& filter, const db::data::Currency currency) const
{
	const auto chartQuery{ PrepareChartQueryForPendingAndCancelled(filter, currency) };
	const auto finishedChartQuery{ PrepareChartQueryForFinished(filter, currency) };
	const auto resultPeningFailed{ Exec(chartQuery) };
	const auto resultFinished{ Exec(finishedChartQuery) };

	data::AffiliateChartRecords finishedRecords{};
	data::AffiliateChartRecords failedRecords{};
	data::AffiliateChartRecords pendingRecords{};

	if (!filter.performedAt_ || !filter.performedAt_->from_ || !filter.performedAt_->to_)
	{
		LERR_ << "Missing date range! Can't select chart.";
		return {};
	}

	int timeZone{ 0 };
	auto dateIteratorFrom{ Poco::DateTime{
		Poco::DateTimeParser::parse("%Y-%m-%d", filter.performedAt_->from_.value(), timeZone) } };
	const auto dateIteratorTo{ Poco::DateTime{ Poco::DateTimeParser::parse(
								   "%Y-%m-%d", filter.performedAt_->to_.value(), timeZone) }
							   + Poco::Timespan::DAYS };

	// Finished transactions
	for (const auto& row : resultFinished)
	{
		Poco::DateTime date{ Poco::DateTimeParser::parse(
			"%Y-%m-%d", row["executed_at"].as<data::Date>(), timeZone) };

		PushBackRecord(finishedRecords, row["price_finished"].as<double>(), dateIteratorFrom, date);

		dateIteratorFrom = (date + Poco::Timespan::DAYS);
	}

	// Failed and pending transactions
	dateIteratorFrom = Poco::DateTime{ Poco::DateTimeParser::parse(
		"%Y-%m-%d", filter.performedAt_->from_.value(), timeZone) };
	for (const auto& row : resultPeningFailed)
	{
		Poco::DateTime date{ Poco::DateTimeParser::parse(
			"%Y-%m-%d", row[column::PerformedAt].as<data::Date>(), timeZone) };

		PushBackRecord(failedRecords, row["price_failed"].as<double>(), dateIteratorFrom, date);
		PushBackRecord(pendingRecords, row["price_pending"].as<double>(), dateIteratorFrom, date);

		dateIteratorFrom = (date + Poco::Timespan::DAYS);
	}

	FillRecordsWithEmptyDatesForRange(finishedRecords, dateIteratorFrom, dateIteratorTo);
	FillRecordsWithEmptyDatesForRange(failedRecords, dateIteratorFrom, dateIteratorTo);
	FillRecordsWithEmptyDatesForRange(pendingRecords, dateIteratorFrom, dateIteratorTo);

	const data::AffiliateChart chart{ std::move(finishedRecords),
									  std::move(failedRecords),
									  std::move(pendingRecords) };

	return chart;
}

}	 // namespace db
