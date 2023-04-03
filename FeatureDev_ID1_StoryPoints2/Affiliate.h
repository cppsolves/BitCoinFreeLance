#pragma once

namespace db
{

class AffiliateWork;

namespace data
{

class Affiliate;
class AffiliatePayment;
class AffiliateFilter;
class Pagination;

}	 // namespace data
}	 // namespace db

namespace insta
{

class Affiliate
{
	//
	// Construction and destruction.
	//
public:
	Affiliate();
	~Affiliate();

	//
	// Public interface.
	//

public:
	//! Returns affiliates account info.
	nl::json Get(const db::data::Id id) const;
	//! Returns affiliates stats.
	nl::json GetStats(const db::data::Id id) const;
	//! Returns list of affiliates.
	nl::json List(const db::data::AffiliateFilter& filter, const db::data::Pagination& pag) const;
	//! Streams a list of affiliates in CSV format on \param outputStream
	void ExportToCsv(const db::data::AffiliateFilter& filter, std::ostream& outputStream) const;
	//! Sends the money earned by the affiliate to the wallet (BTC).
	void SendEarnings(db::data::AffiliatePayment& data) const;
	//! Sets affiliate account info and returns updated object.
	nl::json Set(const db::data::Affiliate& info) const;
	//! Updates the affiliates commissions for a transaction.
	static void UpdateCommissions(
		db::Work& work, const db::data::TransactionInfo& transaction, bool deduct = false) noexcept;

	//
	// Private types.
	//
private:
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	struct Stats
	{
		//
		// Construction and destruction.
		//
	public:
		Stats(nl::json&& data);

		//
		// Public interface.
		//
	public:
		//! Returns data.
		nl::json Get() const;
		//! Returns true if stats is actual.
		bool IsActual() const;
		//! Returns time period since last update.
		TimePoint LastUpdateAt() const;
		//! Updates data and updatedAt.
		void Update(nl::json&& data);

		//
		// Private data members.
		//
	private:
		//! Stats data.
		nl::json data_;
		//! Last update timestamp.
		TimePoint updatedAt_;
	};

	using StatsUnPtr = std::unique_ptr<Stats>;
	using StatsMap = std::map<db::data::Id, StatsUnPtr>;

	//
	// Private methods.
	//
private:
	//! Collects affiliate stats.
	nl::json CollectStats(const db::data::Affiliate& affiliate) const;
	//! Clears stats cache.
	void ClearStatsCache();
	// Returns list of clients for affiliate.
	db::data::Clients GetClients(const db::data::Id id) const;
	// Returns affiliate link for user.
	std::string GetLink(const db::data::Id id) const;
	//! Returns affiliates clicks.
	nl::json GetClicksCount(const db::data::Id id, const bool needTotal = false) const;
	//! Returns affiliates clients.
	nl::json GetClientsCount(const db::data::Id id, const bool needTotal = false) const;
	//! Returns summary commission of transactions.
	nl::json GetCommission(const db::data::Id id, const bool needTotal = false) const;
	//! Returns affiliates sales volume.
	nl::json GetSalesAmount(const db::data::Id id, const bool needTotal = false) const;
	//! Sends BTC to a wallet.
	void SendBtc(const double btcAmount, const std::string& wallet) const;
	//! Updates affiliate balance
	static db::data::DoubleOpt UpdateAffiliateBalance(
		db::Work& work,
		const db::data::Affiliate& affiliate,
		const db::data::Currency& currency,
		double amount,
		bool deduct /*=false*/);

	//
	// Private data members.
	//
private:
	//! Connection to db.
	db::ConnectionUnit connection_;
	//! Db worker.
	std::unique_ptr<db::AffiliateWork> work_;
	//! Mutex which controls access to worker.
	mutable std::mutex mtx_;
	//! Mutex which controls access to cache.
	static std::mutex cacheMtx_;
	//! Stat cached data.
	static std::map<db::data::Id, StatsUnPtr> statsCache_;
	//! Timer for clearing cache.
	static std::unique_ptr<utils::Timer> statsCacheTimer_;
	//! Indicates that timer already initiated.
	static std::once_flag statsTimerInitFlag_;
};

}	 // namespace insta
