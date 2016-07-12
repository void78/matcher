#include "OrderBook.h"

namespace matcher
{
namespace common
{

std::ostream&  operator << (std::ostream& os, const OrderBookLevel& level)
{
	if(level.side == Side::Sell)
		return os << level.price << "@" << level.qty << "(" << level.orders.size() << ")..";

	return os  << "(" << level.orders.size() << ")" << level.qty << "@" << level.price;
}

} // namespace common
} // namespace matcher
