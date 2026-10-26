#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <string>
#include <iostream>
#include <stdexcept>
#include <map>
#include <cstdlib>
#include <fstream>

class BitcoinExchange
{
	public:
		BitcoinExchange();
		BitcoinExchange(const BitcoinExchange& other);
		BitcoinExchange &operator=(const BitcoinExchange& other);
		~BitcoinExchange();

		const std::map<std::string, float>& getDataBase() const;

		void setYear(const std::string &year);
		void setMonth(const std::string &year);
		void setDay(const std::string &year);

		void checkFile(const char* filePath);
		void fileNotEmpty(const std::string& filePath);
		void fileFormat(const std::string& filePath);
		bool checkDate(std::string& date);
		bool checkLeapYear(const std::string &year, const std::string &month, const std::string &day);
		bool checkPrice(double price);
		void loadDataBase(const std::string& dbPath);
		void printDataBase() const;

	private:
		std::map<std::string, float> _database;
		std::string _year;
		std::string _month;
		std::string _day;
};

#endif