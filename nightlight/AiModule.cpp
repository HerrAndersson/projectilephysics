#include "AiModule.h"

AiModule::AiModule(Level* level)
{
	this->level = level;
}

AiModule::~AiModule()
{

}

bool AiModule::HandleAI(Enemy* ai, Character* player, LightObject* spotlight)
{
	bool inLight = InLight(level, ai, spotlight);

	if (!inLight) {
		bool inSight = InSight(level, ai->GetPosition(), player->GetPosition());
		ai->SetFollowingPlayer(inSight);

		if (!ai->IsFollowingPlayer() && !ai->HasValidPath(level))
		{
			XMFLOAT3 p = ai->GetPosition();
			XMINT2 goal = GenerateRandomPosition(ai);
			//ai->SetPath(AStar(level, XMINT2((int)-p.x, (int)-p.z), goal));
			ai->SetPath(AStarNoCorners(level, XMINT2((int)-p.x, (int)-p.z), goal));
		} 
		else if (ai->IsFollowingPlayer()) 
		{
			Coord c1 = ai->GetTileCoord();
			Coord c2 = player->GetTileCoord();
			//ai->SetPath(AStar(level, XMINT2(c1.x, c1.y), XMINT2(c2.x, c2.y)));
			ai->SetPath(AStarNoCorners(level, XMINT2(c1.x, c1.y), XMINT2(c2.x, c2.y)));
		}
	}

	return ai->Update(level, spotlight, inLight);
}

XMINT2 AiModule::GenerateRandomPosition(Enemy* ai)
{
	Coord orig = ai->GetTileCoord();
	XMINT2 goal(0, 0);
	bool found = false;

	while (!found)
	{
		goal.x = rand() % 6 - 3;	
		goal.y = rand() % 6 - 3;

		goal.x += orig.x;
		goal.y += orig.y;

		Tile* tile = level->getTile(goal.x, goal.y);
		if (tile && tile->IsWalkableAI())
			found = true;
	}

	return goal;
}

vector<XMINT2> AiModule::GetPath(Level* level, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	return AStarNoCorners(level, startPosXZ, endPosXZ);
	//return AStar(level, startPosXZ, endPosXZ);
}

void AiModule::ChangeLevel(Level* level)
{
	this->level = level;
}

//void AiModule::Update(vector<GameObject*> dynamicObjects)
//{
//	//Generate dynamic pf each frame, or as often as needed.
//
//	GenerateTotalPF();
//}
//
//void AiModule::GenerateStaticPF(vector<GameObject*> staticObjects)
//{
//	//Sum the hard-coded potential fields of all static objects (walls, pillars, static lights etc). Or check position and generate a field there.
//}
//void AiModule::GenerateDynamicPF(vector<GameObject*> dynamicObjects)
//{
//	//Sum the hard-coded potential fields of all dynamic objects (player, boxes, dynamic lights etc). Or check position and generate a field there.
//}
//void AiModule::GenerateTotalPF()
//{
//	//Sum the static and dynamic fields to get the total.
//	//Update the field for each tile?
//}
//vector< vector<int> > AiModule::GetTotalPF()
//{
//	return totalPF;
//}
