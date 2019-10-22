#include "State.h"

State::State()
{
	// Set state initially to idle
	pState = stateCollection.IDLE;
	pMovementState = stateCollection.WALK;
}

State::~State() { /* Empty */ }

int State ::getState()
{
	return pState;
}

int State::getMovementState()
{
	return pMovementState;
}

State::States State::getStateCollection()
{
	return stateCollection;
}