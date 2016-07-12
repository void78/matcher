#include "Test03.h"

#include <MarketDataItem.h>

#include <stdexcept>

#include <boost/filesystem.hpp>

namespace mc = matcher::common;

void Test03::setUp()
{
	fileName_ = "../../../matcher/tests/test03/initialSequence.txt";
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

void Test03::tearDrop()
{
	fileReader_.reset();
	feedHandler_.reset();

	statistics_.reset();
}


//bool operator == (const mc::Order& lhs, const mc::Order& rhs)
//{
//	return lhs.orderId == rhs.orderId &&
//		lhs.qty == rhs.qty;
//}

void Test03::testOrderBookLevel()
{
	mc::OrdersList level;

	mc::Order o1 = {100, 3};
	mc::Order o2 = {101, 5};
	mc::Order o3 = {102, 2};

	level.push_back(o1);
	level.push_back(o2);
	level.push_back(o3);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), level.size());

	auto& orderIdIndex = level.get<1>();
	auto it = orderIdIndex.find(1000);
	CPPUNIT_ASSERT(it == orderIdIndex.end());

	auto o1Itr = orderIdIndex.find(100);
	CPPUNIT_ASSERT(o1Itr != orderIdIndex.end());
	CPPUNIT_ASSERT(mc::Order(100, 3) == *o1Itr);

	mc::Order newOrder(*o1Itr);
	newOrder.qty = 1;
	orderIdIndex.replace(o1Itr, newOrder);

	auto& sequenceOfOrder = level.get<0>();
	auto itr = sequenceOfOrder.begin();

	CPPUNIT_ASSERT(mc::Order(100, 1) == *itr++);
	CPPUNIT_ASSERT(mc::Order(101, 5) == *itr++);
	CPPUNIT_ASSERT(mc::Order(102, 2) == *itr++);

	orderIdIndex.erase(o1Itr);
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), level.size());
}

void Test03::testMakeOrderBook()
{
	feedHandler_->exec();

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), statistics_.getTotalMessages());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), statistics_.getNewOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getRemoveOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), statistics_.getModifyOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getDuplicatedOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getMissingOrder());
}

void Test03::testLastTradeHandler()
{
	mc::MarketDataItem item1 = {mc::Action::Trade, 1, 100.0};
	lastTradeHandler_->processTrade(item1);

	CPPUNIT_ASSERT_EQUAL(100.0, lastTradeHandler_->getLastPrice());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lastTradeHandler_->getTotalQty());

	lastTradeHandler_->processTrade({mc::Action::Trade, 2, 100.0});
	CPPUNIT_ASSERT_EQUAL(100.0, lastTradeHandler_->getLastPrice());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), lastTradeHandler_->getTotalQty());

	lastTradeHandler_->processTrade({mc::Action::Trade, 5, 101.0});
	CPPUNIT_ASSERT_EQUAL(101.0, lastTradeHandler_->getLastPrice());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), lastTradeHandler_->getTotalQty());
}

void Test03::testProcessFullSequence()
{
	// just not to create separate TestXX
	fileName_ = "../../../matcher/tests/test03/completeSequence.txt";
	fileReader_.reset(new matcher::common::FileReader(fileName_));
	feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));

	feedHandler_->exec();

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), statistics_.getTotalMessages());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), statistics_.getNewOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), statistics_.getRemoveOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getModifyOrder());


	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), statistics_.getDepth(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), statistics_.getDepth(mc::Side::Sell));

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), statistics_.getVolume(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), statistics_.getVolume(mc::Side::Sell));

	statistics_.print(std::cout);
}

void Test03::testCrossedSequence()
{
	// just not to create separate file
	fileName_ = "../../../matcher/tests/test03/cross.txt";
	fileReader_.reset(new matcher::common::FileReader(fileName_));
	feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));

	feedHandler_->exec();
}

void Test03::testTrade()
{
	fileName_ = "../../../matcher/tests/test03/oneTrade.txt";
	fileReader_.reset(new matcher::common::FileReader(fileName_));
	feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));

	feedHandler_->exec();

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), statistics_.getTotalMessages());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), statistics_.getNewOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), statistics_.getRemoveOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getModifyOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), statistics_.getCorruptedMessage());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), statistics_.getTotalTrades());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), statistics_.getNormalTrades());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getInvalidTrades());

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), statistics_.getDepth(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), statistics_.getDepth(mc::Side::Sell));

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(50), statistics_.getVolume(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), statistics_.getVolume(mc::Side::Sell));

//	statistics_.print(std::cout);
}

void Test03::testNormalTrade()
{
	fileName_ = "../../../matcher/tests/test03/normalTrade.txt";
	fileReader_.reset(new matcher::common::FileReader(fileName_));
	feedHandler_.reset(new FeedHandler(*fileReader_, statistics_));

	feedHandler_->exec();

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), statistics_.getTotalMessages());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), statistics_.getNewOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), statistics_.getRemoveOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getModifyOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getMissingOrder());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), statistics_.getCorruptedMessage());

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), statistics_.getDepth(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), statistics_.getDepth(mc::Side::Sell));

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(49), statistics_.getVolume(mc::Side::Buy));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(133), statistics_.getVolume(mc::Side::Sell));

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(133), statistics_.getVolume(mc::Side::Sell));

//	statistics_.print(std::cout);
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test03);
