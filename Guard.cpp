#include "Guard.h"

Guard::Guard(tle::IMesh* guardMesh, tle::IMesh* stateMesh, float moveSpeed, float guardX, float guardY, float guardZ)
	: Character(guardMesh, moveSpeed, guardX, guardY, guardZ)
{
	pStateModel = stateMesh->CreateModel(0.0f, 0.0f + ABOVE_X(), 0.0f);
	pStateModel->AttachToParent(pModel);

	// Call class' setState function to set state model texture
	setState(getState());
}

Guard::~Guard() { /* Empty */ }

void Guard::setState(const int newState)
{
	if (newState == stateCollection.IDLE)
	{
		if (isDead()) // If we are removing the dead flag
		{
			pModel->RotateLocalX(-90);
		}
		pStateModel->SetSkin("blue.png");
	}
	else if (newState == stateCollection.ALERT)
	{
		pStateModel->SetSkin("red.png");
	}
	else if (newState == stateCollection.DEAD)
	{
		if (!isDead()) // If dead flag not already set
		{
			pModel->RotateLocalX(90);
		}
		pStateModel->SetSkin("purple.png");
	}

	pState = newState;
}

bool Guard::isAlerted() noexcept
{
	return pState == stateCollection.ALERT;
}

void Guard::chase(tle::IModel* mod1)
{
	lookTowards(mod1);
	pModel->MoveLocalZ(pPatrolSpeed);
}

void Guard::moveToNext()
{
	// Check if we are at the next coord yet


	const float x = pModel->GetLocalX() - patrolCoords[patrolIndex][0];
	const float y = pModel->GetLocalY() - patrolCoords[patrolIndex][1];
	const float z = pModel->GetLocalZ() - patrolCoords[patrolIndex][2];

	const float dist = sqrt(x * x + y * y + z * z);

	const float ARRIVED_DIST = 0.1f;
	if (dist < ARRIVED_DIST)
	{
		// Keep within the bounds of our array
		if (patrolIndex == (sizeof patrolCoords / sizeof patrolCoords[0]) - 1)
		{
			patrolIndex = 0;
		}
		else {
			patrolIndex++;
		}
	}

	// We have our new path -- let's move towards it...
	lookTowards(patrolCoords[patrolIndex][0], patrolCoords[patrolIndex][1], patrolCoords[patrolIndex][2]);
	pModel->MoveLocalZ(pMoveSpeed);
}

void Guard::patrol()
{
	if (isDead())
	{
		return;
	}
	moveToNext();
}

void Guard::newSoundModel(tle::IMesh* mesh, float x, float y, float z)
{
	// The model for sound is a 5x5 square - so we must subtract 5 from the y coord to make it the requried height
	pSoundModels.push_back(mesh->CreateModel(x, y - 5.0f, z));
	pSoundModels.back()->SetSkin("red.png");
	pSoundModels.back()->RotateLocalX(90);
}

bool Guard::isWithinSoundRangeOfModel(tle::IModel* mod, float range)
{
	for (auto const& soundModel : pSoundModels)
	{
		// Check if the theif is in the square
		const float SQUARE_SIZE = 5.f;
		if (soundModel->GetLocalX() + SQUARE_SIZE > mod->GetLocalX() && soundModel->GetLocalX() - SQUARE_SIZE < mod->GetLocalX() &&
			soundModel->GetLocalZ() + SQUARE_SIZE > mod->GetLocalZ() && soundModel->GetLocalZ() - SQUARE_SIZE < mod->GetLocalZ())
		{
			// We are inside a sound square!

			// Check if our gaurd is within distance
			const float HEARING_DISTANCE = 4.f;
			if (getDistance(soundModel) < SQUARE_SIZE + HEARING_DISTANCE)
			{
				return true;
			}
		}
	}

	// Not within distance
	return false;
}

void Guard::play(Theif* target, float frameTimer)
{
	// Nothing to do if we are dead!
	if (isDead())
	{
		return;
	}

	// Only behave if we are not dead and we have an alive target
	if (target->isDead())
	{
		patrol();
		return;
	}
	
	// If the gaurd is facing the theif and is within distance - set to alerted state
	if (!target->isDead() && isFacing(target->getModel()) && getDistance(target->getModel()) < VIEW_DISTANCE) {
		setState(getStateCollection().ALERT);
	}
	// Change the state of the guard if the theif gets away
	else if (isAlerted() && getDistance(target->getModel()) > ESCAPE_DISTANCE)
	{
		setState(getStateCollection().IDLE);
	}

	// If we are alerted & the theif is alive - chase!
	if (isAlerted() && !target->isDead())
	{
		chase(target->getModel());

		// If we catch the theif set the state to dead
		if (getDistance(target->getModel()) < CAUGHT_DISTANCE)
		{
			target->setState(getStateCollection().DEAD);
			setState(getStateCollection().IDLE);
		}
	}
	else
	{
		patrol();
	}

	lastTime = frameTimer - lastTime;

	// Ignore the sound check if it's been >0.2 secs and we are in a creeping state!
	if (!target->isDead() && lastTime > TIMER_SOUND_CHECK&& target->getMovementState() != getStateCollection().CREEP)
	{
		float soundRange = SOUND_RANGE_WALK;
		// Increase range if we are running
		if (target->getMovementState() == getStateCollection().RUN)
		{
			soundRange = SOUND_RANGE_RUN;
		}

		if (isWithinSoundRangeOfModel(target->getModel(), soundRange))
		{
			setState(getStateCollection().ALERT);
		}
	}
	
}