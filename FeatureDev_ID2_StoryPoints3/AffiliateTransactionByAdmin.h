#pragma once

#include "insta/AffiliateTransaction.h"

namespace insta
{

//!
//! Instance of AffiliateTransaction class which creates
//! appriopriate work for admin user
//!
class AffiliateTransactionByAdmin : public AffiliateTransaction
{
public:
	//! Constructor
	AffiliateTransactionByAdmin();

private:
	//! Creates AffiliateTransactionWorkByAdmin work for admin
	std::shared_ptr<db::AffiliateTransactionWork> CreateWork() override;
};

}	 // namespace insta
