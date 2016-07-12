cmake_minimum_required (VERSION 2.8)

project (MATCHER)

set(CXX g++)

set(UNIT_TEST_PATH ${MATCHER_SOURCE_DIR}/tests)

add_definitions(-std=c++0x)
add_definitions(-static-libstdc++)
add_definitions(-Wall)
add_definitions(-pedantic)

find_path( CPPUNIT_LIBRARY libcppunit.so
	/home/algo/Packages/CppUnit/1.12.1/linux_4.7.0/lib
	/usr/lib)
find_path( CPPUNIT_INCLUDE cppunit/TestSuite.h
	/home/algo/Packages/CppUnit/1.12.1/include
	/usr/include)

find_path( BOOST_INCLUDE boost/version.hpp
	/home/AFadeev/boost_1.59.0/include/
	/usr/include
)
find_path( BOOST_LIBRARY libboost_filesystem.so
	/home/AFadeev/boost_1.59.0/lib
	/usr/lib
)

include_directories( ${CPPUNIT_INCLUDE} )
include_directories (${BOOST_INCLUDE})

link_directories( ${CPPUNIT_LIBRARY} )
link_directories( ${BOOST_LIBRARY} )

add_subdirectory(matcher)
add_subdirectory(tests)
