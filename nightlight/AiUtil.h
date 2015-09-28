#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>
#include <deque>
#include <list>
#include <algorithm>

#include "Level.h"
#include "GameObject.h"
#include "Tile.h"
#include "lightObject.h"

using namespace std;
using DirectX::XMFLOAT2;
using DirectX::XMINT2;

static int ManhattanDistance(Tile* n1, Tile* n2) //Cheap, less accurate
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return (abs(n1c.x - n2c.x) + abs(n1c.y - n2c.y));
}

static int EuclideanDistance(Tile* n1, Tile* n2) //Expensive, more accurate
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return (int)sqrt((pow((n1c.x - n2c.x), 2) + pow((n1c.y - n2c.y), 2)));
}

static int ChebyshevDistance(Tile* n1, Tile* n2)
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return max(abs(n1c.x - n2c.x), abs(n1c.y - n2c.y));
}

static bool Equals(XMINT2 f1, XMINT2 f2)
{
	if (f1.x == f2.x && f1.y == f2.y)
		return true;
	else
		return false;
}

static int GenerateF(Tile* child, Tile* end)
{
	return child->GetG() + ManhattanDistance(child, end);
}

static vector<XMINT2> AStar(Level* level, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	vector<XMINT2> path;

	Tile* start = level->getTile(startPosXZ.x, startPosXZ.y);
	Tile* end = level->getTile(endPosXZ.x, endPosXZ.y);
	Tile* current = nullptr;
	Tile* child = nullptr;

	list<Tile*> openList;
	list<Tile*> closedList;

	list<Tile*>::iterator i;

	if (start && end)
	{
		//Counter to limit path length
		int n = 0;
		int limit = 100;

		openList.push_back(start);
		start->SetInOpen(true);
		start->SetParent(nullptr);

		while (n == 0 || (current != end && n < limit))
		{
			//Find the smallest F value in openList and make it the current tile
			for (i = openList.begin(); i != openList.end(); ++i)
				if (i == openList.begin() || (*i)->GetF() <= current->GetF())
					current = (*i);

			//Stop if it reached the end or the current tile holds a closed door
			Door* door = current->getDoor();
			if (current == end || (door && !door->getIsOpen()))
				break;

			openList.remove(current);
			current->SetInOpen(false);

			closedList.push_back(current);
			current->SetInClosed(true);

			//Get all adjacent tiles
			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					XMINT2 currentTileCoord = current->GetTileCoord();
					if (!(x == 0 && y == 0))
					{
						int xc = currentTileCoord.x + x;
						int yc = currentTileCoord.y + y;
						child = level->getTile(xc, yc);

						if (child && child != current)
						{
							XMINT2 childTileCoord = child->GetTileCoord();

							bool inClosed = child->InClosed();
							bool inOpen = child->InOpen();

							if (!inClosed && child->IsWalkableAI())
							{
								//Check for corners 
	
								int tentativeG = current->GetG() + (int)TILE_SIZE;

								//Already in open but a better solution found, update it
								if (inOpen)
								{
									if (child->GetG() > tentativeG)
									{
										child->SetParent(current);
										child->SetG(tentativeG);
										child->SetF(GenerateF(child, end));
									}
								}
								if (current->GetParent() != child)
								{
									openList.push_back(child);
									child->SetInOpen(true);

									child->SetParent(current);
									child->SetG(tentativeG);
									child->SetF(GenerateF(child, end));
								}
							}
						}
					}
					else
					{
						//Utanför eller current själv?
						continue;
					}
				}
			}

			//Add to the counter
			n++;
		}

		//Reset open/closed in tiles
		for (i = openList.begin(); i != openList.end(); ++i)
			(*i)->SetInOpen(false);
		for (i = closedList.begin(); i != closedList.end(); ++i)
			(*i)->SetInClosed(false);

		//Retrace the path from the end to start
		while (current->GetParent() && current != start)
		{
			XMINT2 currentCoord = current->GetTileCoord();
			path.push_back(XMINT2(currentCoord.x, currentCoord.y));

			current = current->GetParent();
			n++;
		}
	}

	return path;
}

static vector<XMINT2> AStarNoCorners(Level* level, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	vector<XMINT2> path;

	Tile* start = level->getTile(startPosXZ.x, startPosXZ.y);
	Tile* end = level->getTile(endPosXZ.x, endPosXZ.y);
	Tile* current = nullptr;
	Tile* child = nullptr;
	Tile* lastCurrent = nullptr;

	list<Tile*> openList;
	list<Tile*> closedList;

	list<Tile*>::iterator i;

	if (start && end)
	{
		//Counter to limit path length
		int n = 0;
		int limit = 100;

		openList.push_back(start);
		start->SetInOpen(true);
		start->SetParent(nullptr);

		while (n == 0 || (current != end && n < limit))
		{
			lastCurrent = current;
			//Find the smallest F value in openList and make it the current tile
			for (i = openList.begin(); i != openList.end(); ++i)
				if (i == openList.begin() || (*i)->GetF() <= current->GetF())
					current = (*i);

			//Stop if it reached the end or the current tile holds a closed door
			Door* door = current->getDoor();
			if (current == end || (door && !door->getIsOpen()))
			{
				current = lastCurrent;
				break;
			}

			openList.remove(current);
			current->SetInOpen(false);

			closedList.push_back(current);
			current->SetInClosed(true);

			XMINT2 ct = current->GetTileCoord();
			Tile* adj[4];
			adj[0] = level->getTile(ct.x - 1, ct.y);
			adj[1] = level->getTile(ct.x, ct.y - 1);
			adj[2] = level->getTile(ct.x + 1, ct.y);
			adj[3] = level->getTile(ct.x, ct.y + 1);

			for (int i = 0; i < 4; i++)
			{
				XMINT2 currentTileCoord = current->GetTileCoord();
				
				child = adj[i];

				if (child && child != current)
				{
					XMINT2 childTileCoord = child->GetTileCoord();

					bool inClosed = child->InClosed();
					bool inOpen = child->InOpen();

					if (!inClosed && child->IsWalkableAI())
					{
						int tentativeG = current->GetG() + (int)TILE_SIZE;

						//Already in open but a better solution found, update it
						if (inOpen)
						{
							if (child->GetG() > tentativeG)
							{
								child->SetParent(current);
								child->SetG(tentativeG);
								child->SetF(GenerateF(child, end));
							}
						}
						if (current->GetParent() != child)
						{
							openList.push_back(child);
							child->SetInOpen(true);

							child->SetParent(current);
							child->SetG(tentativeG);
							child->SetF(GenerateF(child, end));
						}
					}
				}


			}

			//Add to the counter
			n++;
		}

		//Reset open/closed in tiles
		for (i = openList.begin(); i != openList.end(); ++i)
			(*i)->SetInOpen(false);
		for (i = closedList.begin(); i != closedList.end(); ++i)
			(*i)->SetInClosed(false);

		//Retrace the path from the end to start
		if (current)
		{
			path.push_back(end->GetTileCoord());
			while (current->GetParent() && current != start)
			{
				path.push_back(current->GetTileCoord());

				current = current->GetParent();
				n++;
			}
		}
	}

	return path;
}

static bool InSight(Level* level, XMFLOAT3 objectPos, XMFLOAT3 targetPos)
{
	//Bresenham's line algorithm
	float x1 = objectPos.x;
	float y1 = objectPos.z;
	float x2 = targetPos.x;
	float y2 = targetPos.z;

	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		swap(x1, y1);
		swap(x2, y2);
	}

	if (x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x < maxX; x++)
	{
		Tile* tile = nullptr;

		if (steep)
			tile = level->getTile(-y, -x);
		else
			tile = level->getTile(-x, -y);

		if (tile)
		{
			if (!tile->SeeThrough())
				return false;
		}
		else
			return false;

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}

	return true;
}

static bool InLight(Level* level, GameObject* object, LightObject* spotlight)
{
	if (InSight(level, object->GetPosition(), spotlight->getPosition()))
	{
		XMFLOAT3 pos = object->GetPosition();
		XMFLOAT3 lightEnemyVec = XMFLOAT3((pos.x - spotlight->getPosition().x), (pos.y - spotlight->getPosition().y), (pos.z - spotlight->getPosition().z));
		float vecLenght = sqrt((lightEnemyVec.x * lightEnemyVec.x) + (lightEnemyVec.y * lightEnemyVec.y) + (lightEnemyVec.z * lightEnemyVec.z));

		if ((spotlight->getRange() / 2) > vecLenght)
		{
			XMFLOAT3 spotDirection = spotlight->getDirection();

			float dot = spotDirection.x*lightEnemyVec.x + spotDirection.y*lightEnemyVec.y + spotDirection.z*lightEnemyVec.z;
			float lenSq1 = spotDirection.x*spotDirection.x + spotDirection.y*spotDirection.y + spotDirection.z*spotDirection.z;
			float lenSq2 = lightEnemyVec.x*lightEnemyVec.x + lightEnemyVec.y*lightEnemyVec.y + lightEnemyVec.z*lightEnemyVec.z;
			float angle = acos(dot / sqrt(lenSq1 * lenSq2));

			float angleInRads = (180 / XM_PI) * angle;

			if (spotlight->getCone() < angleInRads)
				return false;

			return true;
		}
	}
	return false;
}