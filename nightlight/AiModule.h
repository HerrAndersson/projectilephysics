#pragma once
#include "AiUtil.h"
#include "Enemy.h"
#include "Character.h"
using namespace std;

class AiModule
{
private:

	const int R_MAX = 2;
	const int R_MIN = 1;

	Level* level;

	//vector< vector<int> > staticPF;
	//vector< vector<int> > dynamicPF;
	//vector< vector<int> > totalPF;

	//void GenerateStaticPF(vector<GameObject*> staticObjects);
	//void GenerateDynamicPF(vector<GameObject*> dynamicObjects);
	//void GenerateTotalPF();

	XMINT2 GenerateRandomPosition(Enemy* ai);

public:

	AiModule(Level* level);
	~AiModule();

	void ChangeLevel(Level* level);

	bool HandleAI(Enemy* enemy, Character* player, LightObject* spotlight);
	vector<XMINT2> GetPath(Level* level, XMINT2 startPosXZ, XMINT2 endPosXZ);

	//void Update(vector<GameObject*> dynamicObjects);
	//vector<vector<int>> GetTotalPF();
};

