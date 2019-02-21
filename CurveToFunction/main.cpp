#include "OneDayCurve.h"
#include "OneDayDaya.h"

void main(int argc, char* argv[])
{
	//TBX_CLI_TOOLS_HANDLE			hTbxCliTools;
	// /********************************************
	// * init cli console window
	// ********************************************/

	///*TBX_CLI_TOOLS_HANDLE			hTbxCliTools;*/

	//TBX_CLI_TOOLS_INIT_PARAMS	CliToolsParams;

	///* Prepare the CLI Tools parameters */
	//memset( &CliToolsParams, 0, sizeof(CliToolsParams) );
	//CliToolsParams.un32MaxScreenWidth		= 256;
	//CliToolsParams.un32MaxScreenHeight		= 150;
	//CliToolsParams.un32MinScreenWidth		= 80;
	//CliToolsParams.un32MinScreenHeight		= 24;

	//CliToolsParams.un32MaxPromptLines		= 1;
	//CliToolsParams.un32LogBufferMaxLines    = 10000;
	//CliToolsParams.fLowPrioryThread			= TBX_TRUE;



	//CliToolsParams.un32DefaultScreenWidth	= 80;
	//CliToolsParams.un32DefaultScreenHeight	= 24;

	//CliToolsParams.fDisplayLog				= TBX_TRUE;
	//CliToolsParams.un32MaxRefreshDelay		= 250;
	//CliToolsParams.un32MinClsDelay			= 10;
	//CliToolsParams.fFlushLogOnlyOnError	= TBX_FALSE;

	//strcpy( CliToolsParams.szLogFileName, "tbtoolpack_service" );
	//CliToolsParams.un32LogBufferMaxLines	= 1000;
	//CliToolsParams.un32MaxLogFileSize		= 1024*1024;
	//CliToolsParams.un32MinRefreshDelay		= 50;
	//CliToolsParams.fDisableTerminalInput	= TBX_FALSE;
	//CliToolsParams.fDisableTerminalOutput	= TBX_FALSE;


	//	/* Display Menu handler */
	////CliToolsParams.pFctDisplayMenu			= TBCAFCliDisplayMenu;
	////CliToolsParams.pCtxDisplayMenu			= (PTBX_VOID)hTbxCliTools;

	///* Menu choice handler */
	////CliToolsParams.pFctHandleMenuChoice	= TbxCliToolsDefaultKeysHandler;
	////CliToolsParams.pCtxHandleMenuChoice	= (PTBX_VOID)hTbxCliTools;

	///* Input validation handler */
	////CliToolsParams.pFctValidateUserInput	= TBCAFCliValidateUserInput;
	////CliToolsParams.pCtxValidateUserInput	= (PTBX_VOID)hTbxCliTools;

	///* User Input apply handler */
	////CliToolsParams.pFctApplyUserInput		= TBCAFCliApplyUserInput;
	////CliToolsParams.pCtxApplyUserInput		= (PTBX_VOID)hTbxCliTools;




	//TBX_RESULT Result = TbxCliToolsInit( &CliToolsParams, &hTbxCliTools );

	///* Launch the command-line thread */
	//Result = TbxCliToolsStart( hTbxCliTools );





	//std::cout << "You have entered " << argc 
 //        << " arguments:" << "\n"; 
 // 
 //   for (int i = 0; i < argc; ++i) 
 //       std::cout << argv[i] << "\n"; 


	OneDayCurve *oc = new OneDayCurve(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),argv[4]);
	oc->initMySQLConnection();
	oc->loadData();
	//OneDayCurve *oc = new OneDayCurve(2018,12,7,"rb1905");

	//OneDayData *od = new OneDayData(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),argv[4],hTbxCliTools);
	//od->initMySQLConnection();
	//od->loadData();



//#ifdef TRADETIME
//
//	oc->connectCTP();
//
//	Sleep(2000);
//
//	oc->login();
//
//	Sleep(2000);
//
//	oc->settlementinfoConfirm();
//
//	Sleep(2000);
//
//	oc->QryAccount();
//
//	oc->initMySQLConnection();
//	
//	oc->loadData();
//
//
//
//	/********************************************
//	 * For test quickly order
//
//	 	oc->orderinsert("rb1905","buy",3290);
//		oc->currentState=buying;
//		oc->BuyingPrice=3290;
//
//	 ********************************************/
//	
//#else
//
//	#ifdef ONLYTESTAPI
//		oc->initMySQLConnection();
//	#else
//		oc->initMySQLConnection();
//		oc->loadData();
//	#endif 
//
//#endif

	getchar();
	return;	

}