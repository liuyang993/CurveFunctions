#ifndef _ONEDAY_H_
#define _ONEDAY_H_

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <numeric>
#include <time.h>
#include <ctime>

#include <Windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "ThostFtdcMdApi.h"              //CTP head file
#include "ThostFtdcTraderApi.h"


#include "tbx_cli.h"
#include "tbx_cli_tools.h"


#pragma comment( lib, "odbc32.lib" )


enum OpenWay
{
	HighOpen=1,
	LowOpen
};

enum DealState
{
	none=1,
	buying,
	buyed,
	selling,
	selled,
};

class OneDayCurve:public CThostFtdcTraderSpi
{

private:
	CThostFtdcTraderApi *m_ptraderapi;

public:
	int year;
	int month;
	int day;

	std::string contractName;
	std::string DBTableName;


	std::vector<std::map<time_t,float>> wholedayXY;

	OpenWay todayOpenWay;
	DealState currentState;

	time_t currentTime;
	std::string sCurrentTime;
	std::string sLatesetTime;



	std::list<std::string> noticableThings; 

	double currentOneMinuteSlope;
	double Last1TimeSlope;
	double Last2TimeSlope;

	float todayHighPoint;
	float todayLowPoint;
	float currentPoint;
	float todayOpen;
	float yesterdaySettle;



	bool startLoadData;


	SQLHANDLE hEnv ;
	SQLRETURN retCode ;
	SQLHANDLE hConn ;
	SQLHANDLE hStmt ;

	std::vector<time_t> x;
	std::vector<double> y;


public:
	OneDayCurve(int y,int m,int d,std::string sContractName);   //set DBTableName

	void loadData();   //query data into vector base on DBTableName

	bool CHECK( SQLRETURN rc, char * msg, bool printSucceededMsg=false, bool quit=true );

	void status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line );

	void initMySQLConnection();

	double slope(const std::vector<time_t>& xaxis, const std::vector<double>& yaxis);

	void judgeBuyOrSell();


public:
	void connect();
	void release();
	void login();
	void settlementinfoConfirm();
	void orderinsert(const char* HeYunName,const char* BuyOrSell,int iPrice);
	void qryInstrument();
	void OnFrontConnected();
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	void OnRtnTrade(CThostFtdcTradeField *pTrade);
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void QryAccount();
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
	int OrderRef;
	int nRequestID;

	bool BuyingState;
	bool SellingState;
	double BuyingPrice;
	double SellingPrice;
	double StopLoseBuyPrice;
	double StopLoseSellPrice;

	int dealRefNumber;

public:    //about timer 
	//PTP_POOL pool;
	//PTP_TIMER_CALLBACK timercallback;
	//TP_CALLBACK_ENVIRON CallBackEnviron;
	//PTP_CLEANUP_GROUP cleanupgroup;

	//VOID CALLBACK  MyTimerCallback(
	//	PTP_CALLBACK_INSTANCE Instance,
	//	PVOID                 Parameter,
	//	PTP_TIMER             Timer
 //   );

	TBX_CLI_TOOLS_HANDLE			hTbxCliTools;

};

#endif
