#include "Framework/MyBotLogic.h"

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include <span>

#include "Framework/Globals.h"
#include "Framework/ConfigData.h"
#include "Framework/TurnData.h"

#include "Core/Coordinates.h"
#include "Core/LevelData.h"
#include "Framework/InitData.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "Utils/Utils.h"
#include <Systems/ScoreSystem.h>

using namespace std;

// ===============================================
// Core API
// ===============================================

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

	SetLocators();
	StoreAgents(_initData);

	LevelData::Get().rowCount = _initData.rowCount;
	LevelData::Get().colCount = _initData.colCount;
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);

	const span npcInfos{_turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize)};
	auto& agentSystem = Locator::Get<AgentSystem>();

	for (const auto& npcInfo : npcInfos)
	{
		const SOrder npcOrder = PlayAgentTurn(agentSystem, npcInfo);
		_orders.emplace_back(npcOrder);
	}
}

// ===============================================
// Utils function
// ===============================================

void MyBotLogic::StoreTurnData(const STurnData& turnData)
{
#ifdef BOT_LOGIC_DEBUG
	LevelData::Get().currentTurn = turnData.turnNb;
#endif

	auto& objectSystem = Locator::Get<ObjectSystem>();
	auto& tileSystem = Locator::Get<TileSystem>();

	tileSystem.StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
	objectSystem.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);

	tileSystem.StoreNonExistingTiles();
}

void MyBotLogic::SetLocators()
{
	Locator::Set(make_shared<ObjectSystem>());
	Locator::Set(make_shared<AgentSystem>());
	Locator::Set(make_shared<TileSystem>());
	Locator::Set(make_shared<ScoreSystem>());
}

void MyBotLogic::StoreAgents(const SInitData& _initData)
{
	auto& agentSystem = Locator::Get<AgentSystem>();
	const span npcInfos{ _initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs) };

	for (const auto& npcInfo : npcInfos)
	{
		const Coordinates agentPos = { .q = npcInfo.q, .r = npcInfo.r };
		Agent agent{ npcInfo.uid, npcInfo.visionRange, agentPos };

		BOT_LOGIC_LOG(mLogger, std::format("Agent{} - Start Position: {}", npcInfo.uid, agentPos), true);

		agentSystem.AddOccupiedTiles(agentPos);
		agentSystem.AddAgent(std::move(agent));
	}
}

SOrder MyBotLogic::PlayAgentTurn(AgentSystem& agentSystem, const SNPCInfo& npcInfo)
{
	Agent& agent = agentSystem.GetAgent(npcInfo.uid);

	agent.UpdateState(Coordinates{ .q = npcInfo.q, .r = npcInfo.r });
	agent.SetOrder();

	const SOrder agentNextOrder = agent.PopAndReturnNextAgentMove();

	const Coordinates agentMoveDirection = agent.GetPosition() + agentNextOrder.direction;
	agentSystem.UpdateOccupiedPosition(agent.GetPosition(), agentMoveDirection);

	BOT_LOGIC_LOG(
		mLogger,
		std::format("Turn{} - Agent{} - State {} - Direction choice: {} -> {}", LevelData::Get().currentTurn, agent.GetId(),
			agent.GetStateName(), agentNextOrder.direction, agentMoveDirection),
		true);

	return agentNextOrder;
}
