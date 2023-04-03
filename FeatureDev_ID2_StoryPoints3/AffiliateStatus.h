#pragma once

namespace insta
{

class AffiliateStatus
{
	//
	// Public interface.
	//
public:
	//! Changes status of an affiliate by id to DISABLED.
	void Disable(const db::data::Id id) const;
	//! Changes status of an affiliate by id to ARCHIVED.
	void Archive(const db::data::Id id) const;
	//! Changes status back to ACTIVE and sets affiliate fee.
	void Reactivate(const db::data::Id id, const double fee) const;

	//
	// Private data members.
	//
private:
	//! Updates the affiliate in the database.
	void UpdateAffiliate(const db::AffiliateWork& affiliateWork, db::data::Affiliate& affiliate,
		 const db::data::AffiliateStatus status, const double fee) const;
};

}	// namespace insta