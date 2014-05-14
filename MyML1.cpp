// The top of every source code file must include this line
#include "sierrachart.h"
#include "MySierraChart.h"
#include "svm.h"

/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/


// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("Custom Study DLL") 

char tempStr[64];

int numAttributes = 50+1;  // 1 more for -1 index

struct svm_model* pModel = svm_load_model("c:\\svm\\ml1a.model");
struct svm_node*  pNode    = new svm_node[numAttributes*2];

inline void CollectTrainingData(SCStudyGraphRef sc)
{
	int cat = -1;
	SCSubgraphRef macd1 = sc.Subgraph[0];
	SCSubgraphRef macd2 = sc.Subgraph[1];
	SCSubgraphRef macd3 = sc.Subgraph[2];
	SCSubgraphRef macd4 = sc.Subgraph[3];
	SCSubgraphRef macd5 = sc.Subgraph[4];
	
	SCSubgraphRef rsi1  = sc.Subgraph[5];
	SCSubgraphRef rsi2  = sc.Subgraph[6];
	SCSubgraphRef rsi3  = sc.Subgraph[7];
	SCSubgraphRef rsi4  = sc.Subgraph[8];
	SCSubgraphRef rsi5  = sc.Subgraph[9];
	
	// Cheack 1 bar ago is greater or lower than 2 bars ago
	if(sc.High[sc.Index-1] >= sc.High[sc.Index-2])
	{
		cat = 1;
	}
	else if(sc.High[sc.Index-1] < sc.High[sc.Index-2])
	{
		cat = 0;
	}
	else
	{
		return;
	}
	
	
	FILE *fp = fopen("c:\\svm\\ml1a", "a");

	//FILE *fp = fopen("c:\\svm\\ml1a.csv", "a");
	if(fp == NULL)
		return;
	
	fprintf(fp, "%d ", cat);
	
	// save feature data
	
	// starting from 2 bars ago
	int index = sc.Index-2;
	int f = 1;
	// save features that generated a positive signal
	for(int i=2; i<=6; i++)
	{
		fprintf(fp, "%d:%.5f ", f++ , macd1.Arrays[5][sc.Index-i]);  
		fprintf(fp, "%d:%.5f ", f++ , macd2.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , macd3.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , macd4.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , macd5.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , rsi1.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , rsi2.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , rsi3.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , rsi4.Arrays[5][sc.Index-i]);
		fprintf(fp, "%d:%.5f ", f++ , rsi5.Arrays[5][sc.Index-i]);

		/*
		fprintf(fp, "%.5f,", macd1.Arrays[5][sc.Index-i]);  
		fprintf(fp, "%.5f,", macd2.Arrays[5][sc.Index-i]);
		fprintf(fp, "%.5f,",  macd3.Arrays[5][sc.Index-i]);
		fprintf(fp, "%.5f,",  macd4.Arrays[5][sc.Index-i]);
		fprintf(fp, "%.5f,",  macd5.Arrays[5][sc.Index-i]);
		*/
	}

	fprintf(fp, "\n");
	fclose(fp);
}

inline int PredictBias(SCStudyGraphRef sc, struct svm_model* pModel, struct svm_node *pNode)
{
	SCSubgraphRef macd1 = sc.Subgraph[0];
	SCSubgraphRef macd2 = sc.Subgraph[1];
	SCSubgraphRef macd3 = sc.Subgraph[2];
	SCSubgraphRef macd4 = sc.Subgraph[3];
	SCSubgraphRef macd5 = sc.Subgraph[4];
	
	SCSubgraphRef rsi1  = sc.Subgraph[5];
	SCSubgraphRef rsi2  = sc.Subgraph[6];
	SCSubgraphRef rsi3  = sc.Subgraph[7];
	SCSubgraphRef rsi4  = sc.Subgraph[8];
	SCSubgraphRef rsi5  = sc.Subgraph[9];

	//sc.AddMessageToLog("Predicting", 0);
	char msg[64];

	int f = 1;
	for(int i=1; i<6; i++)
	{
		pNode[f-1].index = f;  pNode[f-1].value = macd1.Arrays[5][sc.Index-i];  f++;
		pNode[f-1].index = f;  pNode[f-1].value = macd2.Arrays[5][sc.Index-i];  f++;
		pNode[f-1].index = f;  pNode[f-1].value = macd3.Arrays[5][sc.Index-i];  f++;
		pNode[f-1].index = f;  pNode[f-1].value = macd4.Arrays[5][sc.Index-i];  f++;
		pNode[f-1].index = f;  pNode[f-1].value = macd5.Arrays[5][sc.Index-i];  f++;
		pNode[f-1].index = f;  pNode[f-1].value = rsi1.Arrays[5][sc.Index-i];   f++;
		pNode[f-1].index = f;  pNode[f-1].value = rsi2.Arrays[5][sc.Index-i];   f++;
		pNode[f-1].index = f;  pNode[f-1].value = rsi3.Arrays[5][sc.Index-i];   f++;
		pNode[f-1].index = f;  pNode[f-1].value = rsi4.Arrays[5][sc.Index-i];   f++;
		pNode[f-1].index = f;  pNode[f-1].value = rsi5.Arrays[5][sc.Index-i];   f++;
	}
	pNode[50].index = -1; // indicates the end

	//sc.AddMessageToLog("SVM Predict", 0);
	double ret = svm_predict(pModel, pNode);

	if(ret >= 1.0)
	{
		return 1;
	}
	else
	{
		//sc.AddMessageToLog("Return 0", 0);
		return 0;
	}
}

//This is the basic framework of a study function.
SCSFExport scsf_MyML1(SCStudyGraphRef sc)
{
	// Section 1 - Set the configuration variables and defaults
	int& lastBarIndexProcessed = sc.PersistVars->i1; 
	//struct svm_model* pModel   = (struct svm_model*)sc.PersistVars->i2;
	//struct svm_node*  pNode    = (struct svm_node*)sc.PersistVars->i3;
	
	if (sc.Index == 0)     
		lastBarIndexProcessed = -1; 
	
	SCSubgraphRef macd1 = sc.Subgraph[0];
	SCSubgraphRef macd2 = sc.Subgraph[1];
	SCSubgraphRef macd3 = sc.Subgraph[2];
	SCSubgraphRef macd4 = sc.Subgraph[3];
	SCSubgraphRef macd5 = sc.Subgraph[4];
	
	SCSubgraphRef rsi1  = sc.Subgraph[5];
	SCSubgraphRef rsi2  = sc.Subgraph[6];
	SCSubgraphRef rsi3  = sc.Subgraph[7];
	SCSubgraphRef rsi4  = sc.Subgraph[8];
	SCSubgraphRef rsi5  = sc.Subgraph[9];
	
	SCSubgraphRef _signal = sc.Subgraph[10];
	
	// Use Subgraphs as there are 60 per study
	
	int nMA1 = 5;
	int nMA2 = 8;
	int nMA3 = 13;
	int nMA4 = 20;
	int nMA5 = 26;
	int nMA6 = 34;

	if (sc.SetDefaults)
	{
		sc.GraphName = "ML1";
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 1;

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[4].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[5].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[6].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[7].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[8].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[9].DrawStyle = DRAWSTYLE_IGNORE;
		
		sc.Subgraph[10].Name = "Signal";
		sc.Subgraph[10].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[10].DrawZeros = true;
        sc.Subgraph[10].PrimaryColor = RGB(0,185,47); // green
		
		sc.Input[0].Name = "Float Input";
		sc.Input[0].SetFloat(0.0f);
		
		return;
	}

	/*
	// read model file
	if(pModel == NULL)
	{
		sc.AddMessageToLog("Reading model", 0);
		int numAttributes = 50;

		numAttributes += 1; // 1 more for -1 index
		pNode = new svm_node[numAttributes*2];
		if(pNode != NULL)
		{
			sc.PersistVars->i3 = (int)pNode;
		}
		else
		{
			sc.AddMessageToLog("Error allocating memory for pNodes", 0);
		}

		// @@ model file name
		pModel = svm_load_model("c:\\svm\\ml1a.model");
		if(pModel != NULL)
		{
			sc.PersistVars->i2 = (int)pModel;
			sc.AddMessageToLog("Read model", 0);
		}
		else
		{
			// @@ error
			sc.AddMessageToLog("Error loading model", 0);
			return;
		}
	}
	*/
	
	// Section 2 - Do data processing here
	
	if(sc.Index < nMA6)
		return;
	
	// Process MACD and RSI on every tick
		// calculate MACD
		sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[0], nMA1, nMA2, 9, MOVAVGTYPE_EXPONENTIAL);
		sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[1], nMA2, nMA3, 9, MOVAVGTYPE_EXPONENTIAL);
		sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[2], nMA3, nMA4, 9, MOVAVGTYPE_EXPONENTIAL);
		sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[3], nMA4, nMA5, 9, MOVAVGTYPE_EXPONENTIAL);
		sc.MACD(sc.BaseData[SC_LAST], sc.Subgraph[4], nMA5, nMA6, 9, MOVAVGTYPE_EXPONENTIAL);
		
		// calculate RSI
		sc.RSI(sc.BaseDataIn[SC_LAST], sc.Subgraph[5], MOVAVGTYPE_SIMPLE, nMA2);
		sc.RSI(sc.BaseDataIn[SC_LAST], sc.Subgraph[6], MOVAVGTYPE_SIMPLE, nMA3);
		sc.RSI(sc.BaseDataIn[SC_LAST], sc.Subgraph[7], MOVAVGTYPE_SIMPLE, nMA4);
		sc.RSI(sc.BaseDataIn[SC_LAST], sc.Subgraph[8], MOVAVGTYPE_SIMPLE, nMA5);
		sc.RSI(sc.BaseDataIn[SC_LAST], sc.Subgraph[9], MOVAVGTYPE_SIMPLE, nMA6);
		
		// scale data and store in extra array #5
		macd1.Arrays[5][sc.Index] = ScaleData(macd1.Arrays[3][sc.Index], 3, -3);  // macd max/ min values are based on chart data
		macd2.Arrays[5][sc.Index] = ScaleData(macd2.Arrays[3][sc.Index], 3, -3);
		macd3.Arrays[5][sc.Index] = ScaleData(macd3.Arrays[3][sc.Index], 2, -2);
		macd4.Arrays[5][sc.Index] = ScaleData(macd4.Arrays[3][sc.Index], 1, -1);
		macd5.Arrays[5][sc.Index] = ScaleData(macd5.Arrays[3][sc.Index], 1, -1);
		rsi1.Arrays[5][sc.Index] = ScaleData(rsi1[sc.Index], 100, 0);
		rsi2.Arrays[5][sc.Index] = ScaleData(rsi2[sc.Index], 100, 0);
		rsi3.Arrays[5][sc.Index] = ScaleData(rsi3[sc.Index], 100, 0);
		rsi4.Arrays[5][sc.Index] = ScaleData(rsi4[sc.Index], 100, 0);
		rsi5.Arrays[5][sc.Index] = ScaleData(rsi5[sc.Index], 100, 0);
	
	// Process only on new bar
	if(sc.Index == lastBarIndexProcessed)     
	{
		return;
	}
		
	lastBarIndexProcessed = sc.Index;
	
	if(sc.Index < 100)
		return;
	
	//CollectTrainingData(sc);
	
	// real time predict -- comment out train and use 1 bar ago data
	_signal[sc.Index] = PredictBias(sc, pModel, pNode);

	if(sc.LastCallToFunction)
	{
		sc.AddMessageToLog("Last call to function", 0);
		if(pModel != NULL)
			svm_free_and_destroy_model(&pModel);

		if(pNode)
			delete[] pNode;
	}

}
