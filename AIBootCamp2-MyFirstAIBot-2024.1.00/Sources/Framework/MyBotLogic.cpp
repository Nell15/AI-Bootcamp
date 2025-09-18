#include "Framework/MyBotLogic.h"

#include <vector>
#include <array>
#include <unordered_set>
#include <algorithm>

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
	/*
	PathFinder pathFinder{lvlData.getTilesType(), lvlData.getObjects()};

	npcOrders.resize(_initData.nbNPCs);

	for (int i = 0; i < _initData.nbNPCs; ++i)
	{
		const SNPCInfo npcInfo = _initData.npcInfoArray[i];
		const Coordinates npcCoord = {.q = npcInfo.q, .r = npcInfo.r};

		int bestDistance = INT_MAX;
		vector<Coordinates> bestPath{};

		for (const Coordinates& goalTile : lvlData.getGoalTiles())
		{
			if (const auto path = pathFinder.FindPath(npcCoord, goalTile); path.has_value())
			{
				const auto pathSize = path.value().size();
				if (pathSize < bestDistance)
				{
					bestDistance = pathSize;
					bestPath = path.value();
				}
			}
		}

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
	*/
}

static Coordinates bestNeighbor(const vector<Coordinates> neighCoords,
	const unordered_map<Coordinates, EHexCellType>& tiles,
	const unordered_map<Coordinates, vector<SObjectInfo>>& objects)
{
	Coordinates bestNeighCoord;
	int bestNeighScore = -1;
	for (auto neighborCoord : neighCoords)
	{
		const auto neighNeighs = neighborCoord.GetNeighbors(tiles, objects);
		const int neighScore = 6 - neighNeighs.size();

		/*
		if (auto objIt = objects.find(neighborCoord); objIt != objects.end())
			neighScore - objIt->second.size();

		for (auto neighNiegh : neighNeighs)
		{
			
		}
		*/

		if (neighScore > bestNeighScore)
		{
			bestNeighCoord = neighborCoord;
			bestNeighScore = neighScore;
		}
	}

	return bestNeighCoord;
}


void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	const auto currentTurn = _turnData.turnNb - 1;

	// store explored data
	for (int npcId = 0; npcId < _turnData.npcInfoArraySize; ++npcId)
	{
		lvlData.StoreTiles(_turnData.tileInfoArray, _turnData.tileInfoArraySize);
		lvlData.StoreObjects(_turnData.objectInfoArray, _turnData.objectInfoArraySize);
	}

	// think
	for (int i = 0; i < _turnData.npcInfoArraySize; ++i)
	{
		const SNPCInfo npcInfo = _turnData.npcInfoArray[i];
		const Coordinates npcCoord = Coordinates{npcInfo.q, npcInfo.r};
		if (lvlData.getGoalTiles().empty())
		{
			const auto neighbors = npcCoord.GetNeighbors(lvlData.getTilesType(), lvlData.getObjects());
			
			auto bestN = bestNeighbor(neighbors, lvlData.getTilesType(), lvlData.getObjects());
			const SOrder order =
			{
				Move,
				npcInfo.uid,
				npcCoord.GetNeighborDirection(bestN)
			};
			_orders.emplace_back(order);
		}
		else
		{
			int bestDistance = INT_MAX;
			vector<Coordinates> bestPath{};
			for (const Coordinates& goalTile : lvlData.getGoalTiles())
			{
				if (const auto path = pathFinder.FindPath(npcCoord, goalTile); path.has_value())
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
				npcCoord.GetNeighborDirection(bestPath[0])
			};
			_orders.emplace_back(order);
		}
	}
}
