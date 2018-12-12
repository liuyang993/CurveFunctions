#ifndef _UTIL_H_
#define _UTIL_H_

#include <sstream>
#include <ctime>

class commonFuctions
{
public:
	template<typename T>
	static std::string toString(const T& value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

	inline std::string date_timestamp(time_t rawtime)
	{
		//time_t rawtime;
		//std::time(&rawtime);
		struct tm *tinfo = std::localtime(&rawtime);
		char buffer[12];
		strftime(buffer, 12, "%F", tinfo);
		return std::string(buffer);
	}

	static std::string GetStringFromTM(std::tm* t)
	{
		char buffer [80];
		std::strftime(buffer,80,"%Y-%m-%d %H:%M:%S",t);
		std::puts(buffer);
		std::string sRtn = buffer;
		return sRtn;
	
	}

	static void addSecondsToTM(std::tm* t, int isec)
	{
		if(t->tm_sec+isec<60)
		{
			t->tm_sec=t->tm_sec+isec;
			return;
		}

		if(t->tm_sec+isec>=60)
		{
			t->tm_sec=t->tm_sec+isec-60;
			if(t->tm_min+1<60)
				t->tm_min=t->tm_min+1;
			else
			{
				t->tm_min=t->tm_min+1-60;
				t->tm_hour=t->tm_hour+1;
			}

		
			return;

		}
	
		return;
	}


};




#endif
