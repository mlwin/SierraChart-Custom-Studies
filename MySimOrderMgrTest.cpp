// MySimOrderMgrTest.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include "MySimOrderMgr.h"

int main(int argc, char* argv[])
{
	MySimOrderMgr::TradePosition trade1, trade2, trade3, trade4, trade5;


	trade1.m_EntryPrice = 10;
	MySimOrderMgr::Instance()->EnterLong(trade1);

	trade2.m_EntryPrice = 11;
	MySimOrderMgr::Instance()->EnterLong(trade2);

	trade3.m_EntryPrice = 13;
	MySimOrderMgr::Instance()->EnterLong(trade3);


	MySimOrderMgr::Instance()->Process(5);
	MySimOrderMgr::Instance()->Process(27);

	trade4.m_EntryPrice = 20;
	MySimOrderMgr::Instance()->EnterLong(trade4);

	MySimOrderMgr::Instance()->Process(30);
	MySimOrderMgr::Instance()->Process(1);


	getchar();

	return 0;
}

