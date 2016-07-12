cmake_minimum_required(VERSION 2.8)

project(matcher)


include_directories( ${MATCHER_SOURCE_DIR} )

message( STATUS "CPPUNIT_INCLUDE = ${CPPUNIT_INCLUDE}" )
message( STATUS "CPPUNIT_LIBRARY = ${CPPUNIT_LIBRARY}" )
message( STATUS "CMAKE_BINARY_DIR  = ${CMAKE_BINARY_DIR}" )
message( STATUS "COMMIN_INCLUDE  = ${COMMON_INCLUDE}" )

set(CMAKE_CXX_FLAGS "-O3")

link_libraries(rt)
link_libraries(tcmalloc)

set(SOURCES main.cxx
	${MATCHER_SOURCE_DIR}/common/FileReader.cxx
	${MATCHER_SOURCE_DIR}/common/StatisticsHandler.cxx
	${MATCHER_SOURCE_DIR}/common/Parser.cxx
	${MATCHER_SOURCE_DIR}/common/OrderBook.cxx
)

# executables
add_executable (${PROJECT_NAME}	${SOURCES})
add_custom_target(test COMMAND export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):/usr/local/gcc-4.7.0/lib64 && ./matcher ../../matcher/data/sequence-1.txt)
