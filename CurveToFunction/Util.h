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



};




#endif
