#pragma once

#include <memory>

namespace db
{

class AffiliateTransactionWork;

}	 // namespace db

namespace insta
{

class AffiliateTransaction
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	AffiliateTransaction();
	//! Destructor.
	~AffiliateTransaction();

	//
	// Public interface.
	//
public:
	//! Returns list of transaction.
	nl::json Get(db::data::TransactionInfoFilter filter, db::data::Pagination pagination);

protected:
	//! Creates work of propper type
	virtual std::shared_ptr<db::AffiliateTransactionWork> CreateWork();

	//
	// Private methods.
	//
private:
	//! Returns affiliate id of affiliate.
	db::data::Id GetAffiliatesAffiliateId(const db::data::TransactionInfoFilter& filter) const;
	//! Applies needed commission based on affiliateId.
	db::data::Transactions ResolveCommissions(
		db::data::Transactions&& transactions, const db::data::Id& affiliateId);

	//
	// Private data members.
	//
private:
	//! Connection to the database.
	db::ConnectionUnit conn_;

protected:
	//! Base worker for requests.
	db::Work work_;
};

}	 // namespace insta
