#pragma once

#include <FileReader.h>
#include <Parser.h>

#include <string>

#include <cppunit/extensions/HelperMacros.h>

class Test02 : public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE(Test02);
		CPPUNIT_TEST(testParserInvalidSequence);
		CPPUNIT_TEST(testParserValidSequence);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDrop();

protected:
	void testParserInvalidSequence();
	void testParserValidSequence();

private:
	matcher::common::FileReader * fileReader_;
	std::string fileName_;
	matcher::common::Parser parser_;
};
