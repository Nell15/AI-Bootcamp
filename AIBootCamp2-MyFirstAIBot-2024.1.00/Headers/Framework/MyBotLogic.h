#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "Algorithm/Core/Agent.h"
#include "Algorithm/Core/LevelData.h"
#include "Algorithm/PathFinding/PathFinder.h"
#include "Framework/Globals.h"

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

	void Configure(const SConfigData& _configData) override;
	void Init(const SInitData& _initData) override;
	void GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders) override;

private:
	LevelData levelData{};
	std::vector<Agent> agents{};
	PathFinder pathFinder{ levelData };

	void StoreTurnData(const STurnData& turnData);
	void ThinkAgentOrders(const SNPCInfo* npcInfoArray, int nbNpc);
};