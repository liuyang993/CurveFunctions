#include <iostream>
#include <vector>
#include <list>
#include <map>

#include <time.h>
#include <ctime>



void main()
{
	time_t currentTime;
	
	std::tm* timeinfo;
	std::time(&currentTime);
    timeinfo = std::localtime(&currentTime);

	std::pair <time_t,double> product1 (currentTime,2.30);  
	std::pair <time_t,double> product2 (currentTime,2.30);
	std::pair <time_t,double> product3 (currentTime,2.40);  

	//std::vector<std::map<time_t,double>> wholedayXY;
	std::map<time_t,double> wholedayXY;


	wholedayXY.insert(product1);
	wholedayXY.insert(product2);
	wholedayXY.insert(product3);


	//map key do not allow duplicate; 


	return;


}