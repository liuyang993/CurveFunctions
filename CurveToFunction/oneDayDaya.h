#ifndef _ONEDAYDATA_H_
#define _ONEDAYDATA_H_

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <time.h>
#include <ctime>
#include <numeric>


#include <Windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#pragma comment( lib, "odbc32.lib" )

#include "tbx_cli.h"
#include "tbx_cli_tools.h"



enum CurveState
{
	BrownMoTion=1,

	FirstRaise,
	FirstRaiseRollback,
	SecondRaise,
	SecondRaiseRollback,
	ThirdRaise,
	ThirdRaiseRollback,

	FirstDrop,
	FirstDropRollback,
	SecondDropRaise,
	SecondDropRaiseRollback,
	ThirdDropRaise,
	ThirdDropRaiseRollback

};


class OneDayData
{
public:
	int year;
	int month;
	int day;

	std::string contractName;
	std::string DBTableName;


	double lastdaySettle;
	double todayOpen;

	double todayHigh;
	double todayLow;

	double currentOneMinSlope;

	double originPrice;

	//std::map<time_t,double> wholedayXY;

	std::vector<std::pair<time_t,double>> wholedayXY;


	time_t currentTime;
	std::string sMYSQLQueryTime;   //mysql 查询语句用的相对时间

	SQLHANDLE hEnv ;
	SQLRETURN retCode ;
	SQLHANDLE hConn ;
	SQLHANDLE hStmt ;

	std::vector<time_t> x;
	std::vector<double> y;


public:
	OneDayData(int y,int m,int d,std::string sContractName,TBX_CLI_TOOLS_HANDLE handle);   //set DBTableName

	void loadData();   //query data into vector base on DBTableName

	void judgeBuyOrSell(); 

	bool CHECK( SQLRETURN rc, char * msg, bool printSucceededMsg=false, bool quit=true );

	void status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line );

	void initMySQLConnection();

	//double slope(const std::vector<time_t>& xaxis, const std::vector<double>& yaxis);

public:   

	TBX_CLI_TOOLS_HANDLE			hTbxCliTools;
};













#endif 

