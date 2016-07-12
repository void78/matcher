#pragma once

#include <string>

namespace matcher
{
namespace common
{

class MarketDataItem;

class Parser
{
public:
	Parser();
	~Parser();

public:
	bool parse(const std::string& , MarketDataItem& );

};

} // namespace common
} // namespace matcher
