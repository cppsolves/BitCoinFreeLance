#include "pch.h"

#include "GeneralUtils.h"
#include "net/AwsS3Service.h"
#include "utils/Config.h"
#include "utils/Sha.h"
#include "utils/Exception.h"

using namespace std::chrono_literals;

// This test uses aws s3 bucket defined by the config, so be careful changing the path (key value).

TEST(AwsS3ServiceTest, FileUpload)
{
	const std::string filename{ "test-image.jpeg" };
	std::ifstream stream{ "test-res/" + filename, std::fstream::in };

	EXPECT_TRUE(stream);

	fs::path key{ "docs/tests" };
	key /= filename;

	stream.seekg(std::ios::beg);
	EXPECT_NO_THROW(net::AwsS3Service::Singleton().Upload(key, stream, "image/jpeg", "en"));

	std::stringstream ss;
	EXPECT_NO_THROW(net::AwsS3Service::Singleton().Download(key, ss));
	ss.seekg(std::ios::end);
	EXPECT_TRUE(ss.tellg());

	net::AwsS3Service::Singleton().Delete(key);
	try
	{
		net::AwsS3Service::Singleton().Download(key, ss);
	}
	catch (const utils::BadRequest& ex)
	{
		EXPECT_EQ(ex.GetReason(), "Invalid data");
	}
}

TEST(AwsS3ServiceTest, FileDownload)
{
	const std::string data = "THIS IS TEST TEMP FILE, NOTHING SPECIAL ABOUT IT ;)\n";
	const fs::path key = "tempfile.txt";

	std::string link;

	{
		std::stringstream filestream;
		filestream << data;

		link = net::AwsS3Service::Singleton().UploadTempFile(
			key,
			std::move(filestream),
			"temp/test",
			5	 // seconds
		);
	}
	{
		std::string downloadedData = test::DownloadFromAwsLink(link);

		EXPECT_EQ(downloadedData, data);

		std::this_thread::sleep_for(7s);

		try
		{
			test::DownloadFromAwsLink(link);
		}
		catch (const std::exception& ex)
		{
			const std::string exVal{ ex.what() };
			EXPECT_EQ(exVal, "Downloading request failed with code FORBIDDEN.");
		}
	}
}