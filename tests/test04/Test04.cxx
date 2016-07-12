#include "Test04.h"

#include <MarketDataItem.h>

#include <stdexcept>

#include <boost/filesystem.hpp>

namespace mc = matcher::common;

void Test04::setUp()
{
	fileName_ = "../../../matcher/tests/test04/validSequence.txt";
	try
	{
		fileReader_.reset(new matcher::common::FileReader(fileName_));
		feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));
		lastTradeHandler_.reset(new LastTradeHandler(statistics_));
	}
	catch(...)
	{
	}
}

void Test04::tearDrop()
{
	fileReader_.reset();
	feedHandler_.reset();

	statistics_.reset();
}


void Test04::testCrosses()
{
	feedHandler_->exec();
//	statistics_.print(std::cout);
}

void Test04::testSequence1()
{
	fileName_ = "../../../matcher/tests/test04/sequence-1.txt";

	fileReader_.reset(new matcher::common::FileReader(fileName_));
	feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));
	lastTradeHandler_.reset(new LastTradeHandler(statistics_));

	feedHandler_->exec();

	//Buy: depth 10, total qty 49634
	//Sell: depth 15, total qty 62633
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), statistics_.getDepth(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), statistics_.getDepth(mc::Side::Sell));

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(49634), statistics_.getVolume(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(62633), statistics_.getVolume(mc::Side::Sell));

//	statistics_.print(std::cout);
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test04);
