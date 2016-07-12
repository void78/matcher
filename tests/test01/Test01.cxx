#include "Test01.h"

#include <stdexcept>

void Test01::setUp()
{
}

void Test01::tearDrop()
{
}

void Test01::testFileReaderInvalidFile()
{
	fileName_ = "No such file or directory";

	CPPUNIT_ASSERT_THROW_MESSAGE("Exception is expected", new matcher::common::FileReader(fileName_), std::runtime_error);
}

void Test01::testFileReaderEmptyFile()
{
	fileName_ = "../../../matcher/tests/test01/emptyFile.txt";

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception is not expected", fileReader_ = new matcher::common::FileReader(fileName_));

	CPPUNIT_ASSERT_EQUAL(false, fileReader_->hasNext());
}

void Test01::testFileReaderSimpleSequence()
{
	fileName_ = "../../../matcher/tests/test01/simpleSequence.txt";

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception is not expected", fileReader_ = new matcher::common::FileReader(fileName_));

	size_t count = 0;

	while(fileReader_->hasNext())
	{
		fileReader_->next();
		++count;
	}
	CPPUNIT_ASSERT_EQUAL(count, static_cast<size_t>(9));
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test01);
