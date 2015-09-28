#pragma once
#include "GameObject.h"
#include "AiUtil.h"
#include "lightObject.h"

class Enemy : public GameObject
{
private:
	
	const float SPEED = 40.0F;

	int enemyType;

	bool followingPlayer;
	vector<XMINT2> path;
	Coord next, end;
	XMFLOAT3 direction;
	float desiredRotation;

	bool CheckPathValidity(Level* level);

	float frame;
	int moved;

	std::vector<Animation> animations;
	float framelength;
	int currentAnim;
	int primaryAnim;

	void UpdatePrimaryAnimation();
	void UpdateSecondaryAnimation();

public:

	enum EnemyType { SMALL, MEDIUM, LARGE };
	Enemy(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType);

	virtual ~Enemy();

	bool Update(Level* level, LightObject* spotlight, bool inLight);

	void SetPath(vector<XMINT2> path);
	vector<XMINT2> GetPath();

	void SetFollowingPlayer(bool val);
	bool IsFollowingPlayer();
	bool HasValidPath(Level* level);

	void PlayAnimation(int animID);
	void SetPrimaryAnimation(int animID);
	void UpdateAnimation();
	void SetUpAnimation(RenderObject* anim, float framelengthin);
};

