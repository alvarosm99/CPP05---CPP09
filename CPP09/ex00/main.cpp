#include "BitcoinExchange.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "[ERROR]: The number of arguments is invalid." << std::endl;
		return 1;
	}

	BitcoinExchange be;

	try
	{
		be.loadDataBase("./data.csv");
		be.checkFile(argv[1]);
	}
	catch(const std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	
	/* Print the database 
	be.printDataBase();
	*/

	return 0;
}