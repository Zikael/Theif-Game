#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <TL-Engine.h>


#include "State.h"


class Character : public State
{
protected:
	float pMoveSpeed;
	tle::IModel* pModel;

	const float CAUGHT_DISTANCE = 1.f;	// Distance character has to be to "kill" the other
public:

	Character(tle::IMesh* mesh, float moveSpeed, float x, float y, float z);

	~Character();

	tle::IModel* getModel() noexcept;

	void setMoveSpeed(float newSpeed) noexcept;

	float getMoveSpeed();

	float getDistance(tle::IModel* mod);

	void facingVector(float& x, float& y, float& z);

	bool isFacing(tle::IModel* mod);

	void lookTowards(tle::IModel* mod);

	void lookTowards(float x, float y, float z);

	bool isDead() noexcept;

	virtual void setState(const int newState) = 0;
};

#endif