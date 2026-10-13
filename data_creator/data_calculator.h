#pragma once
#include <list>

class DataCalculator
{
public:
	DataCalculator();
	double calculate_std_dev(std::list<double>& data);
	double calculate_average(std::list<double>& data);
};

