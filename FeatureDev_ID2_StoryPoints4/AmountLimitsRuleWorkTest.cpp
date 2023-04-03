#include "pch.h"

// Utils module headers.
#include <utils/Chrono.h>

// DB module headers.
#include <db/Types.h>
#include <db/Connection.h>
#include <db/Work.h>
#include <db/AmountLimitRulesWork.h>

namespace test
{

static const db::data::Id FakeId{ 100 };

// Not the best way for mocking but gmock or fakeit are not included
class AmountLimitRuleWorkLess2000 : public db::AmountLimitRulesWork
{
public:
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000(db::Work w)
		: db::AmountLimitRulesWork{ std::move(w) }
	{
	}
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000(db::HandleShPtr handle)
		: db::AmountLimitRulesWork{ std::move(handle) }
	{
	}

protected:
	virtual std::double_t GetUserTransactionSumTotal(
		const db::data::Id userId,
		const db::data::Currency currency,
		const std::initializer_list<db::data::TransactionStatus>& statuses,
		const std::double_t amount = 0.0)
	{
		return 1000;
	}

	virtual std::optional<PreAuthTransactionInfo> GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::MISSING,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual std::optional<PreAuthTransactionInfo> GetPreAuthTransactionInfo(
		const db::data::Id transactionId)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::MISSING,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual bool HasPOIApproved(const db::data::Id userId)
	{
		return false;
	}

	virtual bool HasPOAApproved(const db::data::Id userId)
	{
		return false;
	}
};

class AmountLimitRuleWorkLess2000ApprovedCard : public AmountLimitRuleWorkLess2000
{
public:
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000ApprovedCard(db::Work w)
		: AmountLimitRuleWorkLess2000(w)
	{
	}
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000ApprovedCard(db::HandleShPtr handle)
		: AmountLimitRuleWorkLess2000(handle)
	{
	}

protected:
	virtual std::optional<PreAuthTransactionInfo> GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::APPROVED,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual std::optional<PreAuthTransactionInfo> GetPreAuthTransactionInfo(
		const db::data::Id transactionId)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::APPROVED,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual bool HasPOIApproved(const db::data::Id userId)
	{
		return false;
	}
};

class AmountLimitRuleWorkLess2000NoTransactionsApprovedCard : public AmountLimitRuleWorkLess2000
{
public:
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000NoTransactionsApprovedCard(db::Work w)
		: AmountLimitRuleWorkLess2000(w)
	{
	}
	//! Constructor.
	explicit AmountLimitRuleWorkLess2000NoTransactionsApprovedCard(db::HandleShPtr handle)
		: AmountLimitRuleWorkLess2000(handle)
	{
	}

protected:
	virtual std::double_t GetTotalUserTransactionAmountInEUR(
		const db::data::Id, const std::initializer_list<db::data::TransactionStatus>& statuses)
	{
		return 0;
	}

	virtual std::optional<PreAuthTransactionInfo> GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1",
															 db::data::DocumentStatus::DENIED,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual std::optional<PreAuthTransactionInfo> GetPreAuthTransactionInfo(
		const db::data::Id transactionId)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::DENIED,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual bool HasPOIApproved(const db::data::Id userId)
	{
		return false;
	}
};

class AmountLimitRuleWorkMore2000 : public AmountLimitRuleWorkLess2000
{
public:
	//! Constructor.
	explicit AmountLimitRuleWorkMore2000(db::Work w)
		: AmountLimitRuleWorkLess2000(w)
	{
	}
	//! Constructor.
	explicit AmountLimitRuleWorkMore2000(db::HandleShPtr handle)
		: AmountLimitRuleWorkLess2000(handle)
	{
	}

protected:
	virtual std::double_t GetTotalUserTransactionAmountInEUR(
		const db::data::Id, const std::initializer_list<db::data::TransactionStatus>& statuses)
	{
		return 2001;
	}

	virtual std::optional<PreAuthTransactionInfo> GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::MISSING,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual std::optional<PreAuthTransactionInfo> GetPreAuthTransactionInfo(
		const db::data::Id transactionId)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::MISSING,
															 2.0,
															 db::data::Currency::EUR,
															 2.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}
};

class AmountLimitRuleWorkMore2000ApprovedCard : public AmountLimitRuleWorkMore2000
{
public:
	//! Constructor.
	explicit AmountLimitRuleWorkMore2000ApprovedCard(db::Work w)
		: AmountLimitRuleWorkMore2000(w)
	{
	}
	//! Constructor.
	explicit AmountLimitRuleWorkMore2000ApprovedCard(db::HandleShPtr handle)
		: AmountLimitRuleWorkMore2000(handle)
	{
	}

protected:
	virtual std::optional<PreAuthTransactionInfo> GetLatestPreAuthTransactionInfo(
		const db::data::Id userId, const TransactionType transactionType)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::APPROVED,
															 2001.0,
															 db::data::Currency::EUR,
															 2001.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual std::optional<PreAuthTransactionInfo> GetPreAuthTransactionInfo(
		const db::data::Id transactionId)
	{
		return AmountLimitRulesWork::PreAuthTransactionInfo{ 1,
															 "1234",
															 db::data::DocumentStatus::APPROVED,
															 2001.0,
															 db::data::Currency::EUR,
															 2001.0,
															 db::data::Currency::EUR,
															 utils::GetCurrentDate(),
															 db::data::PaymentSys::SECURETRADING };
	}

	virtual bool HasPOIApproved(const db::data::Id userId)
	{
		return false;
	}

	virtual bool HasPOAApproved(const db::data::Id userId)
	{
		return false;
	}
};

TEST(AmountLimitsRuleWorkTest, PreauthWithApprovedCardLessThan2k)
{

	//	SECTION("No Context, has a preauth transaction with APPROVED card but less than 2K")
	{
		db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
		AmountLimitRuleWorkLess2000ApprovedCard amountRuleWork{ conn.Get() };

		auto result{ amountRuleWork.GetRequiredInfo(
			db::data::AmountRuleContext::NO_CONTEXT,
			FakeId,
			std::nullopt,
			db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

		EXPECT_TRUE(result.needPOI);
		EXPECT_FALSE(result.needCC);
		EXPECT_EQ(result.lastCardFourDigits, "");
	}
}

TEST(AmountLimitsRuleWorkTest, PreauthWithUnapprovedCardLessThan2k)
{
	// SECTION("No Context, has preauth transaction with UNAPPROVED card but less than 2k")
	{
		db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
		AmountLimitRuleWorkLess2000NoTransactionsApprovedCard amountRuleWork{ conn.Get() };

		auto result{ amountRuleWork.GetRequiredInfo(
			db::data::AmountRuleContext::NO_CONTEXT,
			FakeId,
			std::nullopt,
			db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

		EXPECT_TRUE(result.needPOI);
		EXPECT_TRUE(result.needCC);
		EXPECT_EQ(result.lastCardFourDigits, "1");
	}
}

TEST(AmountLimitsRuleWorkTest, PreauthWithApprovedCardMoreThan2k)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkMore2000ApprovedCard amountRuleWork{ conn.Get() };

	auto result{ amountRuleWork.GetRequiredInfo(
		db::data::AmountRuleContext::NO_CONTEXT,
		FakeId,
		std::nullopt,
		db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

	EXPECT_TRUE(result.needPOI);
	EXPECT_FALSE(result.needCC);
}

TEST(AmountLimitsRuleWorkTest, AcceptTrxContextPreauthWithApprovedCardMoreThan2k)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkMore2000ApprovedCard amountRuleWork{ conn.Get() };

	auto result{ amountRuleWork.GetRequiredInfo(
		db::data::AmountRuleContext::ACCEPT_TRANSACTION,
		FakeId,
		std::nullopt,
		db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

	EXPECT_FALSE(result.needPOI);
	EXPECT_FALSE(result.needCC);
}

TEST(AmountLimitsRuleWorkTest, AcceptTrxContextWireTransfer)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkMore2000ApprovedCard amountRuleWork{ conn.Get() };

	auto result{ amountRuleWork.GetRequiredInfo(
		db::data::AmountRuleContext::ACCEPT_TRANSACTION,
		FakeId,
		std::nullopt,
		db::AmountLimitRulesWork::TransactionType::WIRE) };

	EXPECT_TRUE(result.needPOI);
	EXPECT_FALSE(result.needCC);
}

TEST(AmountLimitsRuleWorkTest, ProcessTrxPreauthWithApprovedCardMoreThan2k)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkMore2000ApprovedCard amountRuleWork{ conn.Get() };

	auto result{ amountRuleWork.GetRequiredInfo(
		db::data::AmountRuleContext::PROCESS_TRANSACTION,
		FakeId,
		1,
		db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

	EXPECT_TRUE(result.needPOI);
	EXPECT_FALSE(result.needCC);
	EXPECT_EQ(result.lastCardFourDigits, "");
}

TEST(AmountLimitsRuleWorkTest, ProcessTrxPreauthWithApprovedCardLessThan2k)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkLess2000ApprovedCard amountRuleWork{ conn.Get() };

	auto result{ amountRuleWork.GetRequiredInfo(
		db::data::AmountRuleContext::PROCESS_TRANSACTION,
		FakeId,
		1,
		db::AmountLimitRulesWork::TransactionType::CREDIT_CARD) };

	EXPECT_TRUE(result.needPOI);
	EXPECT_FALSE(result.needCC);
	EXPECT_EQ(result.lastCardFourDigits, "");
}

TEST(AmountLimitsRuleWorkTest, ProcessTrxContextWireTransfer)
{
	db::ConnectionUnit conn{ db::ConnectionPool::GetInstance().Acquire() };
	AmountLimitRuleWorkMore2000ApprovedCard amountRuleWork{ conn.Get() };

	EXPECT_THROW(
		amountRuleWork.GetRequiredInfo(
			db::data::AmountRuleContext::PROCESS_TRANSACTION,
			FakeId,
			std::nullopt,
			db::AmountLimitRulesWork::TransactionType::WIRE),
		utils::ContextError);
}

class AmountLimitRuleWorkMockedSql : public db::AmountLimitRulesWork
{
public:
	AmountLimitRuleWorkMockedSql()
		: AmountLimitRulesWork(db::ConnectionPool::GetInstance().Acquire())
	{}

	void UpdateQuery(const std::string& sql)
	{
		sql_ = sql;
	}

private:
	db::StmtBind DetermineFlagsQuery(const db::data::Id) const override
	{
		return db::StmtBind{ "", sql_ };
	}

	std::string sql_;
};

class FlagsSetter
{
public:
	FlagsSetter()
	{
		Reset();
	}

	FlagsSetter& Reset()
	{
		surveyStatus_ = db::data::DocumentStatus::APPROVED;
		userArchived_ = false;
		selfieRequired_ = false;
		videoRequired_ = false;
		hasPendingDoc_ = false;
		hasPendingSelfie_ = false;
		hasPendingVideo_ = false;
		hasApprovedDoc_ = true;
		hasApprovedSelfie_ = false;
		hasApprovedVideo_ = false;
		hasApprovedSsVideo_ = false;
		hasApprovedSsDoc_ = false;
		hasExpiredSsVideo_ = false;
		hasPendingCc_ = false;
		needsCc_ = false;
		isLastC6CheckValid_ = true;
		needsAdminReview_ = false;

		return std::ref(*this);
	}

	FlagsSetter& SetSurveyStatus(const db::data::DocumentStatus status)
	{
		surveyStatus_ = status;

		return std::ref(*this);
	}

	FlagsSetter& SetUserArchived(const bool isUserArchived)
	{
		userArchived_ = isUserArchived;

		return std::ref(*this);
	}

	FlagsSetter& SetSelfieRequired(const bool isSelfieRequired)
	{
		selfieRequired_ = isSelfieRequired;

		return std::ref(*this);
	}

	FlagsSetter& SetVideoRequired(const bool isVideoRequired)
	{
		videoRequired_ = isVideoRequired;

		return std::ref(*this);
	}

	FlagsSetter& SetPendingDoc(const bool isDocPending)
	{
		hasPendingDoc_ = isDocPending;

		return std::ref(*this);
	}

	FlagsSetter& SetPendingSelfie(const bool isSelfiePending)
	{
		hasPendingSelfie_ = isSelfiePending;

		return std::ref(*this);
	}

	FlagsSetter& SetPendingVideo(const bool isVideoPending)
	{
		hasPendingVideo_ = isVideoPending;

		return std::ref(*this);
	}

	FlagsSetter& SetDocApproved(const bool isDocApproved)
	{
		hasApprovedDoc_ = isDocApproved;

		return std::ref(*this);
	}

	FlagsSetter& SetSelfieApproved(const bool isSelfieApproved)
	{
		hasApprovedSelfie_ = isSelfieApproved;

		return std::ref(*this);
	}

	FlagsSetter& SetVideoApproved(const bool isVideoApproved)
	{
		hasApprovedVideo_ = isVideoApproved;

		return std::ref(*this);
	}

	FlagsSetter& SetSsVideoApproved(const bool isSsVideoApproved)
	{
		hasApprovedSsVideo_ = isSsVideoApproved;

		return std::ref(*this);
	}

	FlagsSetter& SetSsDocApproved(const bool isSsDocApproved)
	{
		hasApprovedSsDoc_ = isSsDocApproved;

		return std::ref(*this);
	}

	FlagsSetter& SetSsVideoExpired(const bool isSsVideoExpired)
	{
		hasExpiredSsVideo_ = isSsVideoExpired;

		return std::ref(*this);
	}

	FlagsSetter& SetPendingCc(const bool isCcPending)
	{
		hasPendingCc_ = isCcPending;

		return std::ref(*this);
	}

	FlagsSetter& SetNeedsCc(const bool isCcNeeded)
	{
		needsCc_ = isCcNeeded;

		return std::ref(*this);
	}

	FlagsSetter& SetLastC6Check(const bool isCheckValid)
	{
		isLastC6CheckValid_ = isCheckValid;

		return std::ref(*this);
	}

	FlagsSetter& SetNeedsAdminReview(const bool isReviewNeeded)
	{
		needsAdminReview_ = isReviewNeeded;

		return std::ref(*this);
	}

	operator std::string() const
	{
		return fmt::format(
			R"__(
			SELECT
				'{}' AS survey_status,
				{} AS user_archived,
				{} AS selfie_image_required,
				{} AS video_interview_required,
				{} AS has_pending_poi,
				{} AS has_pending_selfie_image,
				{} AS has_pending_video_interview,
				{} AS has_approved_poi,
				{} AS has_approved_selfie_image,
				{} AS has_approved_video_interview,
				{} AS has_approved_sumsub_video_interview,
				{} AS has_approved_sumsub_poi,
				{} AS has_expired_sumsub_video_interview,
				{} AS has_pending_cc,
				{} AS needs_cc,
				{} AS is_last_c6_check_valid,
				{} AS needs_admin_review
			;)__",
			db::data::DocumentStatus2Str(surveyStatus_),
			userArchived_,
			selfieRequired_,
			videoRequired_,
			hasPendingDoc_,
			hasPendingSelfie_,
			hasPendingVideo_,
			hasApprovedDoc_,
			hasApprovedSelfie_,
			hasApprovedVideo_,
			hasApprovedSsVideo_,
			hasApprovedSsDoc_,
			hasExpiredSsVideo_,
			hasPendingCc_,
			needsCc_,
			isLastC6CheckValid_,
			needsAdminReview_);
	}

private:
	db::data::DocumentStatus surveyStatus_;
	bool userArchived_;
	bool selfieRequired_;
	bool videoRequired_;
	bool hasPendingDoc_;
	bool hasPendingSelfie_;
	bool hasPendingVideo_;
	bool hasApprovedDoc_;
	bool hasApprovedSelfie_;
	bool hasApprovedVideo_;
	bool hasApprovedSsVideo_;
	bool hasApprovedSsDoc_;
	bool hasExpiredSsVideo_;
	bool hasPendingCc_;
	bool needsCc_;
	bool isLastC6CheckValid_;
	bool needsAdminReview_;

};

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_NoFlagsNeeded)
{
	AmountLimitRuleWorkMockedSql alrw;

	db::data::UserStatusFlags flags;
	db::data::Id userId;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter);

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_TRUE(flags.empty());

	// has_approved_{poi|selfie_image|video_interview|sumsub_video_interview|sumsub_poi},
	// has_pending_{poi|selfie_image|video_interview|cc} and has_expired_sumsub_video_interview
	// can have different values, but it should not affect the flags retrieved if the
	// selfie/video/poi are not required, e.g. they can all be set to FALSE or TRUE in such case.
	flagsSetter.Reset()
		.SetSurveyStatus(db::data::DocumentStatus::EXPIRED)
		.SetPendingDoc(true)
		.SetPendingSelfie(true)
		.SetPendingVideo(true)
		.SetSelfieApproved(true)
		.SetVideoApproved(true)
		.SetSsVideoApproved(true)
		.SetSsDocApproved(true)
		.SetSsVideoExpired(true)
		.SetPendingCc(true);

	alrw.UpdateQuery(flagsSetter);

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_TRUE(flags.empty());
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_Survey)
{
	AmountLimitRuleWorkMockedSql alrw;
	db::data::UserStatusFlags flags;
	db::data::Id userId;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetSurveyStatus(db::data::DocumentStatus::MISSING));

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_SURVEY), 1);

	alrw.UpdateQuery(flagsSetter.SetSurveyStatus(db::data::DocumentStatus::DENIED));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_SURVEY), 1);

	alrw.UpdateQuery(flagsSetter.SetSurveyStatus(db::data::DocumentStatus::PENDING));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_SURVEY), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_UserArchived)
{
	AmountLimitRuleWorkMockedSql alrw;
	db::data::UserStatusFlags flags;
	db::data::Id userId;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetUserArchived(true));

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::USER_SUSPENDED), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_NeedAdminReview)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetNeedsAdminReview(true));

	db::data::UserStatusFlags flags;
	db::data::Id userId;

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_ADMIN_REVIEW), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_C6Check)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetLastC6Check(false));

	db::data::UserStatusFlags flags;
	db::data::Id userId{};

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_C6_CHECK), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_CreditCard)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetNeedsCc(true));

	db::data::UserStatusFlags flags;
	db::data::Id userId;

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_CC), 1);

	alrw.UpdateQuery(flagsSetter.SetPendingCc(true));

	flags.clear();

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_CC), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_Poi)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	alrw.UpdateQuery(flagsSetter.SetDocApproved(false));

	db::data::UserStatusFlags flags;
	db::data::Id userId;

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_POI), 1);

	alrw.UpdateQuery(flagsSetter.SetPendingDoc(true));

	flags.clear();

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_POI), 1);
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_Selfie)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	// Selfie required; there is no SumSub approved document, so need POI flag will be retrieved
	alrw.UpdateQuery(flagsSetter.SetSelfieRequired(true));

	db::data::UserStatusFlags flags;
	db::data::Id userId;

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_POI), 1);

	// Selfie required, SumSub POI is approved; need selfie flag will be retrieved
	alrw.UpdateQuery(flagsSetter.SetSsDocApproved(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_SELFIE_IMAGE), 1);

	// Selfie required, POI is pending; has pending poi flag will be retrieved
	alrw.UpdateQuery(flagsSetter.Reset().SetSelfieRequired(true).SetPendingDoc(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_POI), 1);

	// Selfie required, video interview is pending; has pending video interview flag will be retrieved
	alrw.UpdateQuery(flagsSetter.Reset().SetSelfieRequired(true).SetPendingVideo(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_VIDEO_INTERVIEW), 1);

	// Selfie required, selfie is pending; has pending selfie flag will be retrieved
	alrw.UpdateQuery(flagsSetter.Reset().SetSelfieRequired(true).SetPendingSelfie(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_SELFIE_IMAGE), 1);

	// Selfie required, and it is already approved; no flags will be retrieved
	alrw.UpdateQuery(flagsSetter.Reset().SetSelfieRequired(true).SetSelfieApproved(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_TRUE(flags.empty());

	// Selfie required, video interview on SumSub is already approved; no flags will be retrieved
	alrw.UpdateQuery(flagsSetter.Reset().SetSelfieRequired(true).SetSsVideoApproved(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_TRUE(flags.empty());
}

TEST(AmountLimitsRuleWorkTest, DetermineUserFlags_VideoInterview)
{
	AmountLimitRuleWorkMockedSql alrw;
	FlagsSetter flagsSetter;

	// Video interview is required
	alrw.UpdateQuery(flagsSetter.SetVideoRequired(true));

	db::data::UserStatusFlags flags;
	db::data::Id userId;

	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_VIDEO_INTERVIEW), 1);

	// Video interview required, and it is already pending
	alrw.UpdateQuery(flagsSetter.SetPendingVideo(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_VIDEO_INTERVIEW), 1);

	// Video interview is required, there is expired SumSub interview and approved selfie
	alrw.UpdateQuery(
		flagsSetter.Reset().SetVideoRequired(true).SetSsVideoExpired(true).SetSelfieApproved(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_TRUE(flags.empty());

	// Video interview is required, there is expired SumSub interview and
	// no approved selfie/SumSub personal document
	alrw.UpdateQuery(flagsSetter.Reset().SetVideoRequired(true).SetSsVideoExpired(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_POI), 1);

	// Video interview is required, there is expired SumSub interview
	// and approved SumSub personal document
	alrw.UpdateQuery(
		flagsSetter.Reset().SetVideoRequired(true).SetSsVideoExpired(true).SetSsDocApproved(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::NEED_SELFIE_IMAGE), 1);

	// Video interview is required; there is expired SumSub video interview and
	// pending personal document
	alrw.UpdateQuery(
		flagsSetter.Reset().SetVideoRequired(true).SetSsVideoExpired(true).SetPendingDoc(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_POI), 1);

	// Video interview is required; there is expired SumSub video interview and
	// pending selfie
	alrw.UpdateQuery(
		flagsSetter.Reset().SetVideoRequired(true).SetSsVideoExpired(true).SetPendingSelfie(true));

	flags.clear();
	alrw.DetermineUserFlags(userId, flags);

	EXPECT_EQ(flags.size(), 1);
	EXPECT_EQ(flags.count(db::data::UserStatusFlag::PENDING_SELFIE_IMAGE), 1);
}

}	 // namespace test
