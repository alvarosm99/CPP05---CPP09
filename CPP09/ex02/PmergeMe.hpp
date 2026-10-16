#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <climits>
#include <utility>
#include <algorithm>
#include <sys/time.h>
#include <stdexcept>

class PmergeMe
{
	public:
		PmergeMe();
		PmergeMe(std::vector<int> vec, std::deque<int> deq);
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);
		~PmergeMe();

		double getVtime();
		double getDtime();

		void printUseq();
		void printSseq();

		void mergePairs(std::vector<std::pair<int, int> > &left, std::vector<std::pair<int, int> > &right, std::vector<std::pair<int, int> > &result);
		void mergePairs(std::deque<std::pair<int, int> > &left, std::deque<std::pair<int, int> > &right, std::deque<std::pair<int, int> > &result); 
		void recursiveSortPairs(std::vector<std::pair<int, int> > &pairs);
		void recursiveSortPairs(std::deque<std::pair<int, int> > &pairs);
		std::vector<int> sortVec();
		std::deque<int> sortDeq();

	private:
		std::vector<int> _uvec;
		std::vector<int> _svec;
		double _vtime;
		std::deque<int> _udeq;
		std::deque<int> _sdeq;
		double _dtime;
};

#endif