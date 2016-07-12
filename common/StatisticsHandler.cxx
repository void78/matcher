#include "StatisticsHandler.h"

#include <limits>
#include <iterator>

namespace
{
//	struct timespec {
//		time_t   tv_sec;        /* seconds */
//		long     tv_nsec;       /* nanoseconds */
//	};
struct timespec operator-(const struct timespec& end, const struct timespec& start)
{
	struct timespec res;
	
	res.tv_sec = end.tv_sec - start.tv_sec;

	if(end.tv_nsec < start.tv_nsec) {
		res.tv_nsec = end.tv_nsec + 1000000000 - start.tv_nsec;
		res.tv_sec -= 1;
	}
	else {
		res.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return res;
}

bool operator < (const struct timespec& lhs, const struct timespec& rhs)
{
	if(lhs.tv_sec < rhs.tv_sec)
		return true;
	else if(lhs.tv_sec == rhs.tv_sec)
		return lhs.tv_nsec < rhs.tv_nsec;

	return false;
}
struct timespec& operator += (struct timespec& lhs, const struct timespec& rhs)
{
	lhs.tv_sec += rhs.tv_sec;
	lhs.tv_nsec += rhs.tv_nsec;
	if(lhs.tv_nsec >= 1000000000) {
		++lhs.tv_sec;
		lhs.tv_nsec -= 1000000000;
	}

	return lhs;
}

}

namespace matcher
{
namespace common
{

StatisticsHandler::StatisticsHandler()
{
	reset();
}

StatisticsHandler::~StatisticsHandler()
{
}

void StatisticsHandler::print(std::ostream& os)
{
	std::string indent;

	os << "--------------------------------------------" << std::endl;
	os << "\t\tSummary:" << std::endl;
	os << "--------------------------------------------" << std::endl;

	indent = "";

	os << "+-+ Messages processed: " << totalMessages_ << std::endl;
	os << "| +-------+ Orders: " << totalOrders_ << std::endl;
	os << "| |       |--- New: " << totalNewOrders_ << std::endl;
	os << "| |       |--- Remove: " << totalRemoveOrders_ << std::endl;
	os << "| |       |--- Modify: " << totalModifyOrders_ << std::endl;
	os << "| |       |--- Duplicated: " << totalDuplicatedOrders_ << std::endl;
	os << "| |       |--- Missing: " << totalMissingOrders_ << std::endl;
	os << "| |" << std::endl;
	os << "| +-------+ Trades: " << totalTrades_ << std::endl;
	os << "| |       |--- Normal: " << normalTrades_ << std::endl;
	os << "| |       |--- Invalid: " << invalidTrades_ << std::endl;
	os << "| |" << std::endl;
	os << "| +-------+ Corrupted: " << corruptedMessages_ << std::endl;
	os << "|" << std::endl;
	os << "|-+ Other statistics:" << std::endl;
	os << "  |" << std::endl;
	os << "  +--- Min time processing: " << getMinTime() << " nsec" << std::endl;
	os << "  +--- Max time processing: " << getMaxTime() << " nsec" << std::endl;
	os << "  +--- Avg time processing: " << getAvgTime() << " nsec" << std::endl;
	os << "  |" << std::endl;
	os << "  +--- Bid depth: " << getDepth(Side::Buy) << std::endl;
	os << "  +--- Bid volume: " << getVolume(Side::Buy) << std::endl;
	os << "  +--- Ask depth: " << getDepth(Side::Sell) << std::endl;
	os << "  +--- Ask volume: " << getVolume(Side::Sell) << std::endl;
	os << "  |" << std::endl;
	os << "  +--- Crossed: " << getCrosses() << std::endl;

	os  << std::endl;
}

uint64_t StatisticsHandler::getTotalMessages() const
{
	return totalMessages_;
}

// Corrupted Messages
void StatisticsHandler::addCorruptedMessage()
{
	++totalMessages_;
	++corruptedMessages_;
}
uint64_t StatisticsHandler::getCorruptedMessage() const
{
	return corruptedMessages_;
}

// New order
void StatisticsHandler::addNewOrder()
{
	++totalMessages_;
	++totalOrders_;
	++totalNewOrders_;
}
uint64_t StatisticsHandler::getNewOrder() const
{
	return totalNewOrders_;
}

// Remove orders
void StatisticsHandler::addRemoveOrder()
{
	++totalMessages_;
	++totalOrders_;
	++totalRemoveOrders_;
}
uint64_t StatisticsHandler::getRemoveOrder() const
{
	return totalRemoveOrders_;
}

// Modify orders
void StatisticsHandler::addModifyOrder()
{
	++totalMessages_;
	++totalOrders_;
	++totalModifyOrders_;
}
uint64_t StatisticsHandler::getModifyOrder() const
{
	return totalModifyOrders_;
}

// Duplicated orders
void StatisticsHandler::addDuplicatedOrder()
{
	++totalMessages_;
	++totalOrders_;
	++totalDuplicatedOrders_;
}
uint64_t StatisticsHandler::getDuplicatedOrder() const
{
	return totalDuplicatedOrders_;
}

// Invalid remove
void StatisticsHandler::addInvalidRemoveOrder()
{
	++totalMessages_;
	++invalidRemoves_;
}
uint64_t StatisticsHandler::getInvalidRemoveOrder() const
{
	return invalidRemoves_;
}

// Missing price
void StatisticsHandler::addMissingOrder()
{
	++totalMessages_;
	++totalOrders_;
	++totalMissingOrders_;
}
uint64_t StatisticsHandler::getMissingOrder() const
{
	return totalMissingOrders_;
}


// Invalid trades
void StatisticsHandler::addInvalidTrade()
{
	++totalMessages_;
	++totalTrades_;
	++invalidTrades_;
}
uint64_t StatisticsHandler::getInvalidTrades() const
{
	return invalidTrades_;
}

// Normal trades
void StatisticsHandler::addNormalTrade()
{
	++totalMessages_;
	++totalTrades_;
	++normalTrades_;
}
uint64_t StatisticsHandler::getNormalTrades() const
{
	return normalTrades_;
}

// Total trades
uint64_t StatisticsHandler::getTotalTrades() const
{
	return totalTrades_;
}

// Total orders
uint64_t StatisticsHandler::getTotalOrders() const
{
	return totalOrders_;
}

// Order book statistics
void StatisticsHandler::incrementDepth(Side side)
{
	if(side == Side::Buy)
		++bidDepth_;
	else
		++askDepth_;
}
void StatisticsHandler::decrementDepth(Side side)
{
	if(side == Side::Buy)
		--bidDepth_;
	else
		--askDepth_;
}
uint64_t StatisticsHandler::getDepth(Side side) const
{
	if(side == Side::Buy)
		return bidDepth_;

	return askDepth_;
}

void StatisticsHandler::addVolume(Side side, uint64_t vol)
{
	if(side == Side::Buy)
		bidVolume_ += vol;
	else
		askVolume_ += vol;

}
void StatisticsHandler::removeVolume(Side side, uint64_t vol)
{
	if(side == Side::Buy)
		bidVolume_ -= vol;
	else
		askVolume_ -= vol;
}

uint64_t StatisticsHandler::getVolume(Side side)
{
	if(side == Side::Buy)
		return bidVolume_;
	else
		return askVolume_;
}

void StatisticsHandler::updateMetric(struct timespec& start, struct timespec& end)
{
	struct timespec ts = end - start;

	if(ts < minProcessing_)
		minProcessing_ = ts;
	if(maxProcessing_ < ts)
		maxProcessing_ = ts;

	sumProcessing_ += ts;
}

// Crosses
void StatisticsHandler::addCross()
{
	++crosses_;
}
uint64_t StatisticsHandler::getCrosses() const
{
	return crosses_;
}

uint64_t StatisticsHandler::getMinTime() const
{
	return minProcessing_.tv_sec * 1000000000 + minProcessing_.tv_nsec;
}

uint64_t StatisticsHandler::getMaxTime() const
{
	return maxProcessing_.tv_sec * 1000000000 + maxProcessing_.tv_nsec;
}
uint64_t StatisticsHandler::getAvgTime() const
{
	return (sumProcessing_.tv_sec * 1000000000 + sumProcessing_.tv_nsec) / (totalOrders_ + totalTrades_);
}
// -----------------------------------------------
void StatisticsHandler::reset()
{
	totalMessages_ = corruptedMessages_ = totalOrders_ = totalDuplicatedOrders_ = 
	totalNewOrders_ = totalRemoveOrders_ = totalModifyOrders_ = totalTrades_ = 
	invalidTrades_ = invalidRemoves_ = totalMissingOrders_ = normalTrades_ = 0;

	bidDepth_ = askDepth_ = bidVolume_ = askVolume_ = 0;
	crosses_ = 0;

	minProcessing_.tv_sec = std::numeric_limits<time_t>::max();
	minProcessing_.tv_nsec = std::numeric_limits<long>::max();

	maxProcessing_.tv_sec = maxProcessing_.tv_nsec = 0;
	sumProcessing_.tv_sec = sumProcessing_.tv_nsec = 0;
}

} // namespace common
} // namespace matcher
