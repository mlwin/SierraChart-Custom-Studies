// ************************************************************************************
//
// This code is provided on an "AS IS" basis, without warranty of any kind,
// including without limitation the warranties of merchantability, fitness for a
// particular purpose and non-infringement.
//
// Copyright (c) 2014
// mlwin1@yahoo.com
//
// ************************************************************************************


// The top of every source code file must include this line
#include "sierrachart.h"
#include "MySierraChart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

using namespace std;

// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("My Retrace2Smoother System") 

//This is the basic framework of a study function.
SCSFExport scsf_MyRetrace2SmootherSystem(SCStudyGraphRef sc)
{
    SCSubgraphRef TouchMA      = sc.Subgraph[0];
	SCSubgraphRef FastMA       = sc.Subgraph[1];
	SCSubgraphRef SlowMA       = sc.Subgraph[2];
	SCSubgraphRef SR_MA        = sc.Subgraph[3];
    SCSubgraphRef LongSignal   = sc.Subgraph[4];
	SCSubgraphRef ShortSignal  = sc.Subgraph[5];
	
    SCInputRef PeriodTouchMA = sc.Input[0];
	SCInputRef PeriodFastMA  = sc.Input[1];
	SCInputRef PeriodSlowMA  = sc.Input[2];
	SCInputRef PeriodSR_MA   = sc.Input[3];
	SCInputRef enabled       = sc.Input[4];

	
	int filterStartTime = HMS_TIME(9, 05, 0);
	int filterEndTime   = HMS_TIME(14, 55, 0);
	int barTime = sc.BaseDateTimeIn.TimeAt(sc.Index);
	
	int& longsEnable  = sc.PersistVars->i1;
	int& shortsEnable = sc.PersistVars->i2;
    int& lastEntryBar = sc.PersistVars->i3;
	
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "My Retrace To Smoother System";
	
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;
		sc.AutoLoop = 1;  //Auto looping is enabled. 
		sc.GraphRegion = 0;
		
        TouchMA.Name         = "Touch EMA";
		TouchMA.DrawStyle    = DRAWSTYLE_LINE;
		TouchMA.PrimaryColor = COLOR_WHITE;
		PeriodTouchMA.Name   = "Touch EMA Period";
		PeriodTouchMA.SetInt(5);
        
		FastMA.Name = "Fast EMA";
		FastMA.DrawStyle = DRAWSTYLE_LINE;
		FastMA.PrimaryColor = COLOR_YELLOW;
		PeriodFastMA.Name = "Fast EMA Period";
		PeriodFastMA.SetInt(20);
		
		SlowMA.Name = "Slow EMA";
		SlowMA.DrawStyle = DRAWSTYLE_LINE;
		SlowMA.PrimaryColor = COLOR_BLUE;
		SlowMA.SecondaryColor = COLOR_MAGENTA;
		PeriodSlowMA.Name = "Slow EMA Period";
		PeriodSlowMA.SetInt(20);
		
		SR_MA.Name = "SR MA";
		SR_MA.DrawStyle = DRAWSTYLE_LINE;
		SR_MA.LineStyle = LINESTYLE_DASH;
		SR_MA.AutoColoring = AUTOCOLOR_SLOPE;
		SR_MA.PrimaryColor = COLOR_BLUE;
		SR_MA.SecondaryColor = COLOR_MAGENTA;
		PeriodSR_MA.Name = "Support/Resistance MA Period";
		PeriodSR_MA.SetInt(40);
        
        LongSignal.Name = "Long Signal";
		LongSignal.DrawStyle = DRAWSTYLE_ARROWUP;
		LongSignal.PrimaryColor = COLOR_CYAN;
		LongSignal.LineWidth = 1;
		LongSignal.DrawZeros = false;

		ShortSignal.Name = "Short Signal";
		ShortSignal.DrawStyle = DRAWSTYLE_ARROWDOWN;
		ShortSignal.PrimaryColor = COLOR_MAGENTA;
		ShortSignal.LineWidth = 1;
		ShortSignal.DrawZeros = false;
			
		enabled.Name = "Trading Enabled";
		enabled.SetYesNo(0); 
			
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
	
	if(sc.UpdateStartIndex == 0)
	{
		// set ACS Tool toolbar button text
		sc.SetACSToolButtonText(1, "Ls");
		sc.SetACSToolButtonText(2, "Ss");
		sc.SetACSToolButtonText(3, "P");
	}
	
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

	
	// Calculate MAs
    // @@@
    
    sc.ExponentialMovAvg(sc.Close, TouchMA, PeriodTouchMA.GetInt());
    Calculate2PoleSuperSmoother(sc, FastMA, PeriodFastMA.GetInt());
    Calculate3PoleSuperSmoother(sc, SlowMA, PeriodSlowMA.GetInt());
    sc.SimpleMovAvg(sc.Close, SR_MA, PeriodSR_MA.GetInt());
	
    int currentTrend = NoTrend;
    
    // Time filter
    if ((barTime < filterStartTime || barTime > filterEndTime))
    {
        return;
    }
	
	if(FastMA[sc.Index] >= SlowMA[sc.Index]   &&
	   FastMA[sc.Index] > FastMA[sc.Index-1]  &&
       SlowMA[sc.Index] > SR_MA[sc.Index])
	{
		currentTrend = UpTrend;
	}
	else if(FastMA[sc.Index] <= SlowMA[sc.Index]  &&
	        FastMA[sc.Index] < FastMA[sc.Index-1] &&
            SlowMA[sc.Index] < SR_MA[sc.Index])
	{
		currentTrend = DownTrend;
	}
	
	if(currentTrend == UpTrend &&
	   sc.Close[sc.Index] > sc.Open[sc.Index] &&
	   sc.High[sc.Index] > TouchMA[sc.Index] &&
	   sc.Low[sc.Index] <= TouchMA[sc.Index])
	{
		//sc.AddMessageToLog("Buy conditions met.", 0);
        LongSignal[sc.Index] = sc.Low[sc.Index]-sc.TickSize*3;
		
		// Buy
		if (enabled.GetYesNo()  &&
		    longsEnable == 1    &&
            sc.Index != lastEntryBar)
		{
            int r = EnterLongOnHigh(sc);
            //sc.AddMessageToLog("Buying.", 0);
            
            if(r > 0)
                lastEntryBar = r;
		}
	}
	else if(currentTrend == DownTrend &&
	        sc.Close[sc.Index] < sc.Open[sc.Index] &&
	        sc.High[sc.Index] >= TouchMA[sc.Index] &&
	        sc.Low[sc.Index] < TouchMA[sc.Index])
	{
		//sc.AddMessageToLog("Sell conditions met.", 0);
        ShortSignal[sc.Index] = sc.High[sc.Index]+sc.TickSize*3;
		
		// Sell
		if (enabled.GetYesNo() &&
		    shortsEnable == 1  &&
            sc.Index != lastEntryBar)
		{
            int r = EnterShortOnLow(sc);
            
            if(r > 0)
                lastEntryBar = r;
		}
	}
}
