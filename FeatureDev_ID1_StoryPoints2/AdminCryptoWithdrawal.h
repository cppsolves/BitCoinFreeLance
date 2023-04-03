#pragma once

#include "db/data/Types.h"
#include "db/data/CryptoWithdrawalInfo.h"

namespace insta
{

class AdminCryptoWithdrawal
{
	//
	// Construction and destruction.
	//
public:
	AdminCryptoWithdrawal();

	//
	// Public interface
	//
public:
	//! Cancel a withdrawal by admin
	//!
	//! \param data Json with the request data parameters.
	//!
	//! \return     Response body in json format
	//!
	//! \throw utils::BadRequest if faced with an invalid request; std::runtime_error if faced with
	//! an unexpected error.
	static nl::json WithdrawalCancel(const nl::json& data);

	//! Returns (to an administrator) a list of user's problematic withdrawal requests
	//!
	//! \param data Json with the request data parameters.
	//! \param lang User language.
	//!
	//! \return     Response body in json format. It contains the list of problematic withdrawals
	//! and pagination
	//!
	//! \throw utils::BadRequest if faced with an invalid request; std::runtime_error if faced with
	//! an unexpected error.
	static nl::json WithdrawalProblematicGet(const nl::json& data, const std::string& lang);

	//! Returns amount problematic withdrawals in the crypto_withdrawal_info table
	//! \return Response body in json format, with key-value: '"count": <NUMBER>'
	//!
	//! \throw utils::InternalServerError if can't execute DB query; std::runtime_error in case of
	//! an unexpected error.
	static nl::json WithdrawalProblematicGetCount();

	//! Clears problematic withdrawals specified in json data by scheduling them for execution.
	//! Clearing problematic withdrawals results in means making following update to
	//! the crypto_withdrawal_info table: setting withdrawal status to WITHDRAWAL_PENDING,
	//! and all of the following to NULL: withdrawal hash, source wallet, succeeded at,
	//! internal error message, user error message.
	//!
	//! \throw utils::InternalServerError if can't execute DB query
	//! \throw utils::BadRequest if request json data has wrong format or content
	//! \throw std::runtime_error in case of an unexpected error
	//! \throw utils::UnprocessableEntity if Redis lock can not be acquired
	static void WithdrawalProblematicClear(const nl::json& data, const db::data::Id superadminId);

	//! Performs hash update for problematic withdrawals, by marking them as already sent.
	//! It swaps withdrawal customHash to a new one, and sets withdrawal status to
	//! WITHDRAWAL_CRYPTO_TX_SENT, it also sets internal error message, and user error message
	//! to NULL.
	//!
	//! \throw utils::InternalServerError if can't execute DB query
	//! \throw utils::BadRequest if request json data has wrong format or content
	//! \throw std::runtime_error in case of an unexpected error
	//! \throw utils::UnprocessableEntity if Redis lock can not be acquired
	static void WithdrawalProblematicHashUpdate(
		const nl::json& data, const db::data::Id superadminId);

private:
	//! Parse system-generated custom hash
	static std::string ParseCustomHashString(const std::string& hash);

	//! Check if jsonData matches checksum, \throw utils::BadRequest on mismatch
	static void ValidateChecksum(const nl::json& jsonData, const std::string& checksum);

	//! Write 'activities_v2' log entry for a problematic withdrawal
	static void LogProblematicWithdrawalActivity(
		const db::Work& work,
		const db::data::Id superadminId,
		const db::data::CryptoWithdrawals& original,
		const db::data::CryptoWithdrawals& updated);
};

}	 // namespace insta
