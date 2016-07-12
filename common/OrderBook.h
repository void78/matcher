#pragma once

#include "MarketDataItem.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>

namespace matcher
{
namespace common
{

struct Order
{
	uint64_t orderId;
	uint64_t qty;

	Order (uint64_t ordId = 0, uint64_t q = 0) :
		  orderId(ordId)
		, qty(q)
	{
	}

	bool operator < (const Order& rhs) const
	{
		return orderId < rhs.orderId;
	}

	bool operator == (const Order& rhs) const
	{
		return orderId == rhs.orderId &&
			qty == rhs.qty;
	}

};

typedef boost::multi_index::multi_index_container<
	Order,
	boost::multi_index::indexed_by<
		boost::multi_index::sequenced<>,
		boost::multi_index::hashed_unique<boost::multi_index::member<Order, uint64_t, &Order::orderId> >
	>
> OrdersList;

struct OrderBookLevel
{
	double price;
	uint64_t qty;
	Side side;
	OrdersList orders;

	OrderBookLevel(const Side& s) : price(0.0), qty(0), side(s)
	{
	}
};

std::ostream&  operator << (std::ostream& os, const OrderBookLevel& level);

typedef std::unique_ptr<OrderBookLevel> LevelPtr;

struct OrderBookLess : public std::binary_function<double, double, bool>
{
	const Side side_;
	OrderBookLess(Side side) : side_(side) {}

	bool operator() (const double& lhs, const double& rhs) const
	{
		return side_ == Side::Buy ? - lhs + rhs > std::numeric_limits<double>::epsilon() :
			- rhs + lhs > std::numeric_limits<double>::epsilon();
	}
};

template <typename Statistics>
class OrderBook;

template <typename Statistics>
class OrderBookSide
{
	typedef std::map<double, LevelPtr, OrderBookLess> OrdersMap;

	Statistics& statisticsHandler_;
	const Side side_;
	OrdersMap orders_;

public:
	OrderBookSide(Statistics& stats, Side side) :
		  statisticsHandler_(stats)
		, side_(side)
		, orders_(OrderBookLess(side_))
	{
	}

	friend class OrderBook<Statistics>;

public:
	double getTopPrice() const
	{
		if(isEmpty())
			return std::numeric_limits<double>::quiet_NaN();

		return side_ == Side::Buy ? orders_.rbegin()->first : orders_.rbegin()->first;

	}

	bool isEmpty() const
	{
		return orders_.empty();
	}

	void addOrder(MarketDataItem& order)
	{
		auto itr = orders_.find(order.price);
		if(itr == orders_.end()) {
			// std::unique is available for C++x14 only
			auto ret = orders_.insert(std::make_pair(order.price, LevelPtr(new OrderBookLevel(side_))));
			itr = ret.first;
			// New level - let's set up price
			itr->second->price = order.price;
			statisticsHandler_.incrementDepth(side_);
		}

		LevelPtr& level = itr->second;
		Order newOrder = {order.orderId, order.qty};

		auto ret = level->orders.push_back(newOrder);

		if(ret.second) {
			level->qty += order.qty;
			statisticsHandler_.addNewOrder();
			statisticsHandler_.addVolume(side_, order.qty);
		}
		else {
			statisticsHandler_.addDuplicatedOrder();
		}
	}
	void modifyOrder(MarketDataItem& order)
	{
		auto itr = orders_.find(order.price);
		if(itr == orders_.end()) {
			statisticsHandler_.addMissingOrder();
		}
		else {
			auto& level = itr->second;
			auto& orderIdIndex = level->orders.get<1>();

			auto it = orderIdIndex.find(order.orderId);
			if(it == orderIdIndex.end()) {
				statisticsHandler_.addMissingOrder();
			}
			else {
				Order update(*it);
				level->qty -= update.qty;
				statisticsHandler_.removeVolume(side_, update.qty);

				update.qty = order.qty;
				level->qty += update.qty;
				orderIdIndex.replace(it, update);
				statisticsHandler_.addVolume(side_, update.qty);

				statisticsHandler_.addModifyOrder();
			}
		}
	}
	void removeOrder(MarketDataItem& order)
	{
		auto itr = orders_.find(order.price);
		if(itr == orders_.end()) {
			statisticsHandler_.addMissingOrder();
		}
		else {
			auto& level = itr->second;
			auto& orderIdIndex = level->orders.get<1>();

			auto it = orderIdIndex.find(order.orderId);

			if(it == orderIdIndex.end()) {
				statisticsHandler_.addInvalidRemoveOrder();
			}
			else {
				const Order& orderToRemove(*it);
				level->qty -= orderToRemove.qty;
				statisticsHandler_.removeVolume(side_, order.qty);

				orderIdIndex.erase(it);
				if(level->orders.empty()) {
					orders_.erase(itr);
					statisticsHandler_.decrementDepth(side_);
				}

				statisticsHandler_.addRemoveOrder();
			}

		}
	}
};

template <typename Statistics>
class OrderBook
{
	Statistics& statisticsHandler_;
	OrderBookSide<Statistics> bids_;
	OrderBookSide<Statistics> asks_;
	bool crossed_;

public:
	OrderBook(Statistics& stats) :
		  statisticsHandler_(stats)
		, bids_(stats, Side::Buy)
		, asks_(stats, Side::Sell)
		, crossed_(false)
	{
	}

public:
	void applyIncremental(MarketDataItem& incremental)
	{
		struct timespec start;
		clock_gettime(CLOCK_MONOTONIC, &start);

		auto& side = incremental.side == Side::Buy ? bids_ : asks_;

		if(incremental.action == Action::New) {
			side.addOrder(incremental);
		}
		else if(incremental.action == Action::Remove) {
			side.removeOrder(incremental);
		}
		else if(incremental.action == Action::Modify) {
			side.modifyOrder(incremental);
		}

		if(!crossed_ && isCrossed()) {
			crossed_ = true;
			statisticsHandler_.addCross();
		}
		if(crossed_ && !isCrossed()) {
			crossed_ = false;
		}
		struct timespec end;
		clock_gettime(CLOCK_MONOTONIC, &end);

		statisticsHandler_.updateMetric(start, end);
	}

	double getMidQuote() const
	{
		double ret = std::numeric_limits<double>::quiet_NaN();

		if(!bids_.isEmpty() && !asks_.isEmpty()) {
			ret = (bids_.getTopPrice() + asks_.getTopPrice() ) / 2.0;
		}

		return ret;
	}

	std::ostream& printCurrentOrderBook(std::ostream& os) const
	{
		size_t counter = 0;
		size_t width = 25;
		// Header
		os << "==|" << std::setw(width) << std::setfill('=') << "=" << "|" << std::setw(width) << std::setfill('=') << "=" << std::endl;
		os << "  |" << std::setw(width) << std::setfill(' ') << std::right << "Bid   " << "|" << std::setw(width) << std::setfill(' ') << std::left << "   Ask" << std::endl;
		os << "==|" << std::setw(width) << std::setfill('=') << "=" << "|" << std::setw(width) << std::setfill('=') << "=" << std::endl;

		// Let's collect all orders
		struct Details
		{
			Details() : total(0), qty(0)
			{
			}
			uint64_t total;
			uint64_t qty;
		};
		std::map<double, std::tuple<Details, Details>, std::greater<double>> levels;

		for(const auto& item : asks_.orders_) {
			auto& level = levels[item.first];
			auto& detail = std::get<1>(level);
			detail.qty += item.second->qty;
			detail.total += item.second->orders.size();
		}
		for(const auto& item : bids_.orders_) {
			auto& level = levels[item.first];
			auto& detail = std::get<0>(level);
			detail.qty += item.second->qty;
			detail.total += item.second->orders.size();
		}

		for(const auto& it : levels) {
			os << std::setw(2) << std::setfill(' ') << std::right << ++counter << "|";

			std::ostringstream oss;

			auto& bid = std::get<0>(it.second);
			auto& ask = std::get<1>(it.second);

			if(bid.total) {

				oss << "(" << bid.total << ")" << bid.qty << "@" << it.first;
			}
			os << std::setw(width-oss.str().size()) << std::setfill('.') << '.' << oss.str();
			os << '|';
			oss.str("");
			if(ask.total) {

				oss << it.first << "@" << ask.qty << "(" << ask.total << ")";
			}
			os << oss.str() << std::setw(width-oss.str().size()) << std::setfill('.') << '.';
			os << std::endl;
		}
		os << "---" << std::setw(width*2 + 1) << std::setfill('-') << '-' << std::endl;
		return os;
	}

	std::ostream& printMidQuote(std::ostream& os) const
	{
		double ret = getMidQuote();

		return os << "Mid quote: " << ret << std::endl;
	}

private:
	std::tuple<double, double> getTopOfBook() const
	{
		return std::make_tuple(bids_.getTopPrice(), asks_.getTopPrice());
	}

	bool isCrossed() const
	{
		bool ret = false;

		if(!bids_.isEmpty() && !asks_.isEmpty()) {
			if(bids_.getTopPrice() - asks_.getTopPrice() >= std::numeric_limits<double>::epsilon()) {
				ret = true;
			}
		}

		return ret;
	}
};

} // namespace common
} // namespace matcher
