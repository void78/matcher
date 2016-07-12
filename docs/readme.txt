1. Intended Audience.
	Anyone who is reay to review the solution for the problem described in the 'background.txt.

2. Pre-requisites.
	For being able to build and run the application ('matcher' from that point) please make sure you've got an appropriate environment.
	Matcher were tested with the following:
	a. Red Hat Enterprise Linux Server release 6.7 (Santiago)
	b. g++ (GCC) 4.7.0
	c. cmake version 2.8.2
	d. boost_1.59.0
	e. tcmalloc.4.1.0 (optional)
	f. CppUnit 1.12.1 (optional)

3. Steps to run
	Warning: it's not mandatory to use 'build.sh' for building application/tests. Please see https://cmake.org for details

	Feel free to build the solution running 'build.sh' script. If the pre-requisites are met you should get the output like the following:

	[afadeev@algodev02 matcher]$ ./build.sh
	-- The C compiler identification is GNU
	-- The CXX compiler identification is GNU
	-- Check for working C compiler: /usr/local/gcc-4.7.0/bin/gcc
	-- Check for working C compiler: /usr/local/gcc-4.7.0/bin/gcc -- works
	-- Detecting C compiler ABI info
	-- Detecting C compiler ABI info - done
	-- Check for working CXX compiler: /usr/local/gcc-4.7.0/bin/g++
	-- Check for working CXX compiler: /usr/local/gcc-4.7.0/bin/g++ -- works
	-- Detecting CXX compiler ABI info
	-- Detecting CXX compiler ABI info - done
	-- CPPUNIT_INCLUDE = /home/algo/Packages/CppUnit/1.12.1/include
	-- CPPUNIT_LIBRARY = /home/algo/Packages/CppUnit/1.12.1/linux_4.7.0/lib
	-- CMAKE_BINARY_DIR  = /home/AFadeev/projects/matcher/.build
	-- COMMIN_INCLUDE  =
	-- CPPUNIT_INCLUDE = /home/algo/Packages/CppUnit/1.12.1/include
	-- CPPUNIT_LIBRARY = /home/algo/Packages/CppUnit/1.12.1/linux_4.7.0/lib
	-- CMAKE_BINARY_DIR  = /home/AFadeev/projects/matcher/.build
	-- Configuring done
	-- Generating done
	-- Build files have been written to: /home/AFadeev/projects/matcher/.build
	Scanning dependencies of target matcher
	[  7%] Building CXX object matcher/CMakeFiles/matcher.dir/main.cxx.o
	[ 14%] Building CXX object matcher/CMakeFiles/matcher.dir/__/common/FileReader.cxx.o
	[ 21%] Building CXX object matcher/CMakeFiles/matcher.dir/__/common/StatisticsHandler.cxx.o
	[ 28%] Building CXX object matcher/CMakeFiles/matcher.dir/__/common/Parser.cxx.o
	[ 35%] Building CXX object matcher/CMakeFiles/matcher.dir/__/common/OrderBook.cxx.o
	Linking CXX executable matcher
	[ 35%] Built target matcher
	Scanning dependencies of target unit_tests
	[ 42%] Building CXX object tests/CMakeFiles/unit_tests.dir/test.cxx.o
	[ 50%] Building CXX object tests/CMakeFiles/unit_tests.dir/__/common/FileReader.cxx.o
	[ 57%] Building CXX object tests/CMakeFiles/unit_tests.dir/__/common/StatisticsHandler.cxx.o
	[ 64%] Building CXX object tests/CMakeFiles/unit_tests.dir/__/common/Parser.cxx.o
	[ 71%] Building CXX object tests/CMakeFiles/unit_tests.dir/__/common/OrderBook.cxx.o
	[ 78%] Building CXX object tests/CMakeFiles/unit_tests.dir/test01/Test01.cxx.o
	[ 85%] Building CXX object tests/CMakeFiles/unit_tests.dir/test02/Test02.cxx.o
	[ 92%] Building CXX object tests/CMakeFiles/unit_tests.dir/test03/Test03.cxx.o
	[100%] Building CXX object tests/CMakeFiles/unit_tests.dir/test04/Test04.cxx.o
	Linking CXX executable unit_tests
	[100%] Built target unit_tests

	To run unit tests go to '.build'  and do 'make unit'.
	If everything isok You should see no crash/asserts.

	Assuming the soution were build by means of 'build.sh' you can run the following:
	> .build/matcher/matcher matcher/data/testSequence.txt
	It should give you the output:
		Mid quote: nan
		Mid quote: 1037.5
		Mid quote: 1037.5
		Mid quote: 1025
		Mid quote: 1025
		Mid quote: 1012.5
		Mid quote: 1012.5
		Mid quote: 1012.5
		Mid quote: 1012.5
		Mid quote: 1037.5
		T,2,1025 -> 2@1025
		Mid quote: 1037.5
		T,1,1025 -> 3@1025
		Mid quote: 1037.5
		Mid quote: 1012.5
		Mid quote: 1012.5
		Mid quote: 1012.5
		==|====================|====================
		  |              Bid   |   Ask
		==|====================|====================
		1 |....................|1075@1(1)...........
		2 |....................|1050@10(1)..........
		3 |....................|1025@4(1)...........
		4 |..........(2)10@1000|....................
		5 |...........(1)30@975|....................
		--------------------------------------------
		--------------------------------------------
				Summary:
		--------------------------------------------
		+-+ Messages processed: 15
		| +-------+ Orders: 13
		| |       |--- New: 9
		| |       |--- Remove: 3
		| |       |--- Modify: 1
		| |       |--- Duplicated: 0
		| |       |--- Missing: 0
		| |
		| +-------+ Trades: 2
		| |       |--- Normal: 2
		| |       |--- Invalid: 0
		| |
		| +-------+ Corrupted: 0
		|
		|-+ Other statistics:
		  |
		  +--- Min time processing: 267 nsec
		  +--- Max time processing: 8624 nsec
		  +--- Avg time processing: 1675 nsec
		  |
		  +--- Bid depth: 2
		  +--- Bid volume: 40
		  +--- Ask depth: 3
		  +--- Ask volume: 15
		  |
		  +--- Crossed: 1


4. Analysis
	The core part of the solution is OrderBook class which is responsible for order book management and handling incremental messages/orders.
	From the background it's clear that we must meet two requisites: to maintain the sequence of the incrementals and be able to address the order by it's id.
	Not to re-invernt the wheel with std::list and std::set/std::hash boost::multi_index_container were chosen with sequenced and hashed_unique indecis.
	Please see common/OrderBook.h:46.
	(Worth to note that it was possible to use not only hashed_unique index but orderd_unique one. Actually I tried both but did not notice significant differences.)
	boost::multi_index_container not only gives us the same complexity as for STL containers but allowes much easier to support the code by reducing the number of containers.
	Also it give us benefit in memory consumption which in its turn makes the application more CPU cache friendly.
	For more details please see:
		-- http://www.boost.org/doc/libs/1_59_0/libs/multi_index/doc/performance.html
		-- http://www.boost.org/doc/libs/1_59_0/libs/multi_index/doc/reference/index.html
		-- http://www.boost.org/doc/libs/1_59_0/libs/multi_index/doc/reference/hash_indices.html#complexity_signature

			Complexity signature

			Here and in the descriptions of operations of hashed indices,
			we adopt the scheme outlined in the complexity signature section. The complexity signature of hashed indices is:

			copying: c(n)=n*log(n),
			insertion: average case i(n)=1 (amortized constant), worst case i(n)=ndist,
			hinted insertion: average case h(n)=1 (amortized constant), worst case h(n)=ndist,
			deletion: d(n)=1 (constant),
			replacement:
			if the new element key is equivalent to the original, r(n)=1 (constant),
			otherwise, average case r(n)=1 (constant), worst case r(n)=ndist,
			modifying: average case m(n)=1 (constant), worst case m(n)=ndist,
			where ndist is the number of non-equivalent elements out of the total n.

5. C++ code
	Taking into account performance issues I decided to avoid (as much as it's possible) the following:
	1. Exceptions
	2. Interrfaces/virtual functions.

	The second point give us not only the performance benefit. But we can use for statistics collection or parsing any entity wich provide the necessary methods.

6. Testing
	For being able to test not only the sequence provided in the background a python script were created: bin/sequence_generator.py
	Is uses pseudo gandom generator you are able to set up by passing as an argument. The script does not contain the documentation.
	Plese see the code for the details.

	In words it makes the following:
	1. Set up range with prices and 'random' choose the price, quantity and the side.
	2. After that it's either match the order or add it to the book.
	3. The most important thing the sript provides is the order book statistics (sequence-summary-<seed>.txt)
	4. running the script without argument is equivalent running with '1' one.

	In case of building the application with tcmalloc support feel free to run is with heap checking:
		> export HEAPCHECK=normal && .build/matcher/matcher matcher/data/testSequence.txt
		Have memory regions w/o callers: might report false leaks
		No leaks found for check "_main_" (but no 100% guarantee that there aren't any): found 14 reachable heap objects of 666 bytes

	(assuming you current path is matcher's root)

7. Peformance testing.
	With all interim output disabled the following results were received for average message processing:
	1. with TC Malloc disabled: 200 nsec
	2. with TC Malloc enabled: 160 nsec
	Please create the data set by running ./sequence_generator.py without agrument. It's large enough to be added to archive.
	By default it create 1M incrementals with th following summary:
	>Buy: depth 12, total qty 5355707
	>Sell: depth 17, total qty 5382021
	>Trades: 776937

	The statictics collected reflects only the incremental message processing. It does not take into account parsing.

8. Things to be improved
	1. There is a point in the background re trades with no corresponding order. According to the messages provided it is not possible
	   to say if there is a correlation between a trade and order removing. So I have no idea how to do it with 100% guarantee.
	   In real live there is a trade type: OTC or regular trade.
	2. Definitely the parser in the solution is not a production one if we care about the performance.
	   From another side the task is about order matching (at least that's how I understand it). At any rate the pasres is just an entiry you can change
	   without problem: alol you need just to make sure it supports 'parse' method. If it's critical for the test task please let me know so I change it.
	3. It's possible to test different memory allocators (not only tcmalloc) - it might give us some performance benefites.
