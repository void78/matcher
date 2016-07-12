#include "FileReader.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace matcher
{
namespace common
{
FileReader::FileReader(const std::string& fileName) :
	ifs_(fileName.c_str(), std::ios::in)
{
	if(!ifs_.good()) {
		std::ostringstream oss;
		oss << "Failed to open file '" << fileName << "'. errno = " << errno << std::endl;

		throw std::runtime_error(oss.str().c_str());
	}
}

FileReader::~FileReader()
{
	ifs_.close();
}

bool FileReader::hasNext() const
{
	return std::getline(ifs_, order_);
}

const std::string& FileReader::next() const
{
	return order_;
}

} // namespace common
} // namespace matcher
