#include "Framework/MyBotLogic.h"

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include <span>

#include "Framework/Globals.h"
#include "Framework/ConfigData.h"
#include "Framework/TurnData.h"

#include "Algorithm/Core/Coordinates.h"
#include "Algorithm/Utils/Utils.h"
#include "Framework/InitData.h"

using namespace std;

void MyBotLogic::Configure(const SConfigData& _configData)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_configData.logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);
}

void MyBotLogic::Init(const SInitData& _initData)
{
	BOT_LOGIC_LOG(mLogger, "Init", true);

	LevelData::rowCount = _initData.rowCount;
	LevelData::colCount = _initData.colCount;

	const span npcInfos{_initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs)};

	for (const auto& npcInfo : npcInfos)
	{
		auto agentPos = Coordinates{.q = npcInfo.q, .r = npcInfo.r};
		BOT_LOGIC_LOG(mLogger, std::format("Agent{} - Start Position: {}", npcInfo.uid, agentPos), true);

		LevelData::AddOccupiedTiles(agentPos);
		LevelData::GetAgents().emplace_back(npcInfo.uid, agentPos);
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);

	const span npcInfos{_turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize)};
	for (const auto& npcInfo : npcInfos)
	{
		const auto agentIt = ranges::find_if(LevelData::GetAgents(), [&](const Agent& agent) { return agent.GetId() == npcInfo.uid; });

		agentIt->UpdateState(Coordinates{.q = npcInfo.q, .r = npcInfo.r});
		agentIt->SetOrder();

		auto order = SOrder{.orderType = Move, .npcUID = agentIt->GetId(), .direction = agentIt->PopAndReturnNextAgentMove()};

		const Coordinates directionCoord = agentIt->GetCoordinates() + Coordinates::DirToCoordinates(order.direction);
		LevelData::UpdateOccupiedTile(agentIt->GetCoordinates(), directionCoord);

		BOT_LOGIC_LOG(
			mLogger,
			std::format("Turn{} - Agent{} - State {} - Direction choice: {} -> {}", _turnData.turnNb, agentIt->GetId(),
				agentIt->GetStateName(), order.direction, directionCoord),
			true);

		_orders.emplace_back(order);
	}
}

void MyBotLogic::StoreTurnData(const STurnData& turnData)
{
	auto npcIndices = views::iota(0, turnData.npcInfoArraySize);

#ifdef BOT_LOGIC_DEBUG
	LevelData::currentTurn = turnData.turnNb;
#endif

	ranges::for_each(npcIndices, [&](int)
	{
		LevelData::StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
		LevelData::StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
	});
}
