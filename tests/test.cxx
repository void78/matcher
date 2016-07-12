#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

int main(int argc, char * argv[])
{
	CPPUNIT_NS :: TestResult controller;
	CPPUNIT_NS :: TestResultCollector results;
	controller.addListener( &results );
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );

	CPPUNIT_NS::TestRunner runner;
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
	runner.run( controller );

	CPPUNIT_NS::CompilerOutputter outputter( &results, CPPUNIT_NS::stdCOut() );
	outputter.write();

	return results.wasSuccessful() ? 0 : 1;
}
