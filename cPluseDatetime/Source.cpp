#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <sstream>

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);


    std::cout << "hour=" << tstruct.tm_hour << std::endl;



    std::cout << "year=" << tstruct.tm_year+1900 << std::endl;
    std::cout << "month=" << tstruct.tm_mon+1 << std::endl;
    std::cout << "day=" << tstruct.tm_mday << std::endl;



    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}



const std::string getStringFromYMD() {
	int year = 2019;
	int month =  3;
	int day = 26; 

	std::stringstream s;
	s<<year;
	if(month<10)
	{
		s<<"0";
		s<<month;
	}
	else
	{
		s<<month;
	}
	if(day<10)
	{
		s<<"0";
		s<<day;
	}
	else
	{
		s<<day;
	}

	

	std::string s1 = s.str();



	return s1;
}

int main() {
    std::cout << "currentDateTime()=" << currentDateTime() << std::endl;
    
	
	std::string s1= getStringFromYMD();

	
	
	getchar();  // wait for keyboard input
}