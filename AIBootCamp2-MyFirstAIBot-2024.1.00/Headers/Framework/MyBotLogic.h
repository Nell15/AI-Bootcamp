#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "Algorithm/Core/LevelData.h"
#include "Algorithm/PathFinding/PathFinder.h"

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG
#endif

#ifdef BOT_LOGIC_DEBUG
#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif

struct SConfigData;
struct STurnData;

//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{
public:
	Logger mLogger;

	MyBotLogic() = default;
	~MyBotLogic() override = default;

	virtual void Configure(const SConfigData& _configData);
	virtual void Init(const SInitData& _initData);
	virtual void GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders);

private:
	LevelData lvlData{};
	PathFinder pathFinder{ lvlData.getTilesType(), lvlData.getObjects() };
};