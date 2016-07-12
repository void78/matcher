#include "Parser.h"
#include "MarketDataItem.h"


#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>

namespace matcher
{
namespace common
{

Parser::Parser()
{
}

Parser::~Parser()
{
}

bool Parser::parse(const std::string& line, MarketDataItem& order)
{
	bool ret = true;
	// A,100000,S,1,1075
	std::stringstream ss(line);
	std::string item;

	char action;
	uint64_t orderId;
	char side;
	uint64_t qty;
	double price;

	try
	{
		std::getline(ss, item, ',');
		action = boost::lexical_cast<char>(item);
		switch(action)
		{
			case 'A': order.action = Action::New; break;
			case 'X': order.action = Action::Remove; break;
			case 'M': order.action = Action::Modify; break;
			case 'T': order.action = Action::Trade; break;
			default: return false; break;
		}

		if(order.action == Action::Trade) {
			std::getline(ss, item, ',');
			qty = boost::lexical_cast<uint64_t>(item);
			order.qty = qty;

			std::getline(ss, item, ',');
			price = boost::lexical_cast<double>(item);
			order.price = price;

			item.clear();

			std::getline(ss, item, ',');
			if(!item.empty()) {
				return false;
			}
		}
		else {
			std::getline(ss, item, ',');
			orderId = boost::lexical_cast<uint64_t>(item);
			order.orderId = orderId;

			std::getline(ss, item, ',');
			side = boost::lexical_cast<char>(item);
			switch(side)
			{
				case 'S': order.side = Side::Sell; break;
				case 'B': order.side = Side::Buy; break;
				default: return false; break;
			}


			std::getline(ss, item, ',');
			qty = boost::lexical_cast<uint64_t>(item);
			order.qty = qty;

			std::getline(ss, item, ',');
			price = boost::lexical_cast<double>(item);
			order.price = price;

			item.clear();

			std::getline(ss, item, ',');
			if(!item.empty()) {
				return false;
			}
		}

	}
	catch(const boost::bad_lexical_cast &)
	{
		ret = false;
	}


	return ret;
}

} // namespace common
} // namespace matcher

