#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() {};

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
    _database = other._database;
    _year = other._year;
    _month = other._month;
    _day = other._day;
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
	{
        _database = other._database;
        _year = other._year;
        _month = other._month;
        _day = other._day;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}



const std::map<std::string, float>& BitcoinExchange::getDataBase() const
{
    return _database;
}

void BitcoinExchange::setYear(const std::string &year)
{
	_year = year;
}

void BitcoinExchange::setMonth(const std::string &month)
{
	_month = month;
}

void BitcoinExchange::setDay(const std::string &day)
{
	_day = day;
}



void BitcoinExchange::printDataBase() const
{
    for (std::map<std::string, float>::const_iterator it = _database.begin();
         it != _database.end(); ++it)
    {
        std::cout << it->first << " => " << it->second << std::endl;
    }
}

void BitcoinExchange::fileExtension(const std::string& filePath, std::string ext)
{
	if (filePath.size() <= ext.size())
    {
        std::string error = "[ERROR]: The file name '" + filePath + "' is too short.";
		throw std::runtime_error(error);
    }
}

void BitcoinExchange::fileNotEmpty(const std::string& filePath)
{
	std::ifstream file(filePath.c_str());

	if (!file.is_open())
	{
		std::string error = "[ERROR]: '" + filePath + "' doesn't exist or is not accessible.";
		throw std::runtime_error(error);
	}

	char c;

	if (!file.get(c))
	{
		std::string error = "[ERROR]: The file '" + filePath + "' is empty.";
		throw std::runtime_error(error);
	}
	
	file.close();
}

bool BitcoinExchange::checkLeapYear(const std::string &year, const std::string &month, const std::string &day)
{
	int n_year = std::atoi(year.c_str());
	bool isLeap = (n_year % 4 == 0 && n_year % 100 != 0) || (n_year % 400 == 0);
	
	if (month == "02" && day == "29")
   		return isLeap;

	return true;
}

bool BitcoinExchange::checkDate(std::string& date)
{
	if (date.size() != 10)
        return false;

	if (date[4] != '-' || date[7] != '-')
        return false;

	std::string year;
	std::string month;
	std::string day;

	for (size_t i = 0; i < date.size(); i++)
    {
		if (i < 4)
			year.push_back(date[i]);

		if (i > 4 && i < 7)
			month.push_back(date[i]);

		if (i > 7)
			day.push_back(date[i]);

        if (i == 4 || i == 7)
            continue;

        if (!std::isdigit(date[i]))
            return false;
    }

	if (date[5] > '1' || (date[5] == '1' && date[6] > '2') || (date[5] == '0' && date[6] == '0') || (date[8] == '0' && date[9] == '0') || (date[8] > '3') || (date[8] == '3' && date[9] > '1') || (date[5] == '0' && date[6] == '2' && date[8] > '2'))
		return false;

	if (date[5] == '0' && (date[6] == '4' || date[6] == '6' || date[6] == '9') && date[8] == '3' && date[9] == '1')
		return false;

	if (date[5] == '1' && date[6] == '1' && date[8] == '3' && date[9] == '1')
		return false;

	setYear(year);
	setMonth(month);
	setDay(day);

	if (!checkLeapYear(year, month, day))
	{
		std::string error = "[ERROR]: Invalid date.";
		return false;
	}

	return true;
}

bool BitcoinExchange::checkPrice(double price)
{
	if (price < 0)
	{
		std::cerr << "[ERROR]: not a positive number." << std::endl; 
		return false;
	}	
	if (price > 1000)
	{
		std::cerr << "[ERROR]: too large a number." << std::endl; 
		return false;
	}
	return true;
}

void BitcoinExchange::fileFormat(const std::string& filePath)
{
	std::ifstream userDB(filePath.c_str());
	std::string line;

	while (std::getline(userDB, line))
	{
		if (line == "date | value")
		{
			if (!std::getline(userDB, line))
			{
				std::cerr << "[WARNING]: The file is empty." << std::endl;
				return;
			}
		}

		size_t separator = line.find(" | ");

		if (separator == std::string::npos)
		{
			std::cerr << "[ERROR]: bad input => " << line << std::endl;
			continue;
		}

		std::string dateString = line.substr(0, separator);

		if (!checkDate(dateString))
		{
			std::cerr << "[ERROR]: bad input => " << dateString << std::endl;
			continue;
		}

		std::string priceString = line.substr(separator + 3);
        char* endPtr;
        double price = std::strtod(priceString.c_str(), &endPtr);

		if (*endPtr != '\0' && *endPtr != '\n')
		{
			std::cerr << "[ERROR]: bad input => " << priceString << std::endl;
			continue;
		}

		if (!checkPrice(price))
		{
			continue;
		}
		
		std::map<std::string, float>::iterator it = this->_database.lower_bound(dateString);

        if (it == this->_database.begin() && it->first != dateString)
		{
            std::cerr << "[ERROR]: Date is too early for database." << std::endl;
            continue;
        }

        if (it == this->_database.end() || it->first != dateString)
		{
            --it; 
        }

        std::cout << dateString << " => " << price << " = " << (price * it->second) << std::endl;
	}
}

void BitcoinExchange::checkFile(const char* filePath)
{
	std::string fileExt = filePath;
	size_t dot = fileExt.find_last_of('.');

	// I do this because if a '.' is not found the variable dot gets the value of std::string::npos which is the largest size_t as possible
	if (dot == std::string::npos)
		dot = 0;

	fileExt = fileExt.substr(dot, fileExt.size());

	fileExtension(filePath, fileExt);
	fileNotEmpty(filePath);
	fileFormat(filePath);
}

void BitcoinExchange::loadDataBase(const std::string& dbPath)
{
	std::ifstream db(dbPath.c_str());
	if (!db.good())
	{
		std::string error = "[ERROR]: The data base failed to open.";
		throw std::runtime_error(error);
	}
	std::string line;
	size_t comma;
	std::string dateString;
	std::string priceString;
	float price;

	std::getline(db, line);
	while (std::getline(db, line))
	{
		comma = line.find(',');
		if (comma == std::string::npos)
		{
			std::string error = "[ERROR]: The file '" + dbPath + "' has an invalid format.";
			throw std::runtime_error(error);
		}
		dateString = line.substr(0, comma);
		priceString = line.substr(comma + 1);
		price = atof(priceString.c_str());
		this->_database[dateString] = price;
	}
}