#ifndef _PERIODSUMMARY_H_
#define _PERIODSUMMARY_H_

#include <time.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <numeric>

class PeriodSummary
{
public:
	time_t startTime;
	time_t endTime;

	double periodSlope;
	double periodHigh;
	double periodLow;

	std::vector<time_t> x;
	std::vector<double> y;



};


#endif
