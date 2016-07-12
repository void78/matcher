#include "Test02.h"

#include <MarketDataItem.h>

#include <stdexcept>

#include <boost/filesystem.hpp>

void Test02::setUp()
{
}

void Test02::tearDrop()
{
}

void Test02::testParserInvalidSequence()
{
	fileName_ = "../../../matcher/tests/test02/invalidSequence.txt";

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception is not expected", fileReader_ = new matcher::common::FileReader(fileName_));

	size_t count = 0;

	while(fileReader_->hasNext())
	{
		const std::string& line = fileReader_->next();
		matcher::common::MarketDataItem mdItem;

//		std::cout << "line is: " << line << std::endl;

		CPPUNIT_ASSERT_EQUAL(false, parser_.parse(line, mdItem));

		++count;
	}
	CPPUNIT_ASSERT_EQUAL(count, static_cast<size_t>(13));

	delete fileReader_;
}

void Test02::testParserValidSequence()
{
	fileName_ = "../../../matcher/tests/test02/validSequence.txt";
	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception is not expected", fileReader_ = new matcher::common::FileReader(fileName_));

	size_t count = 0;

	while(fileReader_->hasNext())
	{
		const std::string& line = fileReader_->next();
		matcher::common::MarketDataItem mdItem;

//		std::cout << "line is: " << line << std::endl;
		CPPUNIT_ASSERT_EQUAL(true, parser_.parse(line, mdItem));
		++count;
	}
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), count);

	delete fileReader_;
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test02);
