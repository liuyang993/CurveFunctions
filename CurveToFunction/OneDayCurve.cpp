#include "OneDayCurve.h"
#include "Util.h"
#include "Timer.h"


OneDayCurve::OneDayCurve(int y,int m,int d,std::string sContractName)
{
	currentOneMinuteSlope=0;
	Last1TimeSlope=0;
	Last2TimeSlope=0;

	OrderRef=1;     //  ref id 
	nRequestID=1;
	BuyingState=false;
	SellingState=false;

    BuyingPrice=0;
	SellingPrice=0;
    BuyedPrice=0;
	SelledPrice=0;
	StopLoseBuyPrice=0;
	StopLoseSellPrice=0;

	dProfitFlyingStartSlope=0;

	currentState=none;
	dealRefNumber = 0;

	ifOrdDealed = false;


	year=y;
	month=m;
	day=d;
	contractName = sContractName;

	startLoadData = true;
	//currentTime = time(0);

	//tm* localtm = localtime(&currentTime);
	std::tm* timeinfo;

    std::time(&currentTime);
    timeinfo = std::localtime(&currentTime);

	sCurrentTime = commonFuctions::GetStringFromTM(timeinfo);

#ifdef TRADETIME
	if(timeinfo->tm_hour>12)
	{
		timeinfo->tm_hour = 15;
		timeinfo->tm_min = 33;

		sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo);
	}

	if(timeinfo->tm_hour<=12)
	{
		timeinfo->tm_hour = 11;
		timeinfo->tm_min = 33;

		sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo);
	}
#else
	//timeinfo->tm_year=y-1900;
	//timeinfo->tm_mon=m-1;
	//timeinfo->tm_mday =d;
	//timeinfo->tm_hour = 13;
	//timeinfo->tm_min = 31;
	//timeinfo->tm_sec = 0;

	//sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo);

#endif

	// for rb only 
	//if(timeinfo->tm_hour)

	//timeinfo->tm_hour = 23;
	//timeinfo->tm_min = 33;

	//std::strftime(buffer,80,"%Y-%m-%d-%H:%M:%S",timeinfo);
 //   std::puts(buffer);
	//sLatesetTime = buffer;

	std::string sMonth = commonFuctions::toString(m);
	std::string sDay = commonFuctions::toString(d);

	if(sMonth.length()<2)
		sMonth =  sMonth.insert (0,commonFuctions::toString(0));

	if(sDay.length()<2)
		sDay =  sDay.insert (0,commonFuctions::toString(0));


	DBTableName = sContractName.substr(0,2) + commonFuctions::toString(y) + sMonth + sDay;

	//char cTest [10] = {"aaaabbbb"};
	//std::string s111 = commonFuctions::toString(cTest);


	 /********************************************
	 * init cli console window
	 ********************************************/

	/*TBX_CLI_TOOLS_HANDLE			hTbxCliTools;*/

	TBX_CLI_TOOLS_INIT_PARAMS	CliToolsParams;

	/* Prepare the CLI Tools parameters */
	memset( &CliToolsParams, 0, sizeof(CliToolsParams) );
	CliToolsParams.un32MaxScreenWidth		= 256;
	CliToolsParams.un32MaxScreenHeight		= 150;
	CliToolsParams.un32MinScreenWidth		= 80;
	CliToolsParams.un32MinScreenHeight		= 24;

	CliToolsParams.un32MaxPromptLines		= 1;
	CliToolsParams.un32LogBufferMaxLines    = 10000;
	CliToolsParams.fLowPrioryThread			= TBX_TRUE;


	
	CliToolsParams.un32DefaultScreenWidth	= 80;
	CliToolsParams.un32DefaultScreenHeight	= 24;

	CliToolsParams.fDisplayLog				= TBX_TRUE;
	CliToolsParams.un32MaxRefreshDelay		= 250;
	CliToolsParams.un32MinClsDelay			= 10;
	CliToolsParams.fFlushLogOnlyOnError	= TBX_FALSE;

	strcpy( CliToolsParams.szLogFileName, "tbtoolpack_service" );
	CliToolsParams.un32LogBufferMaxLines	= 1000;
	CliToolsParams.un32MaxLogFileSize		= 1024*1024;
	CliToolsParams.un32MinRefreshDelay		= 50;
	CliToolsParams.fDisableTerminalInput	= TBX_FALSE;
	CliToolsParams.fDisableTerminalOutput	= TBX_FALSE;


	//CliToolsParams.fprint
	//fPrintAllowed


	/* Menu choice handler */
	//CliToolsParams.pFctHandleMenuChoice	= HandleChoice;
	//CliToolsParams.pCtxHandleMenuChoice	= NULL;


	

	TBX_RESULT Result = TbxCliToolsInit( &CliToolsParams, &hTbxCliTools );

	/* Launch the command-line thread */
	Result = TbxCliToolsStart( hTbxCliTools );

	//TbxCliToolsLogPrint
	//(
	//	hTbxCliTools,
	//	TRACE_LEVEL_3, FGREEN,
	//	"class member start timer \n"
	//);


	//TbxCliToolsPrint
	//(
	//	hTbxCliTools,
	//	"(%c) Help    (%c) Quit",
	//	'?',
	//	'q'
	//);

	//BOOL bRet = FALSE;
 //   PTP_TIMER timer = NULL;
 //   pool = NULL;
 //   PTP_TIMER_CALLBACK timercallback = MyTimerCallback;
 //   TP_CALLBACK_ENVIRON CallBackEnviron;
 //   PTP_CLEANUP_GROUP cleanupgroup = NULL;
 //   FILETIME FileDueTime;
 //   ULARGE_INTEGER ulDueTime;
 //   UINT rollback = 0;
}


bool OneDayCurve::CHECK( SQLRETURN rc, char * msg, bool printSucceededMsg, bool quit)
{
  if( SQL_SUCCEEDED( rc ) )
  {
    if( printSucceededMsg )  printf( "%s succeeded\n", msg ) ;
    
    return true ;
  }
  else
  {
    printf( "NO!!!  %s has FAILED!!\n", msg ) ;
    
    if( quit )  FatalAppExitA( 0, msg ) ;

    return false ;
  }
}


void OneDayCurve::status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line )
{
  SQLCHAR sqlState[6];
  SQLINTEGER nativeError;
  SQLCHAR msgStr[256];
  SQLSMALLINT overBy ; // the number of characters that msgStr buffer was TOO SHORT..
  
  // http://msdn.microsoft.com/en-us/library/ms716256(VS.85).aspx
  // This must be the WEIRDEST ERROR REPORTING FUNCTION I've EVER seen.
  // It requires 8 parameters, and its actually pretty .. silly
  // about the amount of state information it expects YOU to keep track of.

  // It isn't so much a "GetLastError()" function
  // as it is a "GetMeStatus( something very, very specific )" function.
  
  SQLRETURN retCode ;
  
  for( int i = 1 ; i < 20 ; i++ )
  {
    retCode = SQLGetDiagRecA(
      
      handleType,  // the type of object you're checking the status of
      theHandle,   // handle to the actual object you want the status of
      
      i, // WHICH status message you want.  The "Comments" section at the 
      // bottom of http://msdn.microsoft.com/en-us/library/ms716256(VS.85).aspx
      // seems to explain this part well.

      sqlState,    // OUT:  gives back 5 characters (the HY*** style error code)
      &nativeError,// numerical error number
      msgStr,      // buffer to store the DESCRIPTION OF THE ERROR.
      // This is the MOST important one, I suppose

      255,         // the number of characters in msgStr, so that
      // the function doesn't do a buffer overrun in case it
      // has A LOT to tell you
      &overBy      // again in case the function has A LOT to tell you,
      // the 255 character size buffer we passed might not be large
      // enough to hold the entire error message.  If that happens
      // the error message will truncate and the 'overBy' variable
      // will have a value > 0 (it will measure number of characters
      // that you 'missed seeing' in the error message).
      
    ) ;

    if( CHECK( retCode, "SQLGetDiagRecA" ) )
    {
      printf( "LINE %d:  [%s][%d] %s\n", line, sqlState, nativeError, msgStr ) ;
    }
    else
    {
      // Stop looping when retCode comes back
      // as a failure, because it means there are
      // no more messages to tell you
      break ;
    }
  }
}


void OneDayCurve::initMySQLConnection()
{
	// 1.  Create a handle for the environment.

  
	retCode = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv ) ;
  
	CHECK( retCode, "allocate environment handle" ) ;
  

	// 2.  Next, set the version of ODBC to use to ODBC version 3.
	// Format of this command is a bit weird, we cast the value we're passing
	// to (void*) because the function requires it, but then we say that the
	// length of the "string" we've passed in is 0 characters long, so I assume
	// that means SQLSetEnvAttr should know to interpret the "pointer value" that
	// we passed in as actually an integer value (which is what it is).
	retCode = SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 ) ; 

	CHECK( retCode, "setting the environment attribute setting to ODBC version 3" ) ;


	// 3.  Allocate the connection handle.  Note this doesn't
	// connect us to the database YET.  We're still "ALLOCATING",
	// whatever that means :) (Hey i know what allocating is,
	// but this is an awful number of steps to follow if you
	// ask me, microsoft!!  Whatever happened to a simple init()
	// function?


	CHECK( SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hConn ), "allocate handle" ) ;

  
  

	// HOOK IT UP!!  Actually connect to the database.
	SQLCHAR* dsnName = (SQLCHAR*)"SHFuture" ;  // MUST BE THE SAME
	// as the name of the ODBC data source you set
	// in the Microsoft ODBC Administrator window.

	SQLCHAR* userid = (SQLCHAR*)"root";
	SQLCHAR* password = (SQLCHAR*)"MYSQLTB";  // using a BLANK
	// Above are my own correct userid and password credentials to
	// be used when logging into the MySQL database server.

	// 4.  Open database connection.
	retCode = SQLConnectA(
    
	hConn,
    
	dsnName,  // name of data source we are connecting to,
	// AS PER REGISTERED IN ODBC Data Source Administrator.

	// If you are on a 64-bit machine, and you
	// DO NOT USE THE 64-bit driver.  As long as
	// your compiler publishes a 32-bit .exe (which
	// Visual Studio does), you'll keep getting:

	// [Microsoft][ODBC Driver Manager] Data source name not found and no default driver specified

	// SO DON'T USE THE 64-BIT DRIVERS!  Instead, install
	// the 32-bit driver, and then managing/using
	// your 32-bit datasources in
	// c:\windows\syswow64\odbcad32.exe

	// Note that on a 64-bit windows machine, the 32-bit
	// drivers and the 64-bit drivers are managed
	// from COMPLETELY SEPARATE, BUT IDENTICAL-LOOKING
	// windows.  Its really weird.

	// On a 64-bit machine:
	// c:\windows\system32\odbcad32.exe    // 64-bit version [even though it SAYS system32_ in the path, this is the __64__ bit version on a 64-bit machine]
	// c:\windows\syswow64\odbcad32.exe    // 32-bit version [even though it SAYS syswow64_ in the path]

	// Call it stupid, scream, pull your hair out,
	// that's what it is.
	// http://stackoverflow.com/questions/949959/why-do-64bit-dlls-go-to-system32-and-32bit-dlls-to-syswow64-on-64bit-windows

	// and

	// http://blogs.sepago.de/helge/2008/04/20/windows-x64-all-the-same-yet-very-different-part-7/

	// Thanks again, Microsoft,
	// for making the 64-bit programming experience
	// such a pleasure.
	SQL_NTS,  // the DSN name is a NULL TERMINATED STRING, so "count it yourself"

	userid,
	SQL_NTS,  // userid is a null-terminated string
    
	password,
	SQL_NTS   // password is a null terminated string
    
	) ;
	if( !CHECK( retCode, "SqlConnectA", false ) )
	{
	// if this fails, I want that extra status
	// information about WHY the failure happened.
	// status function is defined above.
    
	status( SQL_HANDLE_DBC, hConn, __LINE__ ) ;
	}


	// 6.  Create and allocate a statement
	CHECK( SQLAllocHandle( SQL_HANDLE_STMT, hConn, &hStmt ), "allocate handle for statement" ) ;
}


void OneDayCurve::loadData()
{

	//special use for FuPan
	std::tm* timeinfo1;
	timeinfo1 = std::localtime(&currentTime);
	timeinfo1->tm_year=year-1900; 
	timeinfo1->tm_mon=month-1;
	timeinfo1->tm_mday =day;
	timeinfo1->tm_hour = 15;
	timeinfo1->tm_min = 31;
	//timeinfo1->tm_hour = 9;
	//timeinfo1->tm_min = 30;
	timeinfo1->tm_sec = 0;

	sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo1);


	while(startLoadData)
	{
		x.clear();
		y.clear();

		//SQLCHAR* query = (SQLCHAR*)"select happentime,lastprice from rb20181201 where happentime<='2018-12-1 23:30:30' and hour(happentime)>=21  order by happentime desc limit 120" ;


#ifdef TRADETIME
		//std::string sQuery = "select * from rb20181204 where happentime<='" + sMYSQLQueryTime + "' order by happentime desc limit 120";

		std::string sQuery = "select * from " + DBTableName +  " where happentime<='" + sMYSQLQueryTime + "' order by happentime desc limit 120";
		SQLCHAR* query = (SQLCHAR*)(sQuery.c_str()) ;

#else
		std::string sQuery = "select * from if1904_20190321 where happentime<='" + sMYSQLQueryTime + "' order by happentime desc limit 120";
		SQLCHAR* query = (SQLCHAR*)(sQuery.c_str()) ;

#endif
		//SQLCHAR* query = (SQLCHAR*)"select * from rb20181207 where happentime<='2018-12-7 15:33:00' order by happentime desc limit 120" ;
	
		CHECK( SQLExecDirectA( hStmt, query, SQL_NTS ), "execute query" ) ;

		retCode = SQLFetch( hStmt ) ;

		if(retCode!=0)
		{
			#ifdef TRADETIME
					Sleep(3000);
					continue;
			#else
					commonFuctions::addSecondsToTM(timeinfo1,3);
					sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo1);
					continue;

			#endif

		}

		CHECK( retCode, "first sqlFetch" ) ;


		SQLLEN numRows ;
		retCode = SQLRowCount( hStmt, &numRows ) ;

		for( int i = 1 ; i <= numRows ; i++ )
		{
			// Datatypes
			// SQLGetData

			char buf[256];
			SQLINTEGER numBytes ;

			for( int j = 1 ;   // column counter starts at __1__, not 0.
			  j <= 2 ;   // numCols retrieved above
			  j++ )
			{
			  retCode = SQLGetData(
      
				hStmt,
				j,           // COLUMN NUMBER of the data to get
				SQL_C_CHAR,  // the data type that you expect to receive
				buf,         // the place to put the data that you expect to receive
				255,         // the size in bytes of buf (-1 for null terminator)
				&numBytes    // size in bytes of data returned
      
			  ) ;


			  if(j==1)
			  {
				  struct tm tmdate = {0};
				  tmdate.tm_year = atoi(&buf[0]) - 1900;
				  tmdate.tm_mon = atoi(&buf[5]);
				  tmdate.tm_mday = atoi(&buf[8]);
				  tmdate.tm_hour = atoi(&buf[11]);
				  tmdate.tm_min = atoi(&buf[14]);
				  tmdate.tm_sec = atoi(&buf[17]);
				  


				  time_t t = mktime( &tmdate );
				  x.push_back(t);

			  }
			  if(j==2)
			  {
				  y.push_back(atof(buf));

			  }



			  if( CHECK( retCode, "SqlGetData", false ) )
			  {
				// Print the data we got.
			  }
			}
   

			retCode = SQLFetch( hStmt ) ;
			if( !SQL_SUCCEEDED( retCode ) )
			{
				// SQLFetch FAILS AS IT FETCHES the last row of data.
				break;
			}


		} // end for.


		//update last 3 time slope 
		Last2TimeSlope=Last1TimeSlope;
		Last1TimeSlope = currentOneMinuteSlope;
		currentOneMinuteSlope = slope(sMYSQLQueryTime,x,y);


		//goto judgement logic 
		judgeBuyOrSell();


#ifdef TRADETIME
		Sleep(3000);
#else
		Sleep(1000);
		commonFuctions::addSecondsToTM(timeinfo1,1);
		sMYSQLQueryTime = commonFuctions::GetStringFromTM(timeinfo1);

#endif

	}


	SQLFreeHandle( SQL_HANDLE_STMT, hStmt ) ;
	SQLFreeHandle( SQL_HANDLE_DBC, hConn ) ;
	SQLFreeHandle( SQL_HANDLE_ENV, hEnv ) ;


}

void OneDayCurve::onTimerout()
{

	TbxCliToolsLogPrint
	(
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"timeout event happen ,class member function incoke by thread \n"
	);

	if(!ifOrdDealed)
	{
		// order still not deal ,  cancel the order and re-order 

		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"previous order still not deal \n"
		);
		MyReqOrderAction();


	   TbxCliToolsLogPrint
	   (
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"order canceled , now re - order \n"
	   );

	   if(currentState == buying)
	   {
		   orderinsert(contractName.c_str(),"buy",(int)BuyingPrice+1);
		   BuyingPrice=BuyingPrice+1;
	   }
	   else if(currentState == selling)
	   {
		   orderinsert(contractName.c_str(),"buy",(int)SellingPrice-1);
		   SellingPrice=SellingPrice-1;
	   }


	}
	else
	{
		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"previous order already deal \n"
		);

	}



	return;
}


double OneDayCurve::slope(const std::string sTime, std::vector<time_t>& xaxis, const std::vector<double>& yaxis)
{
	//if(x.size() != y.size()){
 //       throw exception("...");
 //   }
    double n = xaxis.size();

    double avgX = accumulate(xaxis.begin(), xaxis.end(), 0.0) / n;
    double avgY = accumulate(y.begin(), y.end(), 0.0) / n;

    double numerator = 0.0;
    double denominator = 0.0;

    for(int i=0; i<n; ++i){
        numerator += (xaxis[i] - avgX) * (y[i] - avgY);
        denominator += (xaxis[i] - avgX) * (xaxis[i] - avgX);
    }

    //if(denominator == 0){
    //    throw exception("...");
    //}

	double dResult = numerator / denominator;


	TbxCliToolsLogPrint
	(
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"at %s ,last 1 min slope is %f , and new is %6.2f \n" ,sTime.c_str(), dResult,yaxis[0]
	);

    return dResult;

}


void OneDayCurve::judgeBuyOrSell()
{
	if((Last1TimeSlope==0)||(Last2TimeSlope==0)||(currentOneMinuteSlope==0))
		return;

	if((currentState == buying)&&(y[0]>BuyingPrice+5))
	{
		MyReqOrderAction();
	
	}

	//orderinsert(contractName.c_str(),"buy",3360); 


	if((currentOneMinuteSlope>Last1TimeSlope)&&(Last1TimeSlope>Last2TimeSlope)&&(Last1TimeSlope!=0)&&(Last2TimeSlope!=0)&&(currentState==none))
	{
		//start raise , should buy
		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"now slope :%f,last time slope :%f,last 2 time slope :%f,plan buy at %6.2f \n" , currentOneMinuteSlope,Last1TimeSlope,Last2TimeSlope,y[0]+1
		);

#ifdef TRADETIME
		orderinsert(contractName.c_str(),"buy",y[0]+1);     //y[0] is newest price 
		currentState = buying;
		BuyingPrice = y[0]+1;

#else
		currentState = buyed;
		BuyedPrice = y[0]+1;

#endif
		//and in orderinsert function , will record dealRefNumber 
	}

	if((BuyedPrice+5<y[0])&&(currentState==buyed))     // over 5 poing profit 
	{

		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"TradeCurve: Last time buy alreay have 5 point profit now , try to let profit fly \n "
		);

		dProfitFlyingStartSlope = currentOneMinuteSlope;
		dProfitFlyingStartprice=y[0];


		////重置斜率 避免刚止盈还买的情况

	 //   Last1TimeSlope=0;
		//Last2TimeSlope=0;
		//currentOneMinuteSlope=0;

		
#ifdef TRADETIME
		//orderinsert(contractName.c_str(),"closebuy",y[0]-1);     //y[0] is newest price 

		currentState =  profitFlying;
#else
		currentState =  profitFlying;
#endif

	}
	 

	if((currentOneMinuteSlope>dProfitFlyingStartSlope)&&(currentState==profitFlying))
	{
		dProfitFlyingStartSlope=currentOneMinuteSlope;
		dProfitFlyingStartprice = y[0];

	}

	if((currentOneMinuteSlope<dProfitFlyingStartSlope)&&(y[0]<dProfitFlyingStartprice)&&(currentState==profitFlying))
	{
		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"TradeCurve: during profit flying state ,1 min slope begin down,close it \n "
		);

		//止盈
		
		//重置斜率 避免刚止盈还买的情况
	    Last1TimeSlope=0;
		Last2TimeSlope=0;
		currentOneMinuteSlope=0;

		

#ifdef TRADETIME
		//orderinsert(contractName.c_str(),"closebuy",y[0]-1);     //y[0] is newest price 

		currentState =  none;
#else
		currentState =  none;
#endif

	}


	if((BuyedPrice-5 > y[0])&&(currentState==buyed))     // over 5 poing lose 
	{

		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"TradeCurve: Last time buy alreay have 5 point lose now , will close it \n "
		);

#ifdef TRADETIME
		
		orderinsert(contractName.c_str(),"closebuy",y[0]-1);     //y[0] is newest price 
		currentState =  none;
#else
		currentState =  none;
#endif
	}



	//sell part 
	if((currentOneMinuteSlope<Last1TimeSlope)&&(Last1TimeSlope<Last2TimeSlope)&&(Last1TimeSlope!=0)&&(Last2TimeSlope!=0)&&(currentState==none))
	{
		//start raise , should buy
		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"now slope :%f,last time slope :%f,last 2 time slope :%f,plan sell at %6.2f \n" , currentOneMinuteSlope,Last1TimeSlope,Last2TimeSlope,y[0]-1
		);

#ifdef TRADETIME
		orderinsert(contractName.c_str(),"sell",y[0]-1);     //y[0] is newest price 
		currentState = selling;
		SellingPrice = y[0]-1;
#else
		currentState = selled;
		SelledPrice = y[0]-1;
#endif


		//and in orderinsert function , will record dealRefNumber 
	}


	if((SelledPrice>y[0]+5)&&(currentState==selled))     // over 5 poing profit 
	{

		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"TradeCurve: Last time sell alreay have 5 point profit now , will close it \n "
		);

		//重置斜率 避免刚止盈还买的情况

	    Last1TimeSlope=0;
		Last2TimeSlope=0;
		currentOneMinuteSlope=0;


#ifdef TRADETIME		
		orderinsert(contractName.c_str(),"closesell",y[0]+1);     //y[0] is newest price 

		currentState =  none;
#else
		currentState =  none;
#endif

	}


	if((SelledPrice+5 < y[0])&&(currentState==selled))     // over 5 poing lose 
	{

		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"TradeCurve: Last time sell alreay have 5 point lose now , will close it \n "
		);
		
#ifdef TRADETIME	
		orderinsert(contractName.c_str(),"closesell",y[0]+1);     //y[0] is newest price 
		currentState =  none;
#else
		currentState =  none;
#endif

	}




	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void OneDayCurve::connectCTP()
{

    //创建API实例

    m_ptraderapi = CThostFtdcTraderApi::CreateFtdcTraderApi("/flow"); //必须提前创建好flow目录

    m_ptraderapi->RegisterSpi(this);

    m_ptraderapi->SubscribePublicTopic(THOST_TERT_QUICK);

    m_ptraderapi->SubscribePrivateTopic(THOST_TERT_QUICK); //设置私有流订阅模式

    //m_ptraderapi->RegisterFront("tcp://127.0.0.1:41205");
	  

#ifdef ONLYTESTAPI
	m_ptraderapi->RegisterFront("tcp://180.168.146.187:10030");     //非交易时间只测试API连这个 

#else
	m_ptraderapi->RegisterFront("tcp://180.168.146.187:10000");     //交易时间连这个 

#endif 

	  

    m_ptraderapi->Init();

    //输出API版本信息



				TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"CTP API %s\n", m_ptraderapi->GetApiVersion()
		);

  }

 

  //释放

void OneDayCurve::release()
{

    m_ptraderapi->Release();

}

 

  //登陆

void OneDayCurve::login()
{

    CThostFtdcReqUserLoginField t = {0};

    strcpy_s(t.BrokerID, "9999");    // in SimSnow  ,  BrokerID统一为：9999

    strcpy_s(t.UserID, "131888");

    strcpy_s(t.Password, "Piercing77");

    while (m_ptraderapi->ReqUserLogin(&t, 1)!=0)   Sleep(1000);

  }

 

  //结算单确认

void OneDayCurve::settlementinfoConfirm()
{

    CThostFtdcSettlementInfoConfirmField t = {0};

    strcpy_s(t.BrokerID, "9999");

    strcpy_s(t.InvestorID, "131888");

    while (m_ptraderapi->ReqSettlementInfoConfirm(&t, 2)!=0)  Sleep(1000);

}

 

  //报单
void OneDayCurve::orderinsert(const char* HeYunName,const char* BuyOrSell,int iPrice)
{

    CThostFtdcInputOrderField t = { 0 };

    strcpy_s(t.BrokerID, "9999");

    strcpy_s(t.InvestorID, "131888");

    strcpy_s(t.UserID, "131888");

	if((strcmp (BuyOrSell,"buy")==0)||(strcmp (BuyOrSell,"closesell")==0))
	{
	t.Direction = THOST_FTDC_D_Buy;
	}

	if((strcmp (BuyOrSell,"sell")==0)||(strcmp (BuyOrSell,"closebuy")==0))
	{
	t.Direction = THOST_FTDC_D_Sell;
	}

	if((strcmp (BuyOrSell,"buy")==0)||(strcmp (BuyOrSell,"sell")==0))
	{
		t.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}

	if((strcmp (BuyOrSell,"closesell")==0)||(strcmp (BuyOrSell,"closebuy")==0))
	{
		t.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}

    //t.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

    t.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;

    t.ContingentCondition = THOST_FTDC_CC_Immediately;

    strcpy_s(t.InstrumentID, HeYunName);

    t.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

    t.LimitPrice = iPrice;

    t.OrderPriceType = THOST_FTDC_OPT_LimitPrice;

    t.VolumeCondition = THOST_FTDC_VC_AV;

    t.TimeCondition = THOST_FTDC_TC_GFD;

    t.VolumeTotalOriginal = 1;

	OrderRef++;

	dealRefNumber = OrderRef;

	_itoa_s(OrderRef, t.OrderRef, 10);//报单引用


    //strcpy_s(t.OrderRef, "0000001");

    while (m_ptraderapi->ReqOrderInsert(&t, nRequestID++) != 0)
	{
		Sleep(500);

	}

	TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"success ordered %s at price %d, now start timer \n",BuyOrSell,iPrice
   );

	timer::Timer* serverTimer = new timer::Timer([this]{onTimerout();},200);
	serverTimer->start();
	


}


void OneDayCurve::MyReqOrderAction()   //撤单
{
	CThostFtdcInputOrderActionField a = { 0 };

	strcpy_s(a.BrokerID, "9999");

	strcpy_s(a.InvestorID, "131888");

	strcpy_s(a.UserID, "131888");

	strcpy_s(a.OrderSysID,sOrderSysID.c_str());  //对应要撤报单的OrderSysID

	strcpy_s(a.ExchangeID, "SHFE");

	strcpy_s(a.InstrumentID, "rb1905");
	

	a.ActionFlag = THOST_FTDC_AF_Delete;

	while (m_ptraderapi->ReqOrderAction(&a, nRequestID++) != 0) 
	{
		TbxCliToolsLogPrint
		(
			hTbxCliTools,
			TRACE_LEVEL_3, FGREEN,
			"CTP:cancle order not success \n"
		);
		
		Sleep(500);

	}


}
 

  //查询合约
void OneDayCurve::qryInstrument()
{

    CThostFtdcQryInstrumentField t = { 0 };

    strcpy_s(t.ExchangeID, "SHFE");

    strcpy_s(t.InstrumentID, "zn1803");

    while (m_ptraderapi->ReqQryInstrument(&t, 4) != 0) Sleep(1000);

}

 

//前置连接响应
void OneDayCurve::OnFrontConnected()
{

   TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnFrontConnected \n"
   );


}

 

  //登陆请求响应
void OneDayCurve::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


   TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspUserLogin \n"
   );
}

 

  //结算单确认响应
void OneDayCurve::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

   TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspSettlementInfoConfirm \n"
   );
}

 

  //查询合约响应
void OneDayCurve::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

   TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspQryInstrument \n"
   );
}

 

  //报单通知
void OneDayCurve::OnRtnOrder(CThostFtdcOrderField *pOrder)
{

	TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRtnOrder \n"
   );
   
	sOrderSysID = commonFuctions::toString(pOrder->OrderSysID);

}

  

  //成交通知
void OneDayCurve::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if(atoi(pTrade->OrderRef)!=dealRefNumber)    // maybe manual deal 
		return;

	ifOrdDealed = true;

	switch (currentState)
	{
	case none:
		break;
	case buying:
		currentState=buyed;
		BuyedPrice = pTrade->Price;
		break;
	case buyed:
		break;
	case selling:
		currentState=selled;
		SelledPrice = pTrade->Price;
		break;
	case selled:
		break;
	default:
		break;
	}
	
	


		TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRtnTrade \n"
   );

}

 

  //报单录入请求响应
void OneDayCurve::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

		TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspOrderInsert \n"
   );
}

  

  //错误应答
void OneDayCurve::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


  		TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspError \n"
   );


}


  //query account money 
void OneDayCurve::QryAccount()
{
    CThostFtdcQryTradingAccountField a = { 0 };

	strcpy_s(a.BrokerID, "9999");

	strcpy_s(a.InvestorID, "131888");

	strcpy_s(a.CurrencyID, "CNY");

	m_ptraderapi->ReqQryTradingAccount(&a, nRequestID++);

	return;
  }

void OneDayCurve::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


	  
  		TbxCliToolsLogPrint
   (
		hTbxCliTools,
		TRACE_LEVEL_3, FGREEN,
		"CTP:OnRspQryTradingAccount \n"
   );

}
