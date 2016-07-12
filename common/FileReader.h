#pragma once

#include <fstream>

namespace matcher
{
namespace common
{

class FileReader
{
	mutable std::ifstream ifs_;
	mutable std::string order_;

public:
	FileReader(const std::string& fileName);
	~FileReader();

public:
	bool hasNext() const;
	const std::string& next() const;
};

} // namespace common
} // namespace matcher
