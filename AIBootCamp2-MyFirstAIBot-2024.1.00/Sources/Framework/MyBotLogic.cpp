#include "Framework/MyBotLogic.h"

#include <vector>
#include <array>
#include <unordered_set>

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

	lvlData = LevelData{_initData};
	PathFinder pathFinder{lvlData.getTilesType(), lvlData.getObjects()};

	npcOrders.resize(_initData.nbNPCs);

	for (int i = 0; i < _initData.nbNPCs; ++i)
	{
		const SNPCInfo npcInfo = _initData.npcInfoArray[i];
		const Coordinates npcCoord = {.q = npcInfo.q, .r = npcInfo.r};

		int bestDistance = INT_MAX;
		Coordinates bestTile{};

		for (const Coordinates& goalTile : lvlData.getGoalTiles())
		{
			if (const int distance = npcCoord.GetDistance(goalTile); distance < bestDistance)
			{
				bestDistance = distance;
				bestTile = goalTile;
			}
		}

		auto bestPath = pathFinder.FindPath(npcCoord, bestTile);


		auto prev = bestPath.begin();
		size_t j = 0;
		npcOrders[i].resize(bestPath.size() - 1);

		for (auto bestPathIt = std::next(bestPath.begin()); bestPathIt != bestPath.end(); ++bestPathIt)
		{
			const auto& nextCoord = *bestPathIt;
			const auto& currCoord = *prev;

			BOT_LOGIC_LOG(mLogger, format("Target tile coord = {}", currCoord), true);
			const EHexCellDirection direction = currCoord.GetNeighborDirection(nextCoord);
			BOT_LOGIC_LOG(mLogger, format("Target tile direction = {}", to_string(direction)), true);

			npcOrders[i][j++] = SOrder{.orderType = Move, .npcUID = npcInfo.uid, .direction = direction};

			prev = bestPathIt;
		}
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	const auto currentTurn = _turnData.turnNb - 1;
	for (int npcId = 0; npcId < _turnData.npcInfoArraySize; ++npcId)
		_orders.emplace_back(npcOrders[npcId][currentTurn]);
}
