#pragma once
#include "GameObject.h"
class Character : public GameObject 
{
private:

	float radius = 0.5f;
	float speed = 0.1f;
	float dragSpeed = 0.08f;

	int hitPoints = 3;
	int invulTimer = 0;

	float frame;
	int moved;

	std::vector<Animation> animations;
	float framelength;
	int currentAnim;
	int primaryAnim;

	void UpdatePrimaryAnimation();
	void UpdateSecondaryAnimation();
public:

	Character(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	~Character ( );

	float getRadius() const { return radius; }

	int GetHitPoints(){ return hitPoints; }
	void SetHitPoints(int val) { hitPoints = val; }

	int GetInvulTimer(){ return invulTimer; }
	void SetInvulTimer(int val) { invulTimer = val; }

	float GetSpeed() const { return speed; }
	int GetMoved() { return moved; }
	void SetMoved(int val) { moved = val; }
	void SetSpeed(float val) { speed = val; }

	void PlayAnimation(int animID);
	void UpdateCharacterAnimation();
	void SetPrimaryAnimation(int animID);
	void SetUpAnimation(RenderObject* anim, float framelengthin);
};

