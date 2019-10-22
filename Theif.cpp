#include "Theif.h";

Theif::Theif(tle::IMesh* mesh, float moveSpeed, float x, float y, float z)
	: Character(mesh, moveSpeed, x, y, z)
{ /* Empty */}

Theif::~Theif() { /* Empty */ }

void Theif::setState(const int newState)
{


	if (newState == stateCollection.IDLE)
	{
		// IDLE
		pState = newState;
		pModel->SetSkin("sierra.jpg");
	}
	else if (newState == stateCollection.DEAD)
	{
		// DEAD
		pState = newState;
		pModel->SetSkin("purple.png");
	}
	else if (newState == stateCollection.WALK)
	{
		// WALK
		pMovementState = newState;
		setMoveSpeed(pWalkSpeed);
	}
	else if (newState == stateCollection.RUN)
	{
		// RUN
		pMovementState = newState;
		setMoveSpeed(pRunSpeed);
	}
	else if (newState == stateCollection.CREEP)
	{
		// CREEP
		pMovementState = newState;
		setMoveSpeed(pCreepSpeed);
	}
}

void Theif::play(Guard* target)
{
	// Nothing to do if we are dead!
	if (isDead())
	{
		return;
	}

	// Check if theif has killed the target
	if (getDistance(target->getModel()) < CAUGHT_DISTANCE && !target->isAlerted() && !target->isDead())
	{
		// We have snuck up on the guard - set to dead state
		target->setState(getStateCollection().DEAD);
	}
}