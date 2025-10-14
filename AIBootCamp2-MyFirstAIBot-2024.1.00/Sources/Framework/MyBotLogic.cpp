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
#include "Systems/TileSystem.h"

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
	Locator::Set(make_shared<AgentSystem>());
	Locator::Set(make_shared<TileSystem>());

	auto& agentSystem = Locator::Get<AgentSystem>();

	LevelData::Get().rowCount = _initData.rowCount;
	LevelData::Get().colCount = _initData.colCount;

	const span npcInfos{_initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs)};

	for (const auto& npcInfo : npcInfos)
	{
		Coordinates agentPos = {.q = npcInfo.q, .r = npcInfo.r};
		Agent agent{npcInfo.uid, agentPos};

		BOT_LOGIC_LOG(mLogger, std::format("Agent{} - Start Position: {}", npcInfo.uid, agentPos), true);

		agentSystem.AddOccupiedTiles(agentPos);
		agentSystem.AddAgent(std::move(agent));
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);

	const span npcInfos{_turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize)};
	auto& agentSystem = Locator::Get<AgentSystem>();

	for (const auto& npcInfo : npcInfos)
	{
		Agent& agent = agentSystem.GetAgent(npcInfo.uid);

		agent.UpdateState(Coordinates{.q = npcInfo.q, .r = npcInfo.r});
		agent.SetOrder();

		auto agentNextOrder = agent.PopAndReturnNextAgentMove();

		const Coordinates agentMoveDirection = agent.GetCoordinates() + agentNextOrder.direction;
		agentSystem.UpdateOccupiedPosition(agent.GetCoordinates(), agentMoveDirection);

		BOT_LOGIC_LOG(
			mLogger,
			std::format("Turn{} - Agent{} - State {} - Direction choice: {} -> {}", _turnData.turnNb, agentIt->GetId(),
				agentIt->GetStateName(), order.direction, directionCoord),
			true);

		_orders.emplace_back(agentNextOrder);
	}
}

void MyBotLogic::StoreTurnData(const STurnData& turnData)
{
#ifdef BOT_LOGIC_DEBUG
	LevelData::Get().currentTurn = turnData.turnNb;
#endif

	auto& objectSystem = Locator::Get<ObjectSystem>();
	auto& tileSystem = Locator::Get<TileSystem>();

	tileSystem.StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
	objectSystem.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
}
