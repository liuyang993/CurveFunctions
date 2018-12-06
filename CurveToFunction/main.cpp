#include "OneDayCurve.h"

void main(int argc, char* argv[])
{

	OneDayCurve *oc = new OneDayCurve(2018,12,4,"rb1901");

	oc->connect();

	Sleep(2000);

	oc->login();

	Sleep(2000);

	oc->settlementinfoConfirm();

	Sleep(2000);

	oc->QryAccount();


	oc->initMySQLConnection();
	oc->loadData();

	getchar();
	return;	

}