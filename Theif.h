#ifndef _THEIF_H_
#define _THEIF_H_

#include <TL-Engine.h>

#include "Character.h"
#include "Guard.h"

class Guard;

class Theif : public Character
{
private:
	const float pWalkSpeed = pMoveSpeed;
	const float pRunSpeed = pMoveSpeed * 1.3f;
	const float pCreepSpeed = pMoveSpeed * 0.5f;

public:
	Theif(tle::IMesh* mesh, float moveSpeed, float x, float y, float z);

	~Theif();

	void setState(const int newState);

	void play(Guard* target);
};

#endif
