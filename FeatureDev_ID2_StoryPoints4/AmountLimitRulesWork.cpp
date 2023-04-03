#include <pch.h>

#include "db/AmountLimitRulesWork.h"
#include "db/C6Work.h"
#include "db/UserInfoWork.h"
#include "db/data/UserInfo.h"
#include "db/ExchangeRates.h"
#include "utils/Chrono.h"
#include "utils/Exception.h"

namespace db
{

AmountLimitRulesWork::AmountLimitRulesWork(Work w)
	: Work(std::move(w))
{
}

AmountLimitRulesWork::AmountLimitRulesWork(HandleShPtr handle)
	: Work{ std::move(handle) }
{
}

void AmountLimitRulesWork::DetermineUserFlags(
	const db::data::Id userId, db::data::UserStatusFlags& userStatusFlags)
{
	const auto& stmt = DetermineFlagsQuery(userId);
	const auto& rs = Exec(stmt);

	if (rs.size() != 1)
	{
		LERRWT_ << fmt::format(
			"Error getting flags for user {}, using the following query '{}'",
			userId,
			stmt.Dump(true));
		THROW(utils::InternalServerError, "Error getting user flags");
	}

	const auto& rec = rs.front();

	data::DocumentStatusOpt surveyStatus;
	data::SumSubMessageTypeOpt lastSumSubMsgType;
	TryExtract(rec, "survey_status", surveyStatus, data::Str2DocumentStatus);
	const bool userArchived(rec["user_archived"].as<bool>());
	const bool selfieImageRequired(rec["selfie_image_required"].as<bool>());
	const bool videoInterviewRequired(rec["video_interview_required"].as<bool>());
	const bool hasPendingPoi(rec["has_pending_poi"].as<bool>());
	const bool hasPendingSelfieImage(rec["has_pending_selfie_image"].as<bool>());
	const bool hasPendingVideoInterview(rec["has_pending_video_interview"].as<bool>());
	const bool hasApprovedSelfieImage(rec["has_approved_selfie_image"].as<bool>());
	const bool hasApprovedVideoInterview(rec["has_approved_video_interview"].as<bool>());
	const bool hasApprovedSumSubPoi(rec["has_approved_sumsub_poi"].as<bool>());
	// Although "SumSub" is used in the next bools, we just check if the video interviews
	// are not system generated. The bools' names were chosen for simplicity ("SumSub"
	// instead of "NotSystemGenerated").
	const bool hasApprovedSumSubVideoInterview(rec["has_approved_sumsub_video_interview"].as<bool>());
	const bool hasExpiredSumSubVideoInterview(rec["has_expired_sumsub_video_interview"].as<bool>());
	const bool hasApprovedPoi(rec["has_approved_poi"].as<bool>());
	const bool hasPendingCC(rec["has_pending_cc"].as<bool>());
	const bool needsCc(rec["needs_cc"].as<bool>());
	const bool isLastC6CheckValid(rec["is_last_c6_check_valid"].as<bool>());
	const bool needsAdminReview(rec["needs_admin_review"].as<bool>());

	if (userArchived)
	{
		userStatusFlags.insert(data::UserStatusFlag::USER_SUSPENDED);
		return;
	}

	const auto getSelfieFlag = [&hasPendingSelfieImage]() {
		return hasPendingSelfieImage ? data::UserStatusFlag::PENDING_SELFIE_IMAGE
									 : data::UserStatusFlag::NEED_SELFIE_IMAGE;
	};

	const auto getDocFlag = [&hasPendingPoi]() {
		return hasPendingPoi ? data::UserStatusFlag::PENDING_POI : data::UserStatusFlag::NEED_POI;
	};
	const auto getInterviewFlag = [&hasPendingVideoInterview]() {
		return hasPendingVideoInterview ? data::UserStatusFlag::PENDING_VIDEO_INTERVIEW
										: data::UserStatusFlag::NEED_VIDEO_INTERVIEW;
	};

	// Please do not change the order of the flags inserted or do it carefully since the selfie,
	// video interview and personal document flags depend on each other. E.g. even if the selfie is
	// needed but there is the approved SumSub video interview, we can remove need selfie flag.

	if (!hasApprovedPoi)
	{
		userStatusFlags.insert(getDocFlag());
	}

	if (selfieImageRequired && !hasApprovedSelfieImage)
	{
		if (hasPendingVideoInterview)
		{
			// Could happen when nationality was changed and previously the video interview
			// was required, so let's finish it
			userStatusFlags.insert(data::UserStatusFlag::PENDING_VIDEO_INTERVIEW);
		}
		else
		{
			userStatusFlags.insert(getSelfieFlag());
		}
	}

	if (videoInterviewRequired && !hasApprovedVideoInterview)
	{
		userStatusFlags.insert(getInterviewFlag());
	}

	if (hasApprovedSumSubVideoInterview)
	{
		// No need to do the selfie if the client already has the approved video interview on SumSub
		userStatusFlags.erase(data::UserStatusFlag::NEED_SELFIE_IMAGE);
	}

	if (hasExpiredSumSubVideoInterview
		&& (userStatusFlags.erase(data::UserStatusFlag::NEED_VIDEO_INTERVIEW) > 0)
		&& !hasApprovedSelfieImage)
	{
		// If the client has the expired video interview on SumSub, the selfie will be sufficient
		// instead of the new video interview
		userStatusFlags.insert(getSelfieFlag());
	}

	if (!hasApprovedSumSubPoi
		&& (userStatusFlags.erase(data::UserStatusFlag::NEED_SELFIE_IMAGE) > 0))
	{
		// The user needs to upload the personal document on SumSub before the selfie,
		// so it can be compared with the document later, for example.
		userStatusFlags.insert(getDocFlag());
	}

	if (needsCc)
	{
		if (hasPendingCC)
		{
			userStatusFlags.insert(data::UserStatusFlag::PENDING_CC);
		}
		else
		{
			userStatusFlags.insert(data::UserStatusFlag::NEED_CC);
		}
	}

	if (!isLastC6CheckValid)
	{
		userStatusFlags.insert(data::UserStatusFlag::NEED_C6_CHECK);
	}

	if (needsAdminReview)
	{
		userStatusFlags.insert(data::UserStatusFlag::NEED_ADMIN_REVIEW);
	}

	if (surveyStatus.has_value())
	{
		switch (surveyStatus.value())
		{
		case data::DocumentStatus::MISSING:
		case data::DocumentStatus::DENIED:
			userStatusFlags.insert(data::UserStatusFlag::NEED_SURVEY);
			break;
		case data::DocumentStatus::PENDING:
			userStatusFlags.insert(data::UserStatusFlag::PENDING_SURVEY);
			break;
		default:
			break;
		}
	}

	if (!userStatusFlags.empty())
	{
		std::vector<std::string> userStatusFlagsStr;
		for (const auto& flag : userStatusFlags)
		{
			userStatusFlagsStr.emplace_back(db::data::UserStatusFlag2Str(flag));
		}

		LDBGWT_ << "Retrieved flags for user " << userId
				<< " are " << utils::JoinStrList(userStatusFlagsStr, ", ");
	}
}

db::data::UserStatus AmountLimitRulesWork::DetermineUserStatus(
	const db::data::Id userId, UserStatusFlagsRefOpt userStatusFlags)
{
	data::UserStatusFlags flags;
	DetermineUserFlags(userId, flags);

	if (userStatusFlags.has_value())
	{
		userStatusFlags.value().get() = flags;
	}

	if (flags.find(data::UserStatusFlag::USER_SUSPENDED) != flags.end())
	{
		return data::UserStatus::ARCHIVED;
	}

	if (flags.find(data::UserStatusFlag::NEED_ADMIN_REVIEW) != flags.end())
	{
		return data::UserStatus::PENDING;
	}

	//  Checks if all conditions for the user to be ACTIVE are satisfied.
	if (flags.empty()
		|| ((flags.find(data::UserStatusFlag::NEED_POI) == flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_POI) == flags.end())
			&& (flags.find(data::UserStatusFlag::NEED_SELFIE_IMAGE) == flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_SELFIE_IMAGE) == flags.end())
			&& (flags.find(data::UserStatusFlag::NEED_VIDEO_INTERVIEW) == flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_VIDEO_INTERVIEW) == flags.end())
			&& (flags.find(data::UserStatusFlag::NEED_CC) == flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_CC) == flags.end())
			&& (flags.find(data::UserStatusFlag::NEED_SURVEY) == flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_SURVEY) == flags.end())
			&& (flags.find(data::UserStatusFlag::NEED_C6_CHECK) == flags.end())))
	{
		return data::UserStatus::ACTIVE;
	}

	// Check if there is a document which is needed and not pending
	if (((flags.find(data::UserStatusFlag::NEED_POI) != flags.end())
		 && (flags.find(data::UserStatusFlag::PENDING_POI) == flags.end()))
		|| ((flags.find(data::UserStatusFlag::NEED_SELFIE_IMAGE) != flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_SELFIE_IMAGE) == flags.end()))
		|| ((flags.find(data::UserStatusFlag::NEED_VIDEO_INTERVIEW) != flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_VIDEO_INTERVIEW) == flags.end()))
		|| ((flags.find(data::UserStatusFlag::NEED_CC) != flags.end())
			&& (flags.find(data::UserStatusFlag::PENDING_CC) == flags.end())))
	{
		return data::UserStatus::MISSING_DOCS;
	}

	// Check is there are documents waiting for review.
	if ((flags.find(data::UserStatusFlag::PENDING_POI) != flags.end())
		|| (flags.find(data::UserStatusFlag::PENDING_SELFIE_IMAGE) != flags.end())
		|| (flags.find(data::UserStatusFlag::PENDING_VIDEO_INTERVIEW) != flags.end())
		|| (flags.find(data::UserStatusFlag::PENDING_CC) != flags.end())
		|| (flags.find(data::UserStatusFlag::PENDING_SURVEY) != flags.end()))
	{
		return data::UserStatus::PENDING;
	}

	// Only validates C6 if all other validations passed,
	// otherwise we may not have all needed info to request C6 check.
	if ((flags.size() == 1) && (*flags.begin() == data::UserStatusFlag::NEED_C6_CHECK))
	{
		if (C6Work(*this).GetOrCreateC6Status(userId, false))
		{
			return data::UserStatus::ACTIVE;
		}
		else
		{
			return data::UserStatus::PENDING;
		}
	}

	return data::UserStatus::MISSING_DOCS;
}

AmountLimitRulesResult AmountLimitRulesWork::GetRequiredInfo(
	const db::data::AmountRuleContext context,
	const db::data::Id userId,
	const std::optional<db::data::Id>& newTransactionId,
	const TransactionType transactionType)
{
	static const std::string emptyStr{};

	AmountLimitRulesResult result{ false, false, std::nullopt };

	auto isPOINeeded{ [this, &userId]() {
		return !HasPOIApproved(userId);
	} };

	auto isCCNeeded([](const bool hasTrans, const db::data::DocumentStatus cCardStatus) {
		return hasTrans && (cCardStatus != db::data::DocumentStatus::APPROVED);
	});

	// TODO Given that Zelle has been dismissed, should we validate the transactionType field for such case?

	switch (context)
	{
	case db::data::AmountRuleContext::NO_CONTEXT:
	{
		switch (transactionType)
		{
		case TransactionType::WIRE:
		{
			result.needPOI = isPOINeeded();
			result.needCC = false;
			result.lastCardFourDigits = emptyStr;
			break;
		}
		case TransactionType::ZELLE:
		case TransactionType::CREDIT_CARD:
		{
			const auto lastPreAuthTransactionInfo{ GetLatestPreAuthTransactionInfo(
				userId, transactionType) };
			if (lastPreAuthTransactionInfo.has_value())
			{
				result.needPOI = isPOINeeded();
				if (transactionType == TransactionType::CREDIT_CARD)
				{
					result.needCC = isCCNeeded(true, lastPreAuthTransactionInfo->creditCardStatus_);
					result.lastCardFourDigits = result.needCC
						? lastPreAuthTransactionInfo->creditCardFourDigits_
						: emptyStr;
				}
				else
				{
					result.needCC = false;
					result.lastCardFourDigits = emptyStr;
				}
			}
			else
			{
				result.needPOI = isPOINeeded();
				result.needCC = false;
				result.lastCardFourDigits = emptyStr;
			}
			break;
		}
		}
		break;
	}
	case db::data::AmountRuleContext::ACCEPT_TRANSACTION:
	{
		switch (transactionType)
		{
		case TransactionType::WIRE:
		{
			result.needPOI = isPOINeeded();
			result.needCC = false;
			result.lastCardFourDigits = emptyStr;
			break;
		}
		case TransactionType::ZELLE:
		{
			result.needPOI = isPOINeeded();
			result.needCC = false;
			result.lastCardFourDigits = emptyStr;
			break;
		}
		case TransactionType ::CREDIT_CARD:
		{
			result.needPOI = false;
			result.needCC = false;
			result.lastCardFourDigits = emptyStr;
			break;
		}
		}
	}
	break;

	case db::data::AmountRuleContext::PROCESS_TRANSACTION:
	{
		switch (transactionType)
		{
		case TransactionType::WIRE:
		{
			THROW(utils::ContextError, "Function not applicable in this context");
			break;
		}
		case TransactionType::ZELLE:
		case TransactionType::CREDIT_CARD:
		{
			// check parameters
			if (!newTransactionId.has_value())
			{
				THROW(std::invalid_argument, "Invalid 'newTransactionId' parameter (null)");
			}

			const auto lastPreAuthTransactionInfo{ GetPreAuthTransactionInfo(*newTransactionId) };
			if (!lastPreAuthTransactionInfo.has_value())
			{
				THROW(
					std::runtime_error,
					fmt::format(
						"Could not retrieve transaction info data for id: '{}'",
						newTransactionId.value()));
			}

			result.needPOI = isPOINeeded();
			if (transactionType == TransactionType::CREDIT_CARD)
			{
				result.needCC = isCCNeeded(true, lastPreAuthTransactionInfo->creditCardStatus_);
				result.lastCardFourDigits =
					result.needCC ? lastPreAuthTransactionInfo->creditCardFourDigits_ : emptyStr;
			}
			else
			{
				result.needCC = false;
				result.lastCardFourDigits = emptyStr;
			}
			break;
		}
		}
	}
	break;
	}

	return result;
}

std::double_t AmountLimitRulesWork::GetUserTransactionSumTotal(
	const db::data::Id userId,
	const db::data::Currency currency,
	const std::initializer_list<db::data::TransactionStatus>& statuses,
	const std::double_t amount)
{
	if (statuses.size() < 1u)
	{
		throw std::invalid_argument("Invalid 'statuses' parameter (empty)");
	}

	const auto statusesStr{ [&statuses]() {
		std::stringstream sstr{};
		std::size_t i{ 0u };
		for (const auto& s : statuses)
		{
			sstr << '\'' << db::data::TransactionStatus2Str(s) << '\'';
			if (++i < statuses.size())
			{
				sstr << ',';
			}
		}
		return sstr.str();
	}() };

	// ensure we have rates in the database for all relevant transactions
	for (const auto& row : Exec(fmt::format(
			 R"__(
			SELECT DISTINCT(ti.performed_at::DATE) missing_rate_date
			FROM transaction_info ti
			LEFT JOIN exchange_rate rt_orig ON (rt_orig.day = ti.performed_at::DATE AND rt_orig.currency = ti.currency)
			LEFT JOIN exchange_rate rt_conv ON (rt_conv.day = ti.performed_at::DATE AND rt_conv.currency = 'EUR')
			WHERE ti.user_id = {}
				AND ti.status IN ({})
				AND (rt_orig.rate IS NULL OR rt_conv.rate IS NULL)
		)__",
			 userId,
			 statusesStr)))
	{
		ExchangeRates::EnsureRateAvailability(row["missing_rate_date"].as<std::string>());
	}
	const std::string sql{ fmt::format(
		R"__(
select
	sum(((ti.price / rt_orig.rate) * rt_conv.rate)) total
from
	transaction_info ti
inner join exchange_rate rt_orig on
	ti.performed_at::date = rt_orig.day
	and ti.currency = rt_orig.currency
inner join exchange_rate rt_conv on
	ti.performed_at::date = rt_conv.day
	and rt_conv.currency = '{}'
where
	ti.user_id = {}
	and ti.status in ({})
)__",
		db::data::Currency2Str(currency),
		userId,
		statusesStr) };
	//
	return Exec(sql)[0]["total"].as<std::double_t>(0.0) + amount;
}

std::optional<AmountLimitRulesWork::PreAuthTransactionInfo> AmountLimitRulesWork::
	GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
{
	if (transactionType == TransactionType::WIRE)
	{
		throw std::invalid_argument("Invalid transactionType parameter (cannot be WIRE)");
	}

	const auto paymentSysNames(
		transactionType == TransactionType::CREDIT_CARD
			? fmt::format(
				"'{}','{}'",
				db::data::PaymentSys2Str(db::data::PaymentSys::SECURETRADING),
				db::data::PaymentSys2Str(db::data::PaymentSys::LINK4PAY))
			: fmt::format("'{}'", db::data::PaymentSys2Str(db::data::PaymentSys::DEBITWAY)));

	// Get last transaction which is PREAUTH_FINISHED
	const auto lastTransactionSql{ fmt::format(
		"select id from transaction_info where user_id = {} and "
		"status = '{}' and payment_name in ({}) order by id desc limit 1",
		userId,
		db::data::TransactionStatus2Str(db::data::TransactionStatus::PREAUTH_FINISHED),
		paymentSysNames) };

	const auto resultTransaction{ Exec(lastTransactionSql) };

	if (resultTransaction.empty())
	{
		return std::nullopt;
	}
	return GetPreAuthTransactionInfo(resultTransaction[0]["id"].as<std::uint32_t>());
}

std::optional<AmountLimitRulesWork::PreAuthTransactionInfo> AmountLimitRulesWork::
	GetPreAuthTransactionInfo(const db::data::Id transactionId)
{
	// ensure we have rates in the database for all relevant transactions
	for (const auto& row : Exec(fmt::format(
			 R"__(
select
    distinct(performed_at::date) performed_at
from
    transaction_info ti
where
    id = {} and status = '{}' and performed_at::date not in (select distinct(day) from exchange_rate)
)__",
			 transactionId,
			 db::data::TransactionStatus2Str(db::data::TransactionStatus::PREAUTH_FINISHED))))
	{
		ExchangeRates::EnsureRateAvailability(row["performed_at"].as<std::string>());
	}

	// Get last transaction which is PREAUTH_FINISHED
	const auto lastTransactionSql{ fmt::format(
		R"__(
select
	ti.id,
    ti.performed_at,
	ti.credit_card,
	cci.status,
	ti.price,
	((ti.price / rt_orig.rate) * rt_conv.rate) price_conv,
	ti.currency,
	ti.payment_name
from
	transaction_info ti
inner join exchange_rate rt_orig on
	ti.performed_at::date = rt_orig.day
	and ti.currency = rt_orig.currency
inner join exchange_rate rt_conv on
	ti.performed_at::date = rt_conv.day
	and rt_conv.currency =
	case
		when ti.payment_name = 'D' then 'USD'::currency_type
		else 'EUR'::currency_type
end
left outer join credit_card_info cci on
	ti.user_id = cci.user_id
	and ti.credit_card = cci.last_digits
	and cci.status = '{}'
where
	ti.id = {}
	and ti.status = '{}'
order by
	cci.id desc
limit 1;
)__",
		DocumentStatus2Str(db::data::DocumentStatus::APPROVED),
		transactionId,
		db::data::TransactionStatus2Str(db::data::TransactionStatus::PREAUTH_FINISHED)) };
	const auto resultTransaction{ Exec(lastTransactionSql) };

	if (resultTransaction.empty())
	{
		return std::nullopt;
	}

	const auto paymentSys{ db::data::Str2PaymentSys(
		resultTransaction[0]["payment_name"].as<std::string>()) };
	return PreAuthTransactionInfo{
		resultTransaction[0]["id"].as<db::data::Id>(),
		resultTransaction[0]["credit_card"].as<std::string>(""),
		db::data::Str2DocumentStatus(resultTransaction[0]["status"].as<std::string>(
			db::data::DocumentStatus2Str(db::data::DocumentStatus::MISSING))),
		resultTransaction[0]["price"].as<std::double_t>(),
		db::data::Str2Currency(resultTransaction[0]["currency"].as<std::string>()),
		resultTransaction[0]["price_conv"].as<std::double_t>(),
		paymentSys == db::data::PaymentSys::DEBITWAY ? db::data::Currency::USD
													 : db::data::Currency::EUR,
		resultTransaction[0]["performed_at"].as<std::string>(),
		paymentSys
	};
}

bool AmountLimitRulesWork::HasPOIApproved(const db::data::Id userId)
{
	return Exec(fmt::format(
			   "select count(*) cnt from document_info where user_id = {} "
			   "and type in ('{}', '{}') and status = '{}'",
			   userId,
			   DocumentType2Str(db::data::DocumentType::PERSONAL),
			   DocumentType2Str(db::data::DocumentType::PERSONAL_ADDITIONAL),
			   DocumentStatus2Str(db::data::DocumentStatus::APPROVED)))[0]["cnt"]
			   .as<size_t>(0)
		> 0;
}

db::StmtBind AmountLimitRulesWork::DetermineFlagsQuery(const db::data::Id userId) const
{
	static const std::uint16_t c6CheckExpirationDays(
		utils::AppConfig::Singleton().c6_.c6CheckExpirationDays_);
	static const std::string statusQuery(R"__(
		SELECT
			usi.status AS survey_status,
			(uai.status = 'ARCHIVED') AS user_archived,
			cpr.selfie_image_required,
			cpr.video_interview_required,
			EXISTS (
				SELECT 1 FROM document_info where user_id = uai.id AND type = 'PERSONAL' AND status = 'PENDING'
			) AS has_pending_poi,
			EXISTS (
				SELECT 1 FROM document_info where user_id = uai.id AND type = 'SELFIE_IMAGE' AND status = 'PENDING'
			) AS has_pending_selfie_image,
			EXISTS (
				SELECT 1 FROM document_info where user_id = uai.id AND type = 'VIDEO_INTERVIEW' AND status = 'PENDING'
			) AS has_pending_video_interview,
			EXISTS (
				SELECT 1 FROM document_info WHERE user_id = uai.id AND type = 'PERSONAL' AND status = 'APPROVED'
			) AS has_approved_poi,
			EXISTS (
				SELECT 1 FROM document_info WHERE user_id = uai.id AND type = 'SELFIE_IMAGE' AND status = 'APPROVED'
			) AS has_approved_selfie_image,
			EXISTS (
				SELECT 1 FROM document_info WHERE user_id = uai.id AND type = 'VIDEO_INTERVIEW' AND status = 'APPROVED'
			) AS has_approved_video_interview,
			EXISTS (
				SELECT 1 FROM document_info WHERE user_id = uai.id AND type = 'VIDEO_INTERVIEW' AND status = 'APPROVED' AND system_generated = FALSE
			) AS has_approved_sumsub_video_interview,
			EXISTS (
				SELECT 1
				FROM document_info di
				INNER JOIN user_sumsub_flow usf ON (di.user_id = usf.user_id AND di.id = usf.document_id)
				WHERE di.user_id = uai.id AND di."type" = 'PERSONAL' AND di.status = 'APPROVED'
			) AS has_approved_sumsub_poi,
			EXISTS (
				SELECT 1 FROM document_info WHERE user_id = uai.id AND type = 'VIDEO_INTERVIEW' AND status = 'EXPIRED' AND system_generated = FALSE
			) AS has_expired_sumsub_video_interview,
			EXISTS (
				SELECT 1 FROM credit_card_info cci WHERE cci.user_id = uai.id AND status = 'PENDING'
			) AS has_pending_cc,
			EXISTS (
				SELECT 1
				FROM transaction_info ti
				WHERE ti.user_id = uai.id
					AND ti.status = 'PREAUTH_FINISHED'
					AND ti.credit_card NOT IN
					(
						SELECT cci.last_digits
						FROM credit_card_info cci
						WHERE cci.user_id = uai.id
							AND cci.status = 'APPROVED'
					)
			) AS needs_cc,
			COALESCE((
				SELECT c6_on_board
				FROM user_c6_info
				WHERE user_id = uai.id
					AND ((c6_super_admin_id IS NOT NULL)
						OR (c6_last_check IS NOT NULL AND (NOW()::DATE - c6_last_check::DATE) < [:c6_check_expire_days]))
			), FALSE) AS is_last_c6_check_valid,
			EXISTS (
				SELECT 1 FROM user_needs_review unr WHERE unr.user_id = uai.id AND admin_id IS NULL
			) As needs_admin_review
		FROM user_account_info uai
		INNER JOIN user_address_info uadd ON (uadd.id = uai.id)
		INNER JOIN user_personal_info upi ON (upi.id = uai.id)
		INNER JOIN countries c2 ON (c2.name = COALESCE(upi.nationality, uadd.country))
		INNER JOIN country_poi_requirements cpr ON (cpr.country_id = c2.id)
		LEFT JOIN user_survey_info usi ON (usi.user_id = uai.id)
		WHERE uai.id = [:user_id];
	)__");

	StmtBind stmt;
	stmt.SetStmt(statusQuery);
	stmt.BindValue(":user_id", userId);
	stmt.BindValue(":c6_check_expire_days", c6CheckExpirationDays);

	return stmt;
}

}	 // namespace db
