#pragma once

#include "MarketDataItem.h"

#include <time.h>

#include <cstdint>
#include <ostream>

namespace matcher
{
namespace common
{

class StatisticsHandler
{
	uint64_t totalMessages_;
	uint64_t corruptedMessages_;
	uint64_t totalOrders_;
	uint64_t totalDuplicatedOrders_;
	uint64_t totalNewOrders_;
	uint64_t totalRemoveOrders_;
	uint64_t totalModifyOrders_;
	uint64_t invalidRemoves_;
	uint64_t totalMissingOrders_;

	uint64_t totalTrades_;
	uint64_t normalTrades_;
	uint64_t invalidTrades_;

	uint64_t bidDepth_;
	uint64_t askDepth_;
	uint64_t bidVolume_;
	uint64_t askVolume_;
	uint64_t crosses_;

	// performace statistics
	struct timespec minProcessing_;
	struct timespec maxProcessing_;
	struct timespec sumProcessing_;


public:
	StatisticsHandler();
	~StatisticsHandler();

public:
	uint64_t getTotalMessages() const;

	void addCorruptedMessage();
	uint64_t getCorruptedMessage() const;

	void addNewOrder();
	uint64_t getNewOrder() const;

	void addRemoveOrder();
	uint64_t getRemoveOrder() const;
	
	void addModifyOrder();
	uint64_t getModifyOrder() const;

	void addDuplicatedOrder();
	uint64_t getDuplicatedOrder() const;

	void addInvalidRemoveOrder();
	uint64_t getInvalidRemoveOrder() const;

	void addMissingOrder();
	uint64_t getMissingOrder() const;

	uint64_t getTotalTrades() const;

	void addInvalidTrade();
	uint64_t getInvalidTrades() const;

	void addNormalTrade();
	uint64_t getNormalTrades() const;

	uint64_t getTotalOrders() const;

	void incrementDepth(Side side);
	void decrementDepth(Side side);
	uint64_t getDepth(Side side) const;

	void addVolume(Side side, uint64_t);
	void removeVolume(Side side, uint64_t);
	uint64_t getVolume(Side side);

	void addCross();
	uint64_t getCrosses() const;

	void updateMetric(struct timespec& start, struct timespec& end);

	uint64_t getMinTime() const;
	uint64_t getMaxTime() const;
	uint64_t getAvgTime() const;

	void print(std::ostream& os);
	void reset();
};

} // namespace common
} // namespace matcher
