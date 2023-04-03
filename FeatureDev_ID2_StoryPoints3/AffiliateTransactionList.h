#pragma once

namespace db
{

class AffiliateTransactionListWork;

namespace data
{

class AffiliateTransactionFilter;

}	 // namespace data
}	 // namespace db

namespace insta
{

//!
//! The AffiliateTransactionList class
//!
class AffiliateTransactionList
{
public:
	//! Constructor
	AffiliateTransactionList();
	//! Destructor
	virtual ~AffiliateTransactionList();

	//! Returns affiliate transactions volume
	nl::json Get(
		db::data::AffiliateTransactionFilter filter,
		const db::data::Currency currency,
		db::data::Pagination pagination) const;

	//! Return affiliate transaction volume as CSV
	std::string GetCsv(
		db::data::AffiliateTransactionFilter filter, const db::data::Currency currency) const;

private:
	//! Sets date range
	void SetDateRange(db::data::AffiliateTransactionFilter& filter) const;

private:
	//! Connection
	const db::ConnectionUnit connection_;
};

}	 // namespace insta
