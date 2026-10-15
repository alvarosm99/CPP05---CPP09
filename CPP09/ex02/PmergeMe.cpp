#include "PmergeMe.hpp"

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(std::vector<int> vec, std::deque<int> deq) : _uvec(vec), _udeq(deq) 
{
    struct timeval start, end;

    gettimeofday(&start, NULL);
    _svec = sortVec();
    gettimeofday(&end, NULL);
    
    _vtime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    gettimeofday(&start, NULL);
    _sdeq = sortDeq();
    gettimeofday(&end, NULL);
    
    _dtime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
}

PmergeMe::PmergeMe(const PmergeMe &other)
{
	_uvec = other._uvec;
	_svec = other._svec;
	_vtime = other._vtime;
	_udeq = other._udeq;
	_sdeq = other._sdeq;
	_dtime = other._dtime;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this != &other)
	{
		_uvec = other._uvec;
		_svec = other._svec;
		_vtime = other._vtime;
		_udeq = other._udeq;
		_sdeq = other._sdeq;
		_dtime = other._dtime;
	}
	return *this;
}

PmergeMe::~PmergeMe() {}




double PmergeMe::getVtime()
{
	return _vtime;
}

double PmergeMe::getDtime()
{
	return _dtime;
}





void PmergeMe::printUseq()
{
    for (size_t i = 0; i < _uvec.size(); i++)
    {
        std::cout << _uvec[i];
        if (i + 1 < _uvec.size())
            std::cout << " ";
    }
    std::cout << std::endl;
}

void PmergeMe::printSseq()
{
    for (size_t i = 0; i < _svec.size(); i++)
    {
        std::cout << _svec[i];
        if (i + 1 < _svec.size())
            std::cout << " ";
    }
    std::cout << std::endl;
}





void PmergeMe::mergePairs(std::vector<std::pair<int, int> > &left, std::vector<std::pair<int, int> > &right, std::vector<std::pair<int, int> > &result) 
{
    size_t i = 0;
    size_t j = 0;
    result.clear();

    while (i < left.size() && j < right.size()) 
    {
        if (left[i].first < right[j].first) 
        {
            result.push_back(left[i]);
            i++;
        } 
        else 
        {
            result.push_back(right[j]);
            j++;
        }
    }

    while (i < left.size()) 
    {
        result.push_back(left[i]);
        i++;
    }

    while (j < right.size()) 
    {
        result.push_back(right[j]);
        j++;
    }
}

void PmergeMe::recursiveSortPairs(std::vector<std::pair<int, int> > &pairs)
{
    if (pairs.size() <= 1)
        return;

    size_t mid = pairs.size() / 2;
    std::vector< std::pair<int, int> > left(pairs.begin(), pairs.begin() + mid);
    std::vector< std::pair<int, int> > right(pairs.begin() + mid, pairs.end());

    recursiveSortPairs(left);
    recursiveSortPairs(right);

    mergePairs(left, right, pairs);
}

std::vector<int> PmergeMe::sortVec()
{
	std::vector< std::pair<int, int> > pairs;
	std::vector<int> extra;

	for (size_t i = 0; i < _uvec.size(); i += 2)
	{
		if (i + 1 < _uvec.size())
		{
			int a = _uvec[i];
			int b = _uvec[i + 1];
			
			if (a > b)
				pairs.push_back(std::make_pair(a, b));
			else
				pairs.push_back(std::make_pair(b, a));
		}
		else
		{
			extra.push_back(_uvec[i]);
		}
	}

	recursiveSortPairs(pairs);

	std::vector<int> main_chain;
    std::vector<int> pendants;

    for (size_t i = 0; i < pairs.size(); i++)
    {
        main_chain.push_back(pairs[i].first);
        pendants.push_back(pairs[i].second);
    }

    if (pendants.empty())
    {
        if (!extra.empty())
			main_chain.push_back(extra[0]);
        return main_chain;
    }

    main_chain.insert(main_chain.begin(), pendants[0]);

    size_t prev_j = 1;
    size_t curr_j = 3;

    while (prev_j < pendants.size())
    {
        size_t limit = curr_j;
        if (limit > pendants.size())
            limit = pendants.size();

        for (size_t i = limit; i > prev_j; i--)
        {
            int val = pendants[i - 1];
            
            std::vector<int>::iterator it = std::lower_bound(main_chain.begin(), main_chain.end(), val);
            main_chain.insert(it, val);
        }

        size_t next_j = curr_j + 2 * prev_j;
        prev_j = curr_j;
        curr_j = next_j;
    }

    if (!extra.empty())
    {
        std::vector<int>::iterator it = std::lower_bound(main_chain.begin(), main_chain.end(), extra[0]);
        main_chain.insert(it, extra[0]);
    }

    return main_chain;
}





void PmergeMe::mergePairs(std::deque<std::pair<int, int> > &left, std::deque<std::pair<int, int> > &right, std::deque<std::pair<int, int> > &result) 
{
    size_t i = 0;
    size_t j = 0;
    result.clear();

    while (i < left.size() && j < right.size()) 
    {
        if (left[i].first < right[j].first) 
        {
            result.push_back(left[i]);
            i++;
        } 
        else 
        {
            result.push_back(right[j]);
            j++;
        }
    }

    while (i < left.size()) 
    {
        result.push_back(left[i]);
        i++;
    }

    while (j < right.size()) 
    {
        result.push_back(right[j]);
        j++;
    }
}

void PmergeMe::recursiveSortPairs(std::deque<std::pair<int, int> > &pairs)
{
    if (pairs.size() <= 1)
        return;

    size_t mid = pairs.size() / 2;
    std::deque< std::pair<int, int> > left(pairs.begin(), pairs.begin() + mid);
    std::deque< std::pair<int, int> > right(pairs.begin() + mid, pairs.end());

    recursiveSortPairs(left);
    recursiveSortPairs(right);

    mergePairs(left, right, pairs);
}

std::deque<int> PmergeMe::sortDeq()
{
	std::deque< std::pair<int, int> > pairs;
	std::deque<int> extra;

	for (size_t i = 0; i < _udeq.size(); i += 2)
	{
		if (i + 1 < _udeq.size())
		{
			int a = _udeq[i];
			int b = _udeq[i + 1];
			
			if (a > b)
				pairs.push_back(std::make_pair(a, b));
			else
				pairs.push_back(std::make_pair(b, a));
		}
		else
		{
			extra.push_back(_udeq[i]);
		}
	}

	recursiveSortPairs(pairs);

	std::deque<int> main_chain;
    std::deque<int> pendants;

    for (size_t i = 0; i < pairs.size(); i++)
    {
        main_chain.push_back(pairs[i].first);
        pendants.push_back(pairs[i].second);
    }

    if (pendants.empty())
    {
        if (!extra.empty())
			main_chain.push_back(extra[0]);
        return main_chain;
    }

    main_chain.insert(main_chain.begin(), pendants[0]);

    size_t prev_j = 1;
    size_t curr_j = 3;

    while (prev_j < pendants.size())
    {
        size_t limit = curr_j;
        if (limit > pendants.size())
            limit = pendants.size();

        for (size_t i = limit; i > prev_j; i--)
        {
            int val = pendants[i - 1];
            
            std::deque<int>::iterator it = std::lower_bound(main_chain.begin(), main_chain.end(), val);
            main_chain.insert(it, val);
        }

        size_t next_j = curr_j + 2 * prev_j;
        prev_j = curr_j;
        curr_j = next_j;
    }

    if (!extra.empty())
    {
        std::deque<int>::iterator it = std::lower_bound(main_chain.begin(), main_chain.end(), extra[0]);
        main_chain.insert(it, extra[0]);
    }

    return main_chain;
}