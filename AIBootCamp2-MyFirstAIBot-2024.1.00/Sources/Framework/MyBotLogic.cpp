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
#include "Algorithm/PathFinding/PathFinder.h"

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
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	StoreTurnData(_turnData);
	UpdateAgentsState(_turnData.npcInfoArray, _turnData.npcInfoArraySize);

	// think
	for (int i = 0; i < _turnData.npcInfoArraySize; ++i)
	{
		const SNPCInfo npcInfo = _turnData.npcInfoArray[i];
		const auto npcCoord = Coordinates{npcInfo.q, npcInfo.r};
		if (levelData.GetGoalTiles().empty()) // searching
		{
			auto bestNEighbor = levelData.GetBestNeighbor(npcCoord);
			const SOrder order =
			{
				Move,
				npcInfo.uid,
				npcCoord.GetNeighborDirection(bestNEighbor)
			};
			_orders.emplace_back(order);
		}
		else // go to the goal
		{
			int bestDistance = INT_MAX;
			vector<Coordinates> bestPath{};
			for (const Coordinates& goalTile : levelData.GetGoalTiles())
			{
				const auto path = pathFinder.FindPath(npcCoord, goalTile);
				if (path.has_value())
				{
					const auto pathSize = path.value().size();
					if (pathSize < bestDistance)
					{
						bestDistance = pathSize;
						bestPath = path.value();
					}
				}
			}
			// TODO: do the other branch if it's not possible to go to the goal

			const SOrder order =
			{
				Move,
				npcInfo.uid,
				npcCoord.GetNeighborDirection(bestPath[1])
			};
			_orders.emplace_back(order);
		}
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

void MyBotLogic::UpdateAgentsState(const SNPCInfo* npcInfoArray, const int nbNpc)
{
	const span npcInfos{npcInfoArray, static_cast<size_t>(nbNpc)};

	if (agents.empty())
	{
		for (const auto npcInfo : npcInfos)
			agents.emplace(npcInfo.uid, Agent{npcInfo.uid, Coordinates{.q = npcInfo.q, .r = npcInfo.r}});
	}
	else
	{
		for (const auto npcInfo : npcInfos)
		{
			auto agentIt = agents.find(npcInfo.uid);

			if (agentIt == agents.end()) [[unlikely]]
				throw runtime_error("Cannot find agent");

			agentIt->second.UpdateState(levelData, Coordinates{.q = npcInfo.q, .r = npcInfo.r});
		}
	}
}
