// The top of every source code file must include this line
#include "sierrachart.h"
#include "MySierraChart.h"

/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/

// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("MyES-2ndEntriesSystem") 

enum EntryStates
{
   eStateInitial,
   eStateCount0,
   eStateCount0Retrace,
   eStateCount1,
   eStateCount1Retrace,
   eStateCount2
};

char arTempStr[256];

inline void EnterLongOrder(SCStudyGraphRef sc)
{
	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_LIMIT;
	NewOrder.Price1 = sc.High[sc.Index-1]+sc.TickSize;

	int orderResult;
	
	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	
	int& lastEntryBar   = sc.PersistVars->i3;
	
	if(PositionData.PositionQuantity == 0 &&
	   sc.Index != lastEntryBar)
	{
		orderResult = sc.BuyEntry(NewOrder);
		if(orderResult > 0)
		{
			lastEntryBar = sc.Index;
			
			// draw arrow
			DrawMarker(sc, sc.Index, sc.Index, sc.Low[sc.Index]-sc.TickSize*8, MARKER_ARROWUP, COLOR_GREEN);
		}
	}

}

inline void EnterShortOrder(SCStudyGraphRef sc)
{
	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_LIMIT;
	NewOrder.Price1 = sc.Low[sc.Index-1]-sc.TickSize;
	
	int orderResult;
	
	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	
	int& lastEntryBar = sc.PersistVars->i3;
	
	if(PositionData.PositionQuantity == 0 &&
	   sc.Index != lastEntryBar)
	{
		orderResult = sc.SellEntry(NewOrder);
		if(orderResult > 0)
		{
			lastEntryBar = sc.Index;
			
			// draw arrow
			DrawMarker(sc, sc.Index, sc.Index, sc.High[sc.Index]+sc.TickSize*8, MARKER_ARROWDOWN, COLOR_RED);
		}
	}

}

//This is the basic framework of a study function.
SCSFExport scsf_MyES_2ndEntriesSystem(SCStudyGraphRef sc)
{
	SCSubgraphRef LongSignal    = sc.Subgraph[0];
	SCSubgraphRef ShortSignal   = sc.Subgraph[1];
    
    SCSubgraphRef Long1stEntry  = sc.Subgraph[2];
    SCSubgraphRef Short1stEntry = sc.Subgraph[3];
		
    SCSubgraphRef AuxSignal = sc.Subgraph[4];
	//SCSubgraphRef MainMA    = sc.Subgraph[5];
	
	SCSubgraphRef FastFilter = sc.Subgraph[6];
	SCSubgraphRef SlowFilter = sc.Subgraph[7];
    	
	int& longEntryState  = sc.PersistVars->i1;
	int& shortEntryState = sc.PersistVars->i2;
	
	int& lastEntryBar   = sc.PersistVars->i3;
	int& longsEnable  = sc.PersistVars->i4;
	int& shortsEnable = sc.PersistVars->i5;
	
	SCInputRef length    = sc.Input[0];
	SCInputRef enabled   = sc.Input[1];
    
	float highest, lowest;
	
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "My ES 2nd Entries System";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		sc.GraphRegion = 0;
		
		//MainMA.Name = "Main MA";
		//MainMA.DrawStyle = DRAWSTYLE_LINE;
		//MainMA.AutoColoring = AUTOCOLOR_SLOPE;
		//MainMA.PrimaryColor = COLOR_BLUE;
		//MainMA.SecondaryColor = COLOR_MAGENTA;
		//MainMA.LineWidth = 2;
		
		LongSignal.Name = "Long Signal";
		LongSignal.DrawStyle = DRAWSTYLE_ARROWUP;
		LongSignal.PrimaryColor = COLOR_AQUAMARINE;
		LongSignal.LineWidth = 2;
		LongSignal.DrawZeros = false;
		
		ShortSignal.Name = "Short Signal";
		ShortSignal.DrawStyle = DRAWSTYLE_ARROWDOWN;
		ShortSignal.PrimaryColor = COLOR_MAGENTA;
		ShortSignal.LineWidth = 2;
		ShortSignal.DrawZeros = false;
        
        Long1stEntry.Name = "Long 1st Entry";
		Long1stEntry.DrawStyle = DRAWSTYLE_TRIANGLEUP;
		Long1stEntry.PrimaryColor = COLOR_AQUAMARINE;
		Long1stEntry.LineWidth = 2;
		Long1stEntry.DrawZeros = false;
        
        Short1stEntry.Name = "Short 1st Entry";
		Short1stEntry.DrawStyle = DRAWSTYLE_TRIANGLEDOWN;
		Short1stEntry.PrimaryColor = COLOR_MAGENTA;
		Short1stEntry.LineWidth = 2;
		Short1stEntry.DrawZeros = false;
		
		AuxSignal.Name = "Aux Signal";
		AuxSignal.DrawStyle = DRAWSTYLE_DIAMOND;
		AuxSignal.PrimaryColor = COLOR_YELLOW;
		AuxSignal.LineWidth = 2;
		AuxSignal.DrawZeros = false;
		
		sc.Input[0].Name = "Count Length";
		sc.Input[0].SetInt(55);
		
		enabled.Name = "Enable Trading";
		enabled.SetYesNo(0); 
        		
		longEntryState   = eStateInitial;
		shortEntryState  = eStateInitial;
		
		
		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 1;
		sc.SupportReversals = 0;
		
		// ** This false by default. Orders will go to the simulation system always. **
		// @@
		sc.SendOrdersToTradeService = true;
			
		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		
		sc.SupportAttachedOrdersForTrading = true;
		// ** this line will causes to not work
		//sc.TradeWindowConfigFileName = "1Contract.twconfig";
		
		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = false;
		
		// This is true by default, only 1 trade can be processed per bar.
		sc.AllowOnlyOneTradePerBar = true; 
		sc.MaintainTradeStatisticsAndTradesData = true;
		
		longsEnable  = 0;
		shortsEnable = 0;
		
		return;
	}
	
	// Setup buttons
	if (sc.MenuEventID != 0)
	{
		if(sc.MenuEventID == SC_ACS_TOOL1)
		{
			if(sc.MouseEventType == SC_ACS_TOOL_ON)
			{
				sc.AddMessageToLog("Longs On", 0);
				longsEnable = 1;
			}
			else if(sc.MouseEventType == SC_ACS_TOOL_OFF)
			{
				sc.AddMessageToLog("Longs Off", 0);
				longsEnable = 0;
			}
		}
		else if(sc.MenuEventID == SC_ACS_TOOL2)
		{
			if(sc.MouseEventType == SC_ACS_TOOL_ON)
			{
				sc.AddMessageToLog("Shorts On", 0);
				shortsEnable = 1;
			}
			else if(sc.MouseEventType == SC_ACS_TOOL_OFF)
			{
				sc.AddMessageToLog("Shorts Off", 0);
				shortsEnable = 0;
			}
		}
		else if(sc.MenuEventID == SC_ACS_TOOL3)
		{
			if(sc.MouseEventType == SC_ACS_TOOL_ON)
			{
				sc.AddMessageToLog("Pausing", 0);
				longsEnable = 0;
				shortsEnable = 0;
			}
		}
	}
	
	// Section 2 - Do data processing here

	//sc.ExponentialMovAvg(sc.Close, MainMA, length.GetInt());
	sc.WeightedMovingAverage(sc.BaseDataIn[SC_LAST], FastFilter, 20);
	sc.WeightedMovingAverage(sc.BaseDataIn[SC_LAST], SlowFilter, 35);
	
	FindMaxMin(sc, length.GetInt(), highest, lowest);
	
	if(sc.BaseData[SC_LAST][sc.Index] > highest ||
	   sc.BaseData[SC_HIGH][sc.Index] > highest)
	{
		longEntryState = eStateCount0;
		
		//@@
		//shortEntryState = eStateCount0;
	}

	if(sc.BaseData[SC_LAST][sc.Index] < lowest ||
	   sc.BaseData[SC_LOW][sc.Index]  < lowest)
	{
		shortEntryState = eStateCount0;
		
		//@@
		//longEntryState = eStateCount0;
	}

	
	switch(longEntryState)
	{
		case eStateCount0:

			if(sc.BaseData[SC_LAST][sc.Index] < sc.Low[sc.Index-1] ||
			   sc.BaseData[SC_LOW][sc.Index]  < sc.Low[sc.Index-1])           
			{
				longEntryState = eStateCount0Retrace;
			}
			break;
			
		case eStateCount0Retrace:
			
			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1])
			{
				longEntryState = eStateCount1;
				
				// 1st entry
                Long1stEntry[sc.Index] = sc.Low[sc.Index]-sc.TickSize*2;
			}
			break;
			
		case eStateCount1:
			
			if(sc.BaseData[SC_LAST][sc.Index] < sc.Low[sc.Index-1] ||
			   sc.BaseData[SC_LOW][sc.Index]  < sc.Low[sc.Index-1]) 
			{
				longEntryState = eStateCount1Retrace;
			}
			break;
			
		case eStateCount1Retrace:

			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1])
			{
				// 2nd entry
				LongSignal[sc.Index] = sc.Low[sc.Index]-sc.TickSize*2;
								
				longEntryState = eStateCount2;	
				
				if(longsEnable && enabled.GetYesNo() && FastFilter[sc.Index] > SlowFilter[sc.Index])
					EnterLongOrder(sc);
			}
			break;
			
		case eStateCount2:
			
			if(sc.BaseData[SC_LAST][sc.Index] < sc.Low[sc.Index-1] ||
			   sc.BaseData[SC_LOW][sc.Index]  < sc.Low[sc.Index-1] )
			{			
				longEntryState = eStateCount1Retrace;
			}
			break;
	}
	
	switch(shortEntryState)
	{
		case eStateCount0:

			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1])           
			{
				shortEntryState = eStateCount0Retrace;
			}
			break;
			
		case eStateCount0Retrace:
			if(sc.BaseData[SC_LAST][sc.Index] < sc.Low[sc.Index-1] ||
			   sc.BaseData[SC_LOW][sc.Index]  < sc.Low[sc.Index-1])
			{
				shortEntryState = eStateCount1;
				
				// 1st entry
                Short1stEntry[sc.Index] = sc.High[sc.Index]+sc.TickSize*2;
			}
			break;
			
   		case eStateCount1:
   			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1]) 
			{
				shortEntryState = eStateCount1Retrace;
			}
   			break;
   			
   		case eStateCount1Retrace:
   			if(sc.BaseData[SC_LAST][sc.Index] < sc.Low[sc.Index-1] ||
			   sc.BaseData[SC_LOW][sc.Index]  < sc.Low[sc.Index-1])
			{
				// 2nd entry
				ShortSignal[sc.Index] = sc.High[sc.Index]+sc.TickSize*2;
				
				shortEntryState = eStateCount2;	
				
				if(shortsEnable && enabled.GetYesNo() && FastFilter[sc.Index] < SlowFilter[sc.Index])
					EnterShortOrder(sc);
			}
   			break;
   			
   		case eStateCount2:
   			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1] )
			{			
				//state5[sc.Index] = sc.Low[sc.Index]-sc.TickSize*11;
				shortEntryState = eStateCount1Retrace;
			}
   			break;
	}

	
	if(longEntryState == eStateCount1Retrace)
	{
		sprintf(arTempStr, "L arming");
	}
	else
	{
		sprintf(arTempStr, " ");
	}
	DrawText(sc, 9898, 4, 96, COLOR_GREEN, RGB(0,0,0), arTempStr);
	
	if(shortEntryState == eStateCount1Retrace)
	{
		sprintf(arTempStr, "S arming");
	}
	else
	{
		sprintf(arTempStr, " ");
	}
	DrawText(sc, 9899, 4, 92, COLOR_RED, RGB(0,0,0), arTempStr);
	
	if(enabled.GetYesNo() && longsEnable)
	{
		sprintf(arTempStr, "Long Enable");
		DrawText(sc, 9897, 4, 88, COLOR_GREEN, RGB(0,0,0), arTempStr);
	}
	else if(enabled.GetYesNo() && shortsEnable)
	{
		sprintf(arTempStr, "Short Enable");
		DrawText(sc, 9897, 4, 88, COLOR_RED, RGB(0,0,0), arTempStr);
	}
	else
	{
		sprintf(arTempStr, " ");
		DrawText(sc, 9897, 4, 88, COLOR_RED, RGB(0,0,0), arTempStr);
	}
	
}
