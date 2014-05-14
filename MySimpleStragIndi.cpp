// The top of every source code file must include this line
#include "sierrachart.h"


// ************************************************************************************
//
// This indicator is provided on an "AS IS" basis, without warranty of any kind,
// including without limitation the warranties of merchantability, fitness for a
// particular purpose and non-infringement.
//
// mlwin1@yahoo.com
//
// ************************************************************************************


/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/


// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("Custom Study DLL") 

int nBBPeriod = 12;
int fBBStd    = 2;
//This is the basic framework of a study function.
SCSFExport scsf_SimpleStragIndi(SCStudyGraphRef sc)
{
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "My Simple Strag Indi";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		//sc.Subgraph[0].Name = "Name";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB (0, 255, 0);
		
		sc.Subgraph[1].Name = "Signal";
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[1].PrimaryColor = RGB (0, 255, 0);
		
		sc.Input[0].Name = "Fast";
		sc.Input[0].SetInt(12);
		
		sc.Input[1].Name = "Slow";
		sc.Input[1].SetInt(26);
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	sc.BollingerBands(sc.BaseData[SC_LAST], sc.Subgraph[0], nBBPeriod, fBBStd, MOVAVGTYPE_SIMPLE);

	//Access the individual lines
	float Average = sc.Subgraph[0][sc.Index]; //Access the study value at the current index
	float UpperBand = sc.Subgraph[0].Arrays[0][sc.Index];
	float LowerBand = sc.Subgraph[0].Arrays[1][sc.Index];
	
	sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[0], sc.Input[0].GetInt(), sc.Input[1].GetInt(), 9, MOVAVGTYPE_WEIGHTED);

	//Access the individual lines
	float MACD = sc.Subgraph[0][sc.Index]; //Access the study value at the current index
	float MACDMovingAverage = sc.Subgraph[0].Arrays[2][sc.Index];
	
	if(sc.BaseData[SC_HIGH][sc.Index] >= UpperBand &&
	   MACD >= 0 &&
	   MACD >= MACDMovingAverage) 
	{
		sc.Subgraph[1][sc.Index] = 1;
	}
	else if(sc.BaseData[SC_LOW][sc.Index] <= LowerBand &&
	        MACD <= 0 &&
	        MACD <= MACDMovingAverage) 
	{
		sc.Subgraph[1][sc.Index] = -1;
	}
	
	
}
