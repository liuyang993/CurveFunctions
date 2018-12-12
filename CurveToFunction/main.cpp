#include "OneDayCurve.h"

void main(int argc, char* argv[])
{

	//std::cout << "You have entered " << argc 
 //        << " arguments:" << "\n"; 
 // 
 //   for (int i = 0; i < argc; ++i) 
 //       std::cout << argv[i] << "\n"; 


	OneDayCurve *oc = new OneDayCurve(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),argv[4]);
	//OneDayCurve *oc = new OneDayCurve(2018,12,7,"rb1905");


#ifdef TRADETIME

	oc->connectCTP();

	Sleep(2000);

	oc->login();

	Sleep(2000);

	oc->settlementinfoConfirm();

	Sleep(2000);

	oc->QryAccount();

	oc->initMySQLConnection();
	
	oc->loadData();



	/********************************************
	 * For test quickly order

	 	oc->orderinsert("rb1905","buy",3290);
		oc->currentState=buying;
		oc->BuyingPrice=3290;

	 ********************************************/
	
#else

	#ifdef ONLYTESTAPI
		oc->initMySQLConnection();
	#else
		oc->initMySQLConnection();
		oc->loadData();
	#endif 

#endif

	getchar();
	return;	

}