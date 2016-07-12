#include <common/FileReader.h>
#include <common/Parser.h>
#include <common/StatisticsHandler.h>
#include <common/FeedHandler.h>

typedef matcher::common::FileReader FileReader;
typedef matcher::common::StatisticsHandler StatisticsHandler;
typedef matcher::common::Parser Parser;
typedef matcher::common::FeedHandler<FileReader, StatisticsHandler, Parser> FeedHandler;

int main(int argc, char * argv[])
{
	std::string fileName;

	if(argc < 2) {
		fileName = "sequence.txt";
	}
	else
		fileName = argv[1];


	try {
		FileReader fileReader(fileName);
		StatisticsHandler statisticsHandler;
		FeedHandler feedHandler(fileReader, statisticsHandler);

		feedHandler.exec(true);

		statisticsHandler.print(std::cout);
	}
	catch(std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Undefined exception" << std::endl;
	}

	return 0;
}

