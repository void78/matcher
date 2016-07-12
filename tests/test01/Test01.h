#pragma once

#include <FileReader.h>

#include <string>

#include <cppunit/extensions/HelperMacros.h>

class Test01 : public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE(Test01);
		CPPUNIT_TEST(testFileReaderInvalidFile);
		CPPUNIT_TEST(testFileReaderEmptyFile);
		CPPUNIT_TEST(testFileReaderSimpleSequence);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDrop();

protected:
	void testFileReaderInvalidFile();
	void testFileReaderEmptyFile();
	void testFileReaderSimpleSequence();

private:
	matcher::common::FileReader * fileReader_;
	std::string fileName_;
};
