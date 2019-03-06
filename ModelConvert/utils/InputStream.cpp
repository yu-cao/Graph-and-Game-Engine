//
// Created by yu cao on 2019-02-27.
//

#include "InputStream.h"

bool InputStream::getBool(std::string message)
{
	bool res = false;
	for(;;)
	{
		std::cout << message << "(y/n)" << std::endl;
		std::string line;
		getline(std::cin, line);
		if(line == "y")
		{
			res = true;
			break;
		}
		else if(line == "n")
		{
			res = false;
			break;
		}
		std::cout << "Input y/n, try again." << std::endl;
	}
	return res;
}

float InputStream::getFloat(std::string message)
{
	float res = 0.0f;
	for(;;)
	{
		std::cout << message << "(float)" << std::endl;
		std::string line;
		getline(std::cin, line);
		try
		{
			res = std::stof(line);
			break;
		}
		catch(const std::exception&)
		{
			std::cout << "you need to input the \"float\", try again." << std::endl;
		}
	}
	return res;
}

std::string InputStream::getString(std::string message)
{
	std::cout << message << "(string)" << std::endl;
	std::string line;
	getline(std::cin, line);
	return line;
}

int InputStream::getInt(std::string message)
{
	int res = 0;
	for(;;)
	{
		std::cout << message << "(int)" << std::endl;
		std::string line;
		getline(std::cin, line);
		try
		{
			res = std::stoi(line);
			break;
		}
		catch (const std::exception&)
		{
			std::cout << "you need to input the \"int\", try again" << std::endl;
		}
	}
	return res;
}
