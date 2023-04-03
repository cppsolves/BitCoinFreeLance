#pragma once

namespace db
{

class AffiliateWork;

}	 // namespace db

namespace insta
{

class AffiliateSecurity
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	AffiliateSecurity() = default;

	//! Destructor.
	~AffiliateSecurity() = default;

	//
	// Public interface.
	//
public:
	//! Changes password of certain affiliate.
	void ChangePassword(
		const db::data::Id id,
		const std::string& newPwd,
		const std::string& currentPwd) const;
	//! Resets affiliate password.
	void ResetPassword(
		const db::data::Id id,
		const std::string& newPwd) const;
};

}	 // namespace insta
