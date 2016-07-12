#pragma once

#include "MarketDataItem.h"
#include "OrderBook.h"
#include "LastTradeHandler.h"

#include <iostream>

namespace matcher
{
namespace common
{

template <typename MarketDataProvider, typename Statistics, typename Parser>
class FeedHandler
{
	MarketDataProvider& marketDataProvider_;
	Statistics& statisticsHandler_;
	Parser parser_;
	OrderBook<Statistics> orderBook_;
	LastTradeHandler<Statistics> lastTradeHandler_;

public:
	FeedHandler(MarketDataProvider& mdProvider, Statistics& stats) :
		  marketDataProvider_(mdProvider)
		, statisticsHandler_(stats)
		, orderBook_(statisticsHandler_)
		, lastTradeHandler_(statisticsHandler_)
	{
	}
	~FeedHandler()
	{
	}

public:
	void exec(bool debugModeOn = false)
	{
		uint64_t count = 0;
		while(marketDataProvider_.hasNext())
		{
			const std::string& line = marketDataProvider_.next();
			MarketDataItem mdItem;

			if(!parser_.parse(line, mdItem)) {
				statisticsHandler_.addCorruptedMessage();
			}
			else {
				switch (mdItem.action)
				{
					case Action::Trade:
						lastTradeHandler_.processTrade(mdItem);
						lastTradeHandler_.printTotalQty(std::cout);
						break;
					case Action::New:
					case Action::Remove:
					case Action::Modify:
						orderBook_.applyIncremental(mdItem);
						break;
					default:
						statisticsHandler_.addCorruptedMessage();
						break;
				}

				orderBook_.printMidQuote(std::cout);
				if(debugModeOn && ++count%10 == 0) {
					orderBook_.printCurrentOrderBook(std::cerr);
				}
			}
		}

		orderBook_.printCurrentOrderBook(std::cout);
	}

};

} // namespace common
} // namespace matcher
