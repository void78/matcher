cmake_minimum_required(VERSION 2.8)

project(unit_tests)


include_directories( ${CPPUNIT_INCLUDE} )
include_directories (${BOOST_INCLUDE})
include_directories( ${MATCHER_SOURCE_DIR}/common )

message( STATUS "CPPUNIT_INCLUDE = ${CPPUNIT_INCLUDE}" )
message( STATUS "CPPUNIT_LIBRARY = ${CPPUNIT_LIBRARY}" )
message( STATUS "CMAKE_BINARY_DIR  = ${CMAKE_BINARY_DIR}" )

link_libraries(dl)
link_libraries(cppunit)
link_libraries(boost_filesystem)
link_libraries(tcmalloc)



set(SOURCES test.cxx
	${MATCHER_SOURCE_DIR}/common/FileReader.cxx
	${MATCHER_SOURCE_DIR}/common/StatisticsHandler.cxx
	${MATCHER_SOURCE_DIR}/common/Parser.cxx
	${MATCHER_SOURCE_DIR}/common/OrderBook.cxx
)

add_subdirectory(test01)
add_subdirectory(test02)
add_subdirectory(test03)
add_subdirectory(test04)

# executables
add_executable (${PROJECT_NAME}	${SOURCES})

add_custom_target(unit COMMAND export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):/usr/local/gcc-4.7.0/lib64 && ./unit_tests DEPENDS unit_tests)
