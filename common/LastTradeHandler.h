#pragma once

#include "MarketDataItem.h"

#include <limits>

namespace matcher
{
namespace common
{

template <typename Statistics>
class LastTradeHandler
{
	Statistics& statisticsHandler_;
	double lastTradePrice_;
	uint64_t totalQty_;
	uint64_t lastQty_;

public:
	LastTradeHandler(Statistics& stats) :
		  statisticsHandler_(stats)
	{
		reset();
	}
	~LastTradeHandler()
	{
	}

public:
	void processTrade(const MarketDataItem& mdItem)
	{
		if(mdItem.price < 0) {
			statisticsHandler_.addInvalidTrade();
		}
		else {
			if(std::fabs(lastTradePrice_ - mdItem.price) < std::numeric_limits<double>::epsilon()) {
				lastQty_ = mdItem.qty;
			}
			else {
				totalQty_ = 0;
				lastQty_ = mdItem.qty;
				lastTradePrice_ = mdItem.price;
			}

			totalQty_ += mdItem.qty;
			statisticsHandler_.addNormalTrade();
		}
	}

	void reset()
	{
		lastTradePrice_ = std::numeric_limits<double>::quiet_NaN();
		totalQty_ = 0;
		lastQty_ = 0;
	}

	double getLastPrice() const
	{
		return lastTradePrice_;
	}

	uint64_t getTotalQty() const
	{
		return totalQty_;
	}

	std::ostream& printTotalQty(std::ostream& os)
	{
		return os << "T," << lastQty_ << "," << lastTradePrice_ << " -> " << totalQty_ << "@" << lastTradePrice_ << std::endl;
	}
};

} // namespace common
} // namespace matcher
