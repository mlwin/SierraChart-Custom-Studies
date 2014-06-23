#ifndef _MYSIERRACHART_H
#define _MYSIERRACHART_H

// ************************************************************************************
//
// This code is provided on an "AS IS" basis, without warranty of any kind,
// including without limitation the warranties of merchantability, fitness for a
// particular purpose and non-infringement.
//
// mlwin1@yahoo.com
//
// ************************************************************************************

enum TrendTypes 
{
	NoTrend   = 0,
	UpTrend   = 1,
	DownTrend = 2
};

enum StatusCodes
{

	SHORT_SL    = -2,
	LONG_SL     = -1,
	LONG_PT     =  1,
	SHORT_PT    =  2,
	BUY_SIGNAL  =  1,
	SELL_SIGNAL =  2

};

const float MAX_DATA_RANGE = 1.0f;
const float MIN_DATA_RANGE = -1.0f;
const float PI      = 3.14159265358979f;
const float PI_HALF = PI / 2.0f;
const float RAD_TO_DEG = (180.0f/PI);

const float SP_2POLE_ARG = 1.414*PI;
const float SP_3POLE_ARG = 1.738*PI;

void DrawMarker(SCStudyGraphRef sc, unsigned int id, unsigned int barIndex, float value, unsigned int type, unsigned int color)
{
	s_UseTool Tool; 
	
	Tool.Clear(); // reset tool structure for our next use
	Tool.ChartNumber = 0;
	Tool.DrawingType = DRAWING_MARKER;
	Tool.LineNumber =  id;

	Tool.BeginDateTime = sc.BaseDateTimeIn[barIndex];
	Tool.BeginValue = value;
	Tool.Color = color;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.MarkerType = type;
	Tool.MarkerSize = 10;
	Tool.LineWidth = 2;

	sc.UseTool(Tool);
}

void DrawText(SCStudyGraphRef sc, unsigned int id, float x, float y, unsigned int color, unsigned int backcolor, char* text)
{
	s_UseTool Tool;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;
	Tool.Region = sc.GraphRegion;
	Tool.LineNumber = id;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.AddAsUserDrawnDrawing = 0;
	
	Tool.BeginDateTime = x;
	Tool.BeginValue = y;
	Tool.UseRelativeValue = true;
	
	Tool.Color = color;
	Tool.FontBackColor = backcolor;
	Tool.ReverseTextColor = false;
	Tool.FontBold = true;
	Tool.FontSize = 12;
	Tool.Text = text;
	
	sc.UseTool(Tool);
}

// put up text quick and dirty
void DrawSimText1(SCStudyGraphRef sc, char* text)
{
	DrawText(sc, 987601, 4, 80, COLOR_GREEN, RGB(0,0,0), text);
}

void DrawSimText2(SCStudyGraphRef sc, char* text)
{
	DrawText(sc, 987602, 4, 76, COLOR_RED, RGB(0,0,0), text);
}

void DrawTrendRay(SCStudyGraphRef sc, unsigned int id, unsigned int index1, float y1, unsigned int index2, float y2, unsigned int color)
{
	s_UseTool Tool;
	
	Tool.Clear(); // reset tool structure for our next use
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_RAY;
	Tool.LineNumber = id;

	Tool.BeginDateTime = sc.BaseDateTimeIn[index1];
	Tool.EndDateTime = sc.BaseDateTimeIn[index2];
	Tool.BeginValue = y1;
	Tool.EndValue = y2;
	Tool.Region = 0;
	Tool.Color = color;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;

	sc.UseTool(Tool);
}

bool IsHH(SCStudyGraphRef sc, unsigned int start)
{
	for(int i=start; i<=sc.Index; i++)
	{
		if(sc.High[sc.Index] < sc.High[i])
		{
			return false;
		}
	}
	
	return true;
}

bool IsLL(SCStudyGraphRef sc, unsigned int start)
{
	for(int i=start; i<=sc.Index; i++)
	{
		if(sc.Low[sc.Index] > sc.Low[i])
		{
			return false;
		}
	}
	
	return true;
}

void DrawShadedRec(SCStudyGraphRef sc, unsigned int id, unsigned int index1, float y1, unsigned int index2, float y2, unsigned int color, int transparency, int outline)
{
	s_UseTool Tool;
	
	Tool.Clear(); // reset tool structure for our next use
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_RECTANGLEHIGHLIGHT;
	Tool.LineNumber = id;

	Tool.BeginDateTime = sc.BaseDateTimeIn[index1];
	Tool.EndDateTime = sc.BaseDateTimeIn[index2];
	Tool.BeginValue = y1;
	Tool.EndValue = y2;
	Tool.Region = 0;
	Tool.Color = color;
	
	Tool.LineWidth = outline; //To see the outline this must be 1 or greater.
	Tool.SecondaryColor = color;
	
	Tool.TransparencyLevel = transparency;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;

	sc.UseTool(Tool);
}

float ScaleData(float x, float maxValue, float minValue)
{
	return (MAX_DATA_RANGE-MIN_DATA_RANGE)/(maxValue-minValue)*(x-minValue)+MIN_DATA_RANGE;
}

float CalculateBB_Bandwidth(SCStudyGraphRef sc, int length, SCSubgraphRef outBB)
{
	sc.BollingerBands(sc.BaseData[SC_LAST], outBB, length, 2.0, MOVAVGTYPE_SIMPLE);

	SCFloatArrayRef middleBand	= outBB;  
	SCFloatArrayRef upperBand	= outBB.Arrays[0];  
	SCFloatArrayRef lowerBand	= outBB.Arrays[1];  

	return ((upperBand[sc.Index] - lowerBand[sc.Index]) / middleBand[sc.Index]);
}

void FindMaxMin(SCStudyGraphRef sc, int length, float &highest, float &lowest)
{
	lowest  = sc.BaseData[SC_HIGH][sc.Index-1];
	highest = sc.BaseData[SC_LOW][sc.Index-1];
	
	for(int i1 = sc.Index-length; i1< sc.Index; i1++)
	{
		if(sc.BaseData[SC_HIGH][i1] > highest)
			highest = sc.BaseData[SC_HIGH][i1];

		if(sc.BaseData[SC_LOW][i1] < lowest)
			lowest = sc.BaseData[SC_LOW][i1];
	}
}

// Returns date time at sc.Index in YYYY-MM-DD HH:MM:SS format.
// dateStr must have enough space
void GetFormattedDateStr(SCStudyGraphRef sc, char* dateStr)
{
    // Get the date of the bar at the current index
    int CurrentBarDate = sc.BaseDateTimeIn[sc.Index].GetDate();

    // Get the time of the bar at the current index
    int CurrentBarTime = sc.BaseDateTimeIn[sc.Index].GetTime();

	int Year, Month, Day, Hour, Minute, Second;

	TIME_TO_HMS(CurrentBarTime, Hour, Minute, Second);
	DATE_TO_YMD(CurrentBarDate, Year, Month, Day);

	sprintf(dateStr, "%d-%d-%d %d:%d:%d", Year, Month, Day, Hour, Minute, Second);
}

void AppendToTextFile(char* file, char* line)
{
	FILE *fp = fopen(file, "a");
	
	if(fp != NULL)
	{
		fprintf(fp, line);
		fprintf(fp, "\n");
		
		fclose(fp);
	}
}

// Returns the bar # that order was entered if successful or -1
int EnterLongOnHigh(SCStudyGraphRef sc)
{
    // Create an s_SCNewOrder object. 
    s_SCNewOrder NewOrder;
    NewOrder.OrderQuantity = 1;
    NewOrder.OrderType = SCT_LIMIT;
    NewOrder.Price1 = sc.High[sc.Index]-+sc.TickSize;

    int orderResult;
    
    s_SCPositionData PositionData;
    sc.GetTradePosition(PositionData);
        
    if(PositionData.PositionQuantity == 0)
    {
        orderResult = sc.BuyEntry(NewOrder);
        if(orderResult > 0)
        {           
            // draw arrow
            DrawMarker(sc, sc.Index, sc.Index, sc.Low[sc.Index]-sc.TickSize*8, MARKER_ARROWUP, COLOR_GREEN);
            
            return sc.Index;
        }
    }
    
    return -1;
}

// Returns the bar # that order was entered if successful or -1
int EnterShortOnLow(SCStudyGraphRef sc)
{
    // Create an s_SCNewOrder object. 
    s_SCNewOrder NewOrder;
    NewOrder.OrderQuantity = 1;
    NewOrder.OrderType = SCT_LIMIT;
    NewOrder.Price1 = sc.Low[sc.Index]-sc.TickSize;
    
    int orderResult;
    
    s_SCPositionData PositionData;
    sc.GetTradePosition(PositionData);
    
    if(PositionData.PositionQuantity == 0)
    {
        orderResult = sc.SellEntry(NewOrder);
        if(orderResult > 0)
        {            
            // draw arrow
            DrawMarker(sc, sc.Index, sc.Index, sc.High[sc.Index]+sc.TickSize*8, MARKER_ARROWDOWN, COLOR_RED);
            
            return sc.Index;
        }
    }
    
    return -1;
}

// 2 Pole SuperSmoother as presented by John Ehlers
void Calculate2PoleSuperSmoother(SCStudyGraphRef sc, SCSubgraphRef filter, int period)
{
    float a1, b1, cf1, cf2, cf3;
	
	a1 = exp(-SP_2POLE_ARG/period);
	b1 = 2*a1*cos(SP_2POLE_ARG/period);
		
	cf2 = b1;
	cf3 = -a1*a1;
	cf1 = 1-cf2-cf3;
		
	if (sc.Index < 2) 
    {
        filter[sc.Index] = sc.Close[sc.Index];
    }	
	else
    {
        filter[sc.Index] = cf1*((sc.Close[sc.Index]+sc.Close[sc.Index-1])/2) + cf2*filter[sc.Index-1] + cf3*filter[sc.Index-2];
    }
}

// 3 Pole SuperSmoother as presented by John Ehlers
void Calculate3PoleSuperSmoother(SCStudyGraphRef sc, SCSubgraphRef filter, int period)
{
    float a1, b1, c1, cf1, cf2, cf3, cf4;
	
	a1 = exp(-PI/period);
	b1 = 2*a1*cos(SP_3POLE_ARG/period);
	c1 = a1*a1;
		
	cf2 = b1 + c1;
	cf3 = -(c1 + b1*c1);
	cf4 = c1*c1;
	cf1 = 1 - cf2 - cf3 - cf4;	
		
	if (sc.Index < 3)
    {
        filter[sc.Index] = sc.Close[sc.Index];
    }
	else
    {
        filter[sc.Index] = cf1*((sc.Close[sc.Index]+sc.Close[sc.Index-1])/2)  + cf2*filter[sc.Index-1] + cf3*filter[sc.Index-2] + cf4*filter[sc.Index-3];
    }
}

#endif
