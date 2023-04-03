#pragma once

#include "db/data/Types.h"

namespace insta
{

class AdminUserSummary
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	AdminUserSummary();

	//
	// Public interface
	//
public:
	//! Logins the user into the system.
	nl::json Exec(const nl::json& jsdata) const;

	//
	// Private methods.
	//
private:
	// Return transactionInfo data for a given user
	nl::json GetTransactionInfo(
		const db::ConnectionUnit& conn,
		const db::data::Id id,
		const std::optional<std::string>& currency) const;
	// Return documentInfo data for a given user
	nl::json GetDocumentInfo(const db::ConnectionUnit& conn, const db::data::Id id) const;
	// Return proof-of-identification and proof_of-address summary data for a given user
	std::tuple<nl::json, nl::json, nl::json> GetPersonalSelfieInterviewSummary(
		const db::ConnectionUnit& conn, const db::data::Id id) const;
	// Return credit cards summary data for a given user
	nl::json GetCreditCardsSummary(const db::ConnectionUnit& conn, const db::data::Id id) const;
	// Return count of the notes for a given user
	std::uint32_t GetNotesCount(const db::ConnectionUnit& conn, const db::data::Id id) const;
};

}	 // namespace insta
