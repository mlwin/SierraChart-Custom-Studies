
#ifndef _FIND2NDENTRIES_H
#define _FIND2NDENTRIES_H

// ************************************************************************************
//
// This code is provided on an "AS IS" basis, without warranty of any kind,
// including without limitation the warranties of merchantability, fitness for a
// particular purpose and non-infringement.
//
// Copyright (c) 2013 Min Lwin 
// mlwin1@yahoo.com
//
// ************************************************************************************

enum EntryStates
{
   eStateInitial,
   eStateCount0,
   eStateCount0Retrace,
   eStateCount1,
   eStateCount1Retrace,
   eStateCount2
};

void BarEntryState(SCStudyGraphRef sc, int bars, int& longEntryState, int& shortEntryState)
{
   float highest, lowest;
   
   FindMaxMin(sc, bars, highest, lowest);
   
   if(sc.BaseData[SC_LAST][sc.Index] > highest ||
	   sc.BaseData[SC_HIGH][sc.Index] > highest)
   {
      longEntryState = eStateCount0;
   }

   if(sc.BaseData[SC_LAST][sc.Index] < lowest ||
      sc.BaseData[SC_LOW][sc.Index]  < lowest)
   {
     shortEntryState = eStateCount0;
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
				longEntryState = eStateCount2;		
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
				shortEntryState = eStateCount2;	
			}
   			break;
   			
   		case eStateCount2:
   			if(sc.BaseData[SC_LAST][sc.Index] > sc.High[sc.Index-1] ||
			   sc.BaseData[SC_HIGH][sc.Index] > sc.High[sc.Index-1] )
			{			
					shortEntryState = eStateCount1Retrace;
			}
   			break;
	}
}
#endif
