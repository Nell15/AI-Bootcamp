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
#include "Framework/InitData.h"

using namespace std;

static constexpr std::string_view to_string(EHexCellDirection dir) noexcept
{
	switch (dir)
	{
	case W: return "W";
	case NW: return "NW";
	case NE: return "NE";
	case E: return "E";
	case SE: return "SE";
	case SW: return "SW";
	case CENTER: return "CENTER";
	default: return "?";
	}
}

template <>
struct std::formatter<EHexCellDirection> : std::formatter<std::string_view>
{
	auto format(EHexCellDirection dir, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(to_string(dir), ctx);
	}
};

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

	levelData.qMax = _initData.rowCount;
	levelData.rMax = _initData.colCount;

	const span npcInfos{_initData.npcInfoArray, static_cast<size_t>(_initData.nbNPCs)};

	for (const auto& npcInfo : npcInfos)
	{
		auto agentPos = Coordinates{.q = npcInfo.q, .r = npcInfo.r};
		BOT_LOGIC_LOG(mLogger, std::format("Agent{} - Start Position: {}", npcInfo.uid, agentPos), true);

		agents.emplace_back(npcInfo.uid, agentPos);
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);

	const span npcInfos{ _turnData.npcInfoArray, static_cast<size_t>(_turnData.npcInfoArraySize) };
	for (const auto& npcInfo : npcInfos)
	{
		auto agentIt = ranges::find_if(agents, [&](const Agent& agent) { return agent.GetId() == npcInfo.uid; });
		assert(agentIt != agents.end() && "Cannot find agent");

		agentIt->UpdateState(levelData, Coordinates{ .q = npcInfo.q, .r = npcInfo.r });
		agentIt->SetOrder(levelData);

		auto order = SOrder{.orderType = Move, .npcUID = agentIt->GetId(), .direction = agentIt->PopAndReturnBack()};
		BOT_LOGIC_LOG(
			mLogger,
			std::format("Turn{} - Agent{} - Direction choice: {}", _turnData.turnNb, agentIt->GetId(), order.direction),
			true);

		const Coordinates directionCoord = agentIt->GetCoordinates() + Coordinates::DirToCoordinates(order.direction);
		levelData.AddOccupiedTiles(directionCoord);

		_orders.emplace_back(order);
	}
}

void MyBotLogic::StoreTurnData(const STurnData& turnData)
{
	auto npcIndices = views::iota(0, turnData.npcInfoArraySize);
	levelData.ClearOccupiedTiles();

#ifdef BOT_LOGIC_DEBUG
	levelData.currentTurn = turnData.turnNb;
#endif

	ranges::for_each(npcIndices, [&](int)
	{
		levelData.StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
		levelData.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
	});
}

void MyBotLogic::ThinkAgentOrders(const SNPCInfo* npcInfoArray, const int nbNpc)
{

}
