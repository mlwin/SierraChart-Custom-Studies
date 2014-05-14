// The top of every source code file must include this line
#include "sierrachart.h"
#include "MySierraChart.h"

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


//This is the basic framework of a study function.
SCSFExport scsf_MyBB_Bias(SCStudyGraphRef sc)
{
	float Average, UpperBand, LowerBand;
	int &LastBarIndexProcessed = sc.PersistVars->i1;
	
	int CumBias = 0;

	unsigned int textColor = 0;
	
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "MyBB-Bias";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB (0, 255, 0);
		
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[1].PrimaryColor = RGB (0, 255, 0);
		
		sc.Input[0].Name = "#'s of bars";
		sc.Input[0].SetInt(50);
		
		return;
	}
	
	if (sc.Index == 0)     
	{
		LastBarIndexProcessed = -1; 
		return;
	}
			
	if(sc.Index == LastBarIndexProcessed)     
		return; 
		
	LastBarIndexProcessed = sc.Index;
	
	if (sc.Index < sc.Input[0].GetInt())
		return;
	
	// Section 2 - Do data processing here
	sc.BollingerBands(sc.BaseData[SC_LAST], sc.Subgraph[0], 14, 2, MOVAVGTYPE_SIMPLE);

	for(int i=1; i<=sc.Input[0].GetInt(); i++)
	{
		Average   = sc.Subgraph[0][sc.Index-i]; //Access the study value at the current index
		UpperBand = sc.Subgraph[0].Arrays[0][sc.Index-i];
		LowerBand = sc.Subgraph[0].Arrays[1][sc.Index-i];
		
		(sc.BaseData[SC_HIGH][sc.Index-i] > Average) ? CumBias++ : CumBias--;
		(sc.BaseData[SC_LOW][sc.Index-i]  > Average) ? CumBias++ : CumBias--;
		if(sc.BaseData[SC_HIGH][sc.Index-i] >= UpperBand) 
			CumBias++;
		else if(sc.BaseData[SC_LOW][sc.Index-i] <= LowerBand) 
			CumBias--;
	}
	
	if (CumBias > 0)
		sc.Subgraph[1][sc.Index-1] = 1;
	else
		sc.Subgraph[1][sc.Index-1] = -1;
		
		char arTempStr[64];
	// textColor = (ADX_Array[sc.Index] > ADX_Threshold.GetFloat()) ? COLOR_AQUAMARINE : COLOR_MAGENTA;
	sprintf(arTempStr, "%d", CumBias);
	DrawText(sc, 9003, 126, 80, COLOR_MAGENTA, RGB(0,0,0), arTempStr);

	//sc.AddMessageToLog(arTempStr, 1);
	
}
