#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <stdexcept>

class RPN
{
	public:
		RPN();
		RPN(const std::string &xpr);
		RPN(const RPN &other);
		RPN &operator=(const RPN &other);
		~RPN();

		std::string getXpr();
		int getResult();

		int parseStk(const std::string &str);

	private:
		std::string _xpr;
		int _result;
};

#endif