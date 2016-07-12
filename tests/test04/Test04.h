#pragma once

#include <FileReader.h>
#include <Parser.h>
#include <StatisticsHandler.h>
#include <OrderBook.h>
#include <FeedHandler.h>
#include <LastTradeHandler.h>


#include <string>
#include <memory>

#include <cppunit/extensions/HelperMacros.h>


typedef matcher::common::FileReader FileReader;
typedef matcher::common::StatisticsHandler StatisticsHandler;
typedef matcher::common::Parser Parser;
typedef matcher::common::FeedHandler<FileReader, StatisticsHandler, Parser> FeedHandler;
typedef matcher::common::LastTradeHandler<StatisticsHandler> LastTradeHandler;

class Test04 : public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE(Test04);
		CPPUNIT_TEST(testCrosses);
		CPPUNIT_TEST(testSequence1);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDrop();

protected:
	void testCrosses();
	void testSequence1();

private:
	std::string fileName_;

	std::unique_ptr<FileReader> fileReader_;
	StatisticsHandler statistics_;
	std::unique_ptr<FeedHandler> feedHandler_;
	std::unique_ptr<LastTradeHandler> lastTradeHandler_;
};
