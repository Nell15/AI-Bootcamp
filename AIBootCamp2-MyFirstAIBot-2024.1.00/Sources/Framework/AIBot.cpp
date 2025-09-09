#include "Framework/AIBot.h"

#include <assert.h>

#include "Framework/MyBotLogic.h"

#include "Framework/ConfigData.h"
#include "Framework/InitData.h"
#include "Framework/TurnData.h"

AIBot::~AIBot()
{
	BOT_LOG(mLogger, "~Bot\n", true);

	if (mpBotLogic != nullptr)
	{
		delete mpBotLogic;
	}
}

void AIBot::Configure(const SConfigData& _configData)
{
	//Instantiate BotLogic Here
	SetBotLogic(new MyBotLogic());

	mpBotLogic->Configure(_configData);
}

void AIBot::Init(const SInitData& _initData)
{
	BOT_LOG(mLogger, "Init", true);

	assert(mpBotLogic != nullptr && "mpBotLogic cannot be null at this point. Forgot to assign a botlogic in AIBot::Configure() ? ");

	BOT_LOG(mLogger, "Init BotLogic", true);

	mpBotLogic->Init(_initData);
}

void AIBot::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOG(mLogger, "GetTurnOrders", true);

	assert(mpBotLogic != nullptr && "mpBotLogic cannot be null at this point. Forgot to assign a botlogic in AIBot::Configure() ? ");

	BOT_LOG(mLogger, "GetTurnOrders", true);

	mpBotLogic->GetTurnOrders(_turnData, _orders);
}

void AIBot::SetBotLogic(BotLogicIF * _pBotLogic)
{
	BOT_LOG(mLogger, "SetBotLogic", true);

	if (mpBotLogic != nullptr)
	{
		delete mpBotLogic;
	}
	mpBotLogic = _pBotLogic;
}
