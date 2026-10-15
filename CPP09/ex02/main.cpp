#include "PmergeMe.hpp"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "[ERROR]: The number of arguments are invalid." << std::endl;
		return 1;
	}
	
	std::vector<int> vec;
	std::deque<int> deq;

	try
	{
		for (int i = 1; i < argc; i++)
		{
			std::string str = argv[i];

			for (size_t j = 0; j < str.length(); j++)
			{
				if (!std::isdigit(str[j]))
				{
					std::string error = "[ERROR]: Invalid character found. Only positive integers allowed.";
					throw std::runtime_error(error);
				}
			}

			char* end = NULL;
			long val = std::strtol(argv[i], &end, 10);
			if (*end != '\0' || val <= 0 || val > INT_MAX)
			{
				throw std::runtime_error("[ERROR]: Number out of range.");
			}

			vec.push_back(static_cast<int>(val));
			deq.push_back(static_cast<int>(val));
		}
	}
	
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	PmergeMe sequence(vec, deq);

	std::cout << "Unsorted Sequence: ";
	sequence.printUseq();
	std::cout << "Sorted Sequence: ";
	sequence.printSseq();
	std::cout << "Time to process a range of " << vec.size() << " elements with std::vector : " << sequence.getVtime() << " us" << std::endl;
	std::cout << "Time to process a range of " << deq.size() << " elements with std::deque : " << sequence.getDtime() << " us" << std::endl;

	return 0;	
}