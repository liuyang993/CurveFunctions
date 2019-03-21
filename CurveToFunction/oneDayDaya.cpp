#include "oneDayDaya.h"
#include "Util.h"

bool OneDayData::CHECK( SQLRETURN rc, char * msg, bool printSucceededMsg, bool quit)
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


void OneDayData::status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line )
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


void OneDayData::initMySQLConnection()
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

OneDayData::OneDayData(int y,int m,int d,std::string sContractName,TBX_CLI_TOOLS_HANDLE handle)
{
	year=y;
	month=m;
	day=d;
	contractName = sContractName;

	std::string sMonth = commonFuctions::toString(m);
	std::string sDay = commonFuctions::toString(d);

	if(sMonth.length()<2)
		sMonth =  sMonth.insert (0,commonFuctions::toString(0));

	if(sDay.length()<2)
		sDay =  sDay.insert (0,commonFuctions::toString(0));


	DBTableName = sContractName.substr(0,2) + commonFuctions::toString(y) + sMonth + sDay;

	originPrice =0;

	hTbxCliTools=handle;

}


void OneDayData::loadData()
{
	//special use for FuPan
	std::tm timeinfo1 = {0};
	//timeinfo1 = std::localtime(&currentTime);
	timeinfo1.tm_year=year-1900; 
	timeinfo1.tm_mon=month-1;
	timeinfo1.tm_mday =day;
	timeinfo1.tm_hour = 9;
	timeinfo1.tm_min = 01;
	timeinfo1.tm_sec = 0;

	sMYSQLQueryTime = commonFuctions::GetStringFromTM(&timeinfo1);


	while(true)
	{
		x.clear();
		y.clear();

		std::string sQuery = "select * from " + DBTableName +  " where happentime<='" + sMYSQLQueryTime + "' order by happentime desc limit 7200";
		SQLCHAR* query = (SQLCHAR*)(sQuery.c_str()) ;

		CHECK( SQLExecDirectA( hStmt, query, SQL_NTS ), "execute query" ) ;
		retCode = SQLFetch( hStmt ) ;

		if(retCode!=0)
		{
			#ifdef TRADETIME
					Sleep(3000);
					continue;
			#else
					commonFuctions::addSecondsToTM(&timeinfo1,3);
					sMYSQLQueryTime = commonFuctions::GetStringFromTM(&timeinfo1);
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

			time_t t;
			double p;

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


				  t = mktime( &tmdate );
				  //x.push_back(t);

			  }
			  if(j==2)
			  {
				  //y.push_back(atof(buf));
				  p=atof(buf);
				  if(originPrice==0)
					  originPrice=p;

			  }



			  if( CHECK( retCode, "SqlGetData", false ) )
			  {
				// Print the data we got.
			  }
			}
			
			//wholedayXY.insert( std::pair<time_t,double>(t,p) );
			wholedayXY.push_back( std::pair<time_t,double>(t,p) );
			

			retCode = SQLFetch( hStmt ) ;
			if( !SQL_SUCCEEDED( retCode ) )
			{
				// SQLFetch FAILS AS IT FETCHES the last row of data.
				break;
			}


		} // end for.
		
		Sleep(1000);
		commonFuctions::addSecondsToTM(&timeinfo1,3);
		sMYSQLQueryTime = commonFuctions::GetStringFromTM(&timeinfo1);

	}  // end while 


}


void OneDayData::judgeBuyOrSell()
{
	//first calculate last period 's average price 


	return;
}