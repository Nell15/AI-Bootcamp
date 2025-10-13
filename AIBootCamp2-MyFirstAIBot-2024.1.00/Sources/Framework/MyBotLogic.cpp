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
#include "Framework/InitData.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"

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

	Locator::Set(make_shared<ObjectSystem>());

	LevelData::Get().rowCount = _initData.rowCount;
	LevelData::Get().colCount = _initData.colCount;

	const span npcInfos{_initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs)};

	for (const auto& npcInfo : npcInfos)
	{
		auto agentPos = Coordinates{.q = npcInfo.q, .r = npcInfo.r};
		BOT_LOGIC_LOG(mLogger, std::format("Agent{} - Start Position: {}", npcInfo.uid, agentPos), true);

		LevelData::Get().AddOccupiedTiles(agentPos);
		LevelData::Get().GetAgents().emplace_back(npcInfo.uid, agentPos);
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);

	const span npcInfos{_turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize)};
	for (const auto& npcInfo : npcInfos)
	{
		const auto agentIt = ranges::find_if(LevelData::Get().GetAgents(), [&](const Agent& agent) { return agent.GetId() == npcInfo.uid; });

		agentIt->UpdateState(Coordinates{.q = npcInfo.q, .r = npcInfo.r});
		agentIt->SetOrder();

		auto order = agentIt->PopAndReturnNextAgentMove();

		const Coordinates directionCoord = agentIt->GetCoordinates() + CoordUtils::DirToCoordinates(order.direction);
		LevelData::Get().UpdateOccupiedTile(agentIt->GetCoordinates(), directionCoord);

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
#ifdef BOT_LOGIC_DEBUG
	LevelData::Get().currentTurn = turnData.turnNb;
#endif

	auto& objectSystem = Locator::Get<ObjectSystem>();

	LevelData::Get().StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
	objectSystem.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
}
