// Theif Game.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <math.h>
#include <vector>

#include "CMatrix4x4cut.h"
#include "CVector3cut.h"

#include <iostream>

using namespace tle;

class State
{
protected:
	const struct States {
		const int IDLE	= 0;
		const int ALERT	= 1;

		const int CREEP = 10;
		const int WALK	= 11;
		const int RUN	= 12;

		const int DEAD	= 20;
	};

	States stateCollection;

	int pState;
	int pMovementState;

public:
	State()
	{
		// Set state initially to idle
		pState = stateCollection.IDLE;
		pMovementState = stateCollection.WALK;
	}

	~State() { /* Empty */ }

	int getState()
	{
		return pState;
	}

	int getMovementState()
	{
		return pMovementState;
	}

	States getStateCollection()
	{
		return stateCollection;
	}
};

class Character : public State
{
protected:
	float pMoveSpeed;
	IModel* pModel;
public:

	Character(IMesh* mesh, float moveSpeed, float x, float y, float z)
	{
		pMoveSpeed = moveSpeed;
		pModel = mesh->CreateModel(x, y, z);
	}

	~Character() { /* Empty */ }

	IModel* getModel() noexcept
	{
		return pModel;
	}

	void setMoveSpeed(float newSpeed) noexcept
	{
		pMoveSpeed = newSpeed;
	}

	float getMoveSpeed()
	{
		return pMoveSpeed;
	}

	float getDistance(IModel* mod)
	{

		const float x = pModel->GetLocalX() - mod->GetX();
		const float y = pModel->GetLocalY() - mod->GetY();
		const float z = pModel->GetLocalZ() - mod->GetZ();

		return sqrt(x * x + y * y + z * z);
	}

	void facingVector(float& x, float& y, float& z)
	{
		float matrix[16];

		pModel->GetMatrix(matrix);

		x = matrix[8];
		y = matrix[9];
		z = matrix[10];
	}

	bool isFacing(IModel* mod)
	{
		float x, y, z;
		facingVector(x, y, z);

		return (x * mod->GetLocalX() + y * mod->GetLocalY() + z * mod->GetLocalZ()) > 0;
	}

	void lookTowards(IModel* mod)
	{
		float pMatrix[16], modMatrix[16];
		pModel->GetMatrix(pMatrix);
		mod->GetMatrix(modMatrix);

		CVector3 pPosition, modPosition;

		pPosition.Set(&pMatrix[12]);
		modPosition.Set(&modMatrix[12]);
		
		// Calculate matrix axes for guard
		// Get facing (z) vector from positions
		CVector3 vecZ = Normalise(Subtract(modPosition, pPosition));
		// Use cross products to get other axes
		// Must normalise axes

		CVector3 vecX = Normalise(Cross(kYAxis, vecZ));

		//CVector3 vecY = Normalise(Cross(vecZ, vecX));
		CVector3 vecY = Cross(vecZ, vecX);

		// Build matrix from axes + position
		// Matrix constructor using four CVector3 variables
		// - one for each row/column
		// (see matrix header)
		// Build matrix by row (axes + position)
		CMatrix4x4 pMat;
		pMat.MakeIdentity();
		pMat.SetRow(0, vecX);
		pMat.SetRow(1, vecY);
		pMat.SetRow(2, vecZ);
		pMat.SetRow(3, pPosition);

		// Set position of guard using matrix
		pModel->SetMatrix(&pMat.e00);
	}

	bool isDead() noexcept
	{
		return pState == stateCollection.DEAD;
	}
};

class Guard : public Character
{
private:
	inline constexpr float ABOVE_X() { return 4.0f; };
	const float pPatrolSpeed = pMoveSpeed * 0.6f;
	int pState;
	IModel* pStateModel;

	int patrolIndex = 0;
	const float patrolCoords[5][3] = {
		{  25.0f, 0.0f, 0.0f },
		{  25.0f, 0.0f, 25.0f },
		{  0.0f, 0.0f, 25.0f },
		{  -25.0f, 0.0f, 0.0f },
		{  0.0f, 0.0f, 0.0f },
	};


	// Sound control
	std::vector<IModel*> pSoundModels;

public:
	Guard(IMesh* guardMesh, IMesh* stateMesh, float moveSpeed, float guardX, float guardY, float guardZ)
		: Character(guardMesh, moveSpeed, guardX, guardY, guardZ)
	{
		pStateModel = stateMesh->CreateModel(0.0f, 0.0f + ABOVE_X(), 0.0f);
		pStateModel->AttachToParent(pModel);

		// Call class' setState function to set state model texture
		setState(getState());
	}

	~Guard() { /* Empty */ }

	void setState(const int newState)
	{
		pState = newState;

		if (newState == stateCollection.IDLE)
		{
			pModel->SetSkin("Shirt-B.bmp");
			pStateModel->SetSkin("blue.png");
		}
		else if (newState == stateCollection.ALERT)
		{
			pStateModel->SetSkin("red.png");
		}
		else if (newState == stateCollection.DEAD)
		{
			pModel->SetSkin("purple.png");
		}	
	}

	bool isAlerted() noexcept
	{
		return pState == stateCollection.ALERT;
	}

	void chase(const IModel* mod1)
	{
		pModel->LookAt(mod1);
		pModel->MoveLocalZ(pPatrolSpeed);
	}

	void moveToNext()
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
			if(patrolIndex == (sizeof patrolCoords / sizeof patrolCoords[0]) - 1)
			{
				patrolIndex = 0;
			}
			else {
				patrolIndex++;
			}
		}

		// We have our new path -- let's move towards it...
		pModel->LookAt(patrolCoords[patrolIndex][0], patrolCoords[patrolIndex][1], patrolCoords[patrolIndex][2]);
		pModel->MoveLocalZ(pMoveSpeed);
	}

	void patrol()
	{
		// Only patrol if we are not alerted!
		if (!pState == 0)
		{
			return;
		}

		moveToNext();
	}

	void newSoundModel(IMesh* mesh, float x, float y, float z)
	{
		// The model for sound is a 5x5 square - so we must subtract 5 from the y coord to make it the requried height
		pSoundModels.push_back(mesh->CreateModel(x, y - 5.0f, z));
		pSoundModels.back()->SetSkin("red.png");
		pSoundModels.back()->RotateLocalX(90);
	}

	bool isWithinSoundRangeOfModel(IModel* mod ,float range)
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
					// The gaurd is alerted

					setState(stateCollection.ALERT);
				}
			}
		}

		return true;
	}
};

class Theif : public Character
{
private:
	const float pWalkSpeed =	pMoveSpeed;
	const float pRunSpeed =		pMoveSpeed * 1.3f;
	const float pCreepSpeed =	pMoveSpeed * 0.5f;

public:
	Theif(IMesh* mesh, float moveSpeed, float x, float y, float z) 
		: Character(mesh, moveSpeed, x, y, z)
	{ /* Empty */ }

	~Theif() { /* Empty */ }

	void setState(const int newState)
	{
		

		if (newState == stateCollection.IDLE)
		{
			pState = newState;
			pModel->SetSkin("sierra.jpg");
		}
		else if (newState == stateCollection.DEAD)
		{
			pState = newState;
			pModel->SetSkin("purple.png");
		} 
		else if (newState == stateCollection.WALK)
		{
			pMovementState = newState;
			setMoveSpeed(pWalkSpeed);
		}
		else if (newState == stateCollection.RUN)
		{
			pMovementState = newState;
			setMoveSpeed(pRunSpeed);
		}
		else if (newState == stateCollection.CREEP)
		{
			pMovementState = newState;
			setMoveSpeed(pCreepSpeed);
		}
	}
};

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();


	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
	myEngine->AddMediaFolder("./models");

	/**** Set up your scene here ****/
	

	const float floorSoundCoords[5][3] = {
		{10.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 10.0f},
		{0.0f, 0.0f, 20.0f},
		{20.0f, 0.0f, 0.0f},
		{30.0f, 0.0f, 0.0f}
	};
	
	const float VIEW_DISTANCE	= 8.f;	// Distance the guard can see the theif
	const float ESCAPE_DISTANCE	= 12.f;	// Distance theif has to be to change guard state to idle
	const float CAUGHT_DISTANCE = 1.f;	// Distance character has to be to "kill" the other

	const float TIMER_SOUND_CHECK	= 0.2f;	// Time between tile sound checks

	const float SOUND_RANGE_WALK	= 4.0f;	// Tile sound amplification for walking
	const float SOUND_RANGE_RUN		= 8.f;	// Tile sound amplification for running
	
	const float MOVE_SPEED				  = 0.01f;	// Distance moved per game tick
	const float ROTATION_SPEED_MULTIPLIER = 5.f;	// Rotation speed multiplier relative to move speed
	/**** Create world models ****/

	IMesh* casual_A =	myEngine->LoadMesh("./models/casual_A.x");
	IMesh* state =		myEngine->LoadMesh("./models/state.x");
	IMesh* sierra =		myEngine->LoadMesh("./models/sierra.x");
	IMesh* square =		myEngine->LoadMesh("./models/Square.x");

	Guard* guard = new Guard(casual_A, state, MOVE_SPEED/2, 5.0f, 0.0f, 0.0f);
	Theif* theif = new Theif(sierra, MOVE_SPEED, -5.0f, 0.0f, 0.0f);

	for (int idx = 0; idx < sizeof floorSoundCoords / sizeof floorSoundCoords[0]; idx++)
	{
		guard->newSoundModel(square, floorSoundCoords[idx][0], floorSoundCoords[idx][1], floorSoundCoords[idx][2]);
	}

	// Create pointer to our states
	State* states = new State();

	/**** Set up camera ****/
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0.0f, 4.f, -5.0f);
	myCamera->RotateLocalX(25.f);
	myCamera->AttachToParent(theif->getModel());


	float frameTimer = 0.f;
	myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning() && !myEngine->KeyHit(Key_Escape))
	{
		// Draw the scene
		myEngine->DrawScene();
		frameTimer += myEngine->Timer();

		/**** Update your scene each frame here ****/

		// Only check if the guard is not in the dead state
		if (!guard->isDead())
		{
			// If the gaurd is facing the theif and is within distance - set to alerted state
			if (theif->getState() == 0 && guard->isFacing(theif->getModel()) && guard->getDistance(theif->getModel()) < VIEW_DISTANCE) {
				guard->setState(states->getStateCollection().ALERT);
			}
			// Change the state of the guard if the theif gets away
			else if (guard->isAlerted() && guard->getDistance(theif->getModel()) > ESCAPE_DISTANCE)
			{
				guard->setState(states->getStateCollection().IDLE);
			}

			// If we are alerted & the theif is alive - chase!
			if (guard->isAlerted() && theif->getState() == states->getStateCollection().IDLE)
			{
				guard->chase(theif->getModel());

				// If we catch the theif set the state to dead
				if (guard->getDistance(theif->getModel()) < CAUGHT_DISTANCE)
				{
					theif->setState(states->getStateCollection().DEAD);
					guard->setState(states->getStateCollection().IDLE);

					guard->patrol();
				}
			}
			else 
			{
				guard->patrol();
			}

			// Ignore the sound check if it's been >0.2 secs and we are in a creeping state!
			if (frameTimer > TIMER_SOUND_CHECK&& theif->getMovementState() != states->getStateCollection().CREEP)
			{
				frameTimer = 0.f;

				float soundRange = SOUND_RANGE_WALK;
				// Increase range if we are running
				if (theif->getMovementState() == states->getStateCollection().RUN)
				{
					soundRange = SOUND_RANGE_RUN;
				}

				guard->isWithinSoundRangeOfModel(theif->getModel(), soundRange);
			}

			// Check if theif has killed guard
			if (theif->getDistance(guard->getModel()) < CAUGHT_DISTANCE && !guard->isAlerted() && !theif->isDead())
			{
				// We have snuck up on the guard - set to dead state
				guard->setState(states->getStateCollection().DEAD);
			}
		}
		

		

		// Movement keys
		if (myEngine->AnyKeyHeld() || myEngine->AnyKeyHit())
		{
			// Movement keys
			if (myEngine->KeyHeld(Key_W))
			{
				theif->getModel()->MoveLocalZ(theif->getMoveSpeed());
			}
			if (myEngine->KeyHeld(Key_S))
			{
				theif->getModel()->MoveLocalZ(-theif->getMoveSpeed());
			}
			if (myEngine->KeyHeld(Key_A))
			{
				theif->getModel()->MoveLocalX(-theif->getMoveSpeed());
			}
			if (myEngine->KeyHeld(Key_D))
			{
				theif->getModel()->MoveLocalX(theif->getMoveSpeed());
			}

			// Rotation keys
			if (myEngine->KeyHeld(Key_E))
			{
				theif->getModel()->RotateLocalY(theif->getMoveSpeed() * ROTATION_SPEED_MULTIPLIER);
			}
			if (myEngine->KeyHeld(Key_Q))
			{
				theif->getModel()->RotateLocalY(-theif->getMoveSpeed() * ROTATION_SPEED_MULTIPLIER);
			}

			// Speed keys
			if (myEngine->KeyHit(Key_1))
			{
				theif->setState(states->getStateCollection().WALK);
			}
			if (myEngine->KeyHit(Key_2))
			{
				theif->setState(states->getStateCollection().RUN);
			}
			if (myEngine->KeyHit(Key_3))
			{
				theif->setState(states->getStateCollection().CREEP);
			}


			// Respawn key
			if (myEngine->KeyHit(Key_R))
			{
				// Set the theif + guard alive
				theif->setState(states->getStateCollection().IDLE);
				guard->setState(states->getStateCollection().IDLE);
			}

			// Look At keys
			if (myEngine->KeyHit(Key_O) || myEngine->KeyHeld(Key_O))
			{
				guard->lookTowards(theif->getModel());
			}
			if (myEngine->KeyHit(Key_P) || myEngine->KeyHeld(Key_P))
			{
				theif->lookTowards(guard->getModel());
			}
		}
	}

	/**** Clean up ****/
	delete guard, theif, states;

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}