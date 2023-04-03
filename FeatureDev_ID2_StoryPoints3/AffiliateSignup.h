#pragma once

namespace db
{

class AffiliateWork;

namespace data
{

class Affiliate;

}	 // namespace data
}	 // namespace db

namespace insta
{

class AffiliateSignup
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	AffiliateSignup();
	//! Destructor.
	~AffiliateSignup();

	//
	// Public interface.
	//
public:
	//! Creates and registers a affiliate user inside the system.
	nl::json Exec(db::data::Affiliate&& info, const std::string& sessionId) const;

	//
	// Private methods.
	//
private:
	//! Generate API key
	std::string GenerateKey() const;
	//! Creates JWT signature.
	std::string GetJwt(const db::data::Id id, const std::string& sessionId) const;

	//
	// Private data members.
	//
private:
	//! Connection to db.
	db::ConnectionUnit connection_;
	//! Db worker.
	std::unique_ptr<db::AffiliateWork> work_;
	//! Role for Affiliates
	db::data::RoleType roleType_;
};

}	 // namespace insta
