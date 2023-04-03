#include "db/data/Ranges.h"
#include "db/AffiliateTransactionWorkByAdmin.h"
#include "db/data/TransactionInfo.h"

#include <utility>

namespace db
{

AffiliateTransactionWorkByAdmin::AffiliateTransactionWorkByAdmin(Work w)
	: AffiliateTransactionWork{ std::move(w) }
{
}

data::TransactionInfo AffiliateTransactionWorkByAdmin::FillTransaction(
	const pqxx::const_result_iterator::reference row) const
{
	auto tmp{ ExtractTransactionInfo(row) };
	tmp.user_.email_ = row["email"].as<std::string>();
	tmp.user_.firstName_ = row["first_name"].as<std::string>();
	tmp.user_.lastName_ = row["last_name"].as<std::string>();
	return tmp;
}

}	 // namespace db
