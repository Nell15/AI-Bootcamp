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

#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;

// ===============================================
// Core API
// ===============================================

/**
* Method which configures MyBotLogic with SConfigData.
* In the end, nothing special happens here.
*/
void MyBotLogic::Configure(const SConfigData& _configData)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_configData.logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);
}

/**
* Method which initialises MyBotLogic.
* Various systems are prepared by calling SetLocators(), then the agents and the level data are memorized.
*/
void MyBotLogic::Init(const SInitData& _initData)
{
	BOT_LOGIC_LOG(mLogger, "Init", true);

	SetLocators();
	StoreAgents(_initData);

	LevelData::Get().rowCount = _initData.rowCount;
	LevelData::Get().colCount = _initData.colCount;
}

/**
* A method filling the list of _orders with the orders for every NPC.
* Every turn:
*	StoreTurnData is called.
*		Tiles are added to or updated in the TileSystem, which treats GoalTiles differently.
*		Objects are added to or updated in the ObjectSystem.
*	The AgentSystem manages every agent and plays their turn in order, which is then passed to the _orders list.
*		AgentState is updated based on the new coordinates.
*	npcInfo is only used to pass coordinates to the AgentSystem.
*/
void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	std::filesystem::create_directories("measures");
	std::ofstream out("measures\\measures.txt", std::ios::app);

	auto turnStart = std::chrono::system_clock::now().time_since_epoch();

	StoreTurnData(_turnData);

	const span npcInfos{_turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize)};
	auto& agentSystem = Locator::Get<AgentSystem>();

	for (const auto& npcInfo : npcInfos)
	{
		const SOrder npcOrder = PlayAgentTurn(agentSystem, npcInfo);
		_orders.emplace_back(npcOrder);
	}

	auto turnEnd = std::chrono::system_clock::now().time_since_epoch();

	auto turnDuration = chrono::duration_cast<chrono::microseconds>(turnEnd - turnStart).count();

	out << turnDuration << std::endl;

}

// ===============================================
// Utils function
// ===============================================

/**
* Utility method. Tile info is passed to the TileSystem and objectInfo is passed to the ObjectSystem.
* We also store non existing tiles for exploration purposes...
*/
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

/**
* Utility method. Initializes all systems which can then be obtained from the Locator.
*/
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

	// Update the agent position in agent system (used for collaboration)
	const SOrder agentNextOrder = agent.PopAndReturnNextAgentMove();
	const Coordinates agentMoveDirection = agent.GetPosition() + agentNextOrder.direction;
	if (agentNextOrder.orderType == Move)
		agentSystem.UpdateOccupiedPosition(agent.GetPosition(), agentMoveDirection);

	BOT_LOGIC_LOG(
		mLogger,
		std::format("Turn{} - Agent{} - State {} - {} in direction: {} -> {}", LevelData::Get().currentTurn, agent.GetId(),
			agent.GetStateName(), agentNextOrder.orderType, agentNextOrder.direction, agentMoveDirection),
		true);

	return agentNextOrder;
}
