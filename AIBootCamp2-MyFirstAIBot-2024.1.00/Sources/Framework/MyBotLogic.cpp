#include "Framework/MyBotLogic.h"

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include <cassert>
#include <span>

#include "Framework/Globals.h"
#include "Framework/ConfigData.h"
#include "Framework/TurnData.h"

#include "Algorithm/Core/Coordinates.h"
#include "Algorithm/PathFinding/PathFinder.h"
#include "Framework/InitData.h"

using namespace std;

void MyBotLogic::Configure(const SConfigData& _configData)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_configData.logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);

	//Write Code Here
}

// TODO: put this in another class or something
void MyBotLogic::Init(const SInitData& _initData)
{
	BOT_LOGIC_LOG(mLogger, "Init", true);

	const span npcInfos{ _initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs) };

	for (const auto& npcInfo : npcInfos)
		agents.emplace_back(Agent{ npcInfo.uid, Coordinates{.q = npcInfo.q, .r = npcInfo.r} });
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);
	ThinkAgentOrders(_turnData.npcInfoArray, _turnData.npcInfoArraySize);

	for (auto& agent : agents)
	{
		_orders.emplace_back(SOrder{ .orderType = Move, .npcUID = agent.GetId(), .direction = agent.PopAndReturnBack() });
	}
}

void MyBotLogic::StoreTurnData(const STurnData& turnData)
{
	auto npcIndices = views::iota(0, turnData.npcInfoArraySize);

	ranges::for_each(npcIndices, [&](int)
	{
		levelData.StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
		levelData.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
	});
}

void MyBotLogic::ThinkAgentOrders(const SNPCInfo* npcInfoArray, const int nbNpc)
{
	const span npcInfos{npcInfoArray, static_cast<size_t>(nbNpc)};

	for (const auto& npcInfo : npcInfos)
	{
		auto agentIt = ranges::find_if(agents, [&](const Agent& agent) { return agent.GetId() == npcInfo.uid; });

		assert(agentIt != agents.end() && "Cannot find agent");

		agentIt->UpdateState(levelData, Coordinates{ .q = npcInfo.q, .r = npcInfo.r });
		agentIt->SetOrder(levelData);
	}
}
