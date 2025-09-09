#include "DllStuff//stdafx.h"
#include "DllStuff/Interop_API.h"
#include "combaseapi.h"

#include "Framework/AIBot.h"
#include "Framework/ConfigData.h"
#include "Framework/InitData.h"
#include "Framework/TurnData.h"

#include <list>

extern "C"
{
	INTEROP_API AIBot* Internal_CreateNativeAIBot()
	{
		return new AIBot();
	}

	INTEROP_API void Internal_ConfigureAIBot(AIBot* _pAIBot, const SConfigData& _configData)
	{
		_pAIBot->Configure(_configData);
	}

	INTEROP_API void Internal_InitAIBot(AIBot* _pAIBot, const SInitData& _initData)
	{
		_pAIBot->Init(_initData);
	}

	INTEROP_API SOrder* Internal_GetAIBotTurnOrders(AIBot* _pAIBot, const STurnData& _turnData, int& nbOrders)
	{
		std::list<SOrder> orders;

		_pAIBot->GetTurnOrders(_turnData, orders);

		nbOrders = (int)orders.size();
		SOrder* orderArr = (SOrder*)CoTaskMemAlloc(sizeof(SOrder)* nbOrders);

		int i = 0;
		for (SOrder& order : orders)
		{
			orderArr[i] = order;
			i++;
		}
		return orderArr;
	}
}