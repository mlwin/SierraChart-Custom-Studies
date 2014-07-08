#include <iostream>
#include <vector>

using namespace std;

// Simple Order Manager for sim testing without using built in one.
#define MY_DEBUG

class MySimOrderMgr
{

public:

	static const int NO_ACTION = 0;
	static const int LONG_TRADE = 1;
	static const int SHORT_TRADE = 2;
	static const int PROFIT_TARGET_HIT = 3;
	static const int STOPPED_OUT = 4;

	class TradePosition
	{
	public:
		double m_EntryPrice;
		int    m_TradeType;

		int Process(double lastTradedPrice, double ptTicks, double slTicks)
		{
			if (m_TradeType == LONG_TRADE)
			{
				if (lastTradedPrice > (m_EntryPrice + ptTicks))
				{
					#ifdef MY_DEBUG
						cout << "PT hit on long trade @ " << m_EntryPrice << endl;
					#endif

					return PROFIT_TARGET_HIT;
				}
				else if (lastTradedPrice < (m_EntryPrice - slTicks))
				{
					#ifdef MY_DEBUG
						cout << "SL hit on long trade @ " << m_EntryPrice << endl;
					#endif
					return STOPPED_OUT;
				}

				return NO_ACTION;
			}
			else if (m_TradeType == SHORT_TRADE)
			{
				if (lastTradedPrice < (m_EntryPrice - ptTicks))
				{
					#ifdef MY_DEBUG
						cout << "PT hit on short trade @ " << m_EntryPrice << endl;
					#endif

					return PROFIT_TARGET_HIT;
				}
				else if (lastTradedPrice > (m_EntryPrice + slTicks))
				{
					#ifdef MY_DEBUG
						cout << "SL hit on short trade @ " << m_EntryPrice << endl;
					#endif

					return STOPPED_OUT;
				}

				return NO_ACTION;
			}

			return NO_ACTION;
		}
	};

	static MySimOrderMgr* Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new MySimOrderMgr;
		}

		return m_pInstance;

	}

	void EnterLong(TradePosition tradeData)
	{
		tradeData.m_TradeType = MySimOrderMgr::LONG_TRADE;

		m_pTradeDataVec->push_back(tradeData);
	}

	void EnterShort(TradePosition tradeData)
	{
		tradeData.m_TradeType = MySimOrderMgr::SHORT_TRADE;

		m_pTradeDataVec->push_back(tradeData);
	}

	void Process(double lastTradedPrice)
	{
		int ret = 0;
		for (vector<TradePosition>::iterator it = m_pTradeDataVec->begin(); it != m_pTradeDataVec->end(); )
		{
			ret = it->Process(lastTradedPrice, m_ProfitTargetTicks, m_StopLossTicks);

			if (ret == PROFIT_TARGET_HIT)
			{
				// @@ save any data to analyze later 
			}

			if (ret != NO_ACTION)
				it = m_pTradeDataVec->erase(it);
			else
				it++;
		}

		#ifdef MY_DEBUG
		cout << "Open positions: ";
		for (unsigned i=0; i<m_pTradeDataVec->size(); ++i)
			cout << m_pTradeDataVec->at(i).m_EntryPrice << ' ';
		cout << "" << endl;
		#endif
	}

private:

	static vector<TradePosition>*  m_pTradeDataVec;
	static MySimOrderMgr*      m_pInstance;

	double m_TickSize;
	double m_ProfitTargetTicks;
	double m_StopLossTicks;

	MySimOrderMgr()
	{
		m_TickSize = 1.0;
		m_ProfitTargetTicks = 15.0 * m_TickSize;
		m_StopLossTicks     = 15.0 * m_TickSize;

		m_pTradeDataVec = new vector<TradePosition>();
	}

};

MySimOrderMgr* MySimOrderMgr::m_pInstance = 0;
vector<MySimOrderMgr::TradePosition>* MySimOrderMgr::m_pTradeDataVec = 0;

