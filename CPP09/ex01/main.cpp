#include "RPN.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "[ERROR]: The number of arguments is invalid." << std::endl;
        return 1;
    }

    try
    {
        RPN arg(argv[1]);
        std::cout << arg.getResult() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}