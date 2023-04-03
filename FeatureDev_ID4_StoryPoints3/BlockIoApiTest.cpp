#include "pch.h"

#include <insta/data/BtcSendingResult.h>
#include <insta/services/btc/BtcSendingApi.h>
#include <insta/services/btc/BlockIoApi.h>

#include <cstdlib>

namespace
{

class BlockIoApiTest : public ::testing::Test
{
public:
	void SetUp() override
	{
		const char* ci = getenv("CI");

		// Reference: https://docs.gitlab.com/ee/ci/variables/predefined_variables.html
		if ((ci == nullptr) || (strcmp(ci, "true") != 0))
		{
			GTEST_SKIP() << "Block IO test skipped since it is not running on a white-listed CI environment";
		}
	}
};

} // namespace (anonymous)

// If needed top up this wallet 2MxqLR4SXLobjQ6BGhnHMGcakauGWPrgLkN with faucet site
TEST_F(BlockIoApiTest, BalanceCheck)
{
	const double minimumBalance = 0.00010;
	insta::BlockIoApi source{ "3d22-351a-f9e0-d6d2", "0dm9spsgusrqv11gwuuu" };
	double balance = 0.0;
	EXPECT_NO_THROW(balance = source.GetBtcBalance());
	EXPECT_TRUE(balance > minimumBalance);
}

TEST_F(BlockIoApiTest, SingleSend)
{
	insta::BlockIoApi source{ "3d22-351a-f9e0-d6d2", "0dm9spsgusrqv11gwuuu" };
	std::string receivingAddress = "2N3zM8L6LyPEZr8b935cQBwiaWyZLLVH62G";
	insta::BtcSendingResult result;
	const double amountToSend = 0.00005;
	EXPECT_NO_THROW(result = source.SendBtc(amountToSend, receivingAddress));
	EXPECT_TRUE(result.IsSuccessful());
}

TEST_F(BlockIoApiTest, BatchSend)
{
	// Make sure that at least one of the 2 accounts has the minimum available balance
	// Credentials of davide.g@arringo.com account
	insta::BlockIoApi source{ "3d22-351a-f9e0-d6d2", "0dm9spsgusrqv11gwuuu" };

	std::string firstAddress = "2MsLcmNeEYrazkaEJohFNdGiMS7ihbY4SGj";
	std::string secondAddress = "2N6Ux2mekwLDeFnqhE7tWuaj8AQKX45nFP5";

	// Test batch (the sum is 0.00005, the same transferred in the SingleSend), the 2 accounts are
	// exchanging money
	const insta::BtcSendingApi::BtcBatchTrxSpecs specs = { { firstAddress, 0.00002 },
														   { secondAddress, 0.00003 } };
	insta::BtcSendingResult result;
	EXPECT_NO_THROW(result = source.SendBtc(specs));
	EXPECT_TRUE(result.IsSuccessful());
}

TEST_F(BlockIoApiTest, BatchSendToBech32Addresses)
{
	// Make sure that at least one of the 2 accounts has the minimum available balance
	// Credentials of davide.g@arringo.com account
	insta::BlockIoApi source{ "3d22-351a-f9e0-d6d2", "0dm9spsgusrqv11gwuuu" };

	// Address having 20 bytes
	std::string firstAddress = "tb1q9dmf0j24x32c5knxa66tw22eh4ly5e29cvjr4m";
	// Address having 32 bytes
	std::string secondAddress = "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7";

	// Test batch (the sum is 0.00005, the same transferred in the SingleSend), the 2 accounts are
	// exchanging money
	const insta::BtcSendingApi::BtcBatchTrxSpecs specs = { { firstAddress, 0.00002 },
														   { secondAddress, 0.00003 } };
	insta::BtcSendingResult result;
	EXPECT_NO_THROW(result = source.SendBtc(specs));
	EXPECT_TRUE(result.IsSuccessful());
}
