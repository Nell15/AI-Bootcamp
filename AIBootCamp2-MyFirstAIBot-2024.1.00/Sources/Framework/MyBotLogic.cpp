#include "Framework/MyBotLogic.h"

#include <vector>
#include <array>
#include <unordered_set>
#include <algorithm>
#include <ranges>

#include "Framework/Globals.h"
#include "Framework/ConfigData.h"
#include "Framework/InitData.h"
#include "Framework/TurnData.h"

#include "Algorithm/Core/Coordinates.h"
#include "Algorithm/PathFinding/PathFinder.h"

using namespace std;

namespace
{
	vector<vector<SOrder>> npcOrders; // TODO: avoid global
}

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
	UpdateAgentsState();

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

	ranges::for_each(npcIndices, [&](auto)
	{
		levelData.StoreTiles(turnData.tileInfoArray, turnData.tileInfoArraySize);
		levelData.StoreObjects(turnData.objectInfoArray, turnData.objectInfoArraySize);
	});
}

void MyBotLogic::UpdateAgentsState()
{
	ranges::for_each(agents, [&](Agent& agent)
	{
		agent.UpdateState(levelData);
	});
}
