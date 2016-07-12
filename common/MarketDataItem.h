#pragma once

#include <iostream>

namespace matcher
{
namespace common
{

enum class Action : char { New = 'A', Remove = 'X', Modify = 'M', Trade = 'T' };
enum class Side : char { Buy = 'B', Sell = 'S' };

// A,100004,B,10,950
struct MarketDataItem
{
	MarketDataItem(Action act, uint64_t q, double prc) :
		  action(act)
		, qty(q)
		, price(prc)
	{
	}
	MarketDataItem()
	{
	}

	Action action;
	uint64_t orderId;
	Side side;
	uint64_t qty;
	double price;
	
	void print(std::ostream& os)
	{
		os << static_cast<char>(action) << "," << static_cast<char>(side) << "," << qty << "," << price << std::endl;
	}
};

} // namespace common
} // namespace matcher
