#pragma once

namespace db
{
class RefundInfoWork;
namespace data
{
class RefundInfo;
}
}	 // namespace db

namespace insta
{

class AdminRefund
{
	//
	// Construction and destruction.
	//
public:
	AdminRefund();

	//
	// Public interface
	//
public:
	//! Get refunds list for admin.
	//! \param data Json with the request data parameters.
	//! \return Response body in json format.
	static nl::json GetRefundList(const nl::json& data);

	//! Get refund log list for admin.
	//! \param data Json with the request data parameters.
	//! \return Response body in json format.
	static nl::json GetRefundLog(const nl::json& data);

	//! Make refund.
	//! \param data Json with the request data parameters.
	//! \param adminId Id of the admin that's initiating the automatic refund.
	//! \param ip Ip address of the admin.
	static nl::json MakeRefund(const nl::json& data, db::data::Id adminId, const std::string& ip);

	//! Set refund status for admin.
	//! \param data Json with the request data parameters.
	//! \param adminId Id of the admin that's changing the refund status.
	//! \param ip Ip address of the admin.
	static nl::json SetRefundStatus(
		const nl::json& data, const db::data::Id adminId, const std::string& ip);

private:
	static void SendEmail(
		const db::data::TransactionInfo& trxInfo,
		const db::data::RefundInfo& refund,
		const db::data::RefundStatus& status,
		const bool shouldSend);
	static void SetRejected(
		db::RefundInfoWork& refundInfoWork,
		db::data::RefundInfo& refund,
		db::data::Id adminId,
		const std::string& note);
	static void SetApproved(
		db::RefundInfoWork& refundInfoWork,
		db::data::RefundInfo& refund,
		db::data::Id adminId,
		const std::string& note);
	static void SetProcessed(
		db::RefundInfoWork& refundInfoWork,
		db::data::RefundInfo& refund,
		db::data::Id adminId,
		const std::string& note);
};

}	 // namespace insta
