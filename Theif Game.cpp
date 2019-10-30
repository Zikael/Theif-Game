// Theif Game.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <math.h>

#include "State.h"
#include "Theif.h"
#include "Guard.h"

using namespace tle;

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

		guard->play(theif, frameTimer);
		theif->play(guard);
		
		// Movement keys
		if (myEngine->AnyKeyHeld() || myEngine->AnyKeyHit())
		{
			// Movement keys

			// Check the theif is alive

			if (!theif->isDead())
			{
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
			}

			// Respawn key
			if (myEngine->KeyHit(Key_R))
			{
				// Set the theif + guard alive
				theif->setState(states->getStateCollection().IDLE);
				guard->setState(states->getStateCollection().IDLE);
			}

			if (myEngine->KeyHit(Key_T))
			{
				// Set the theif + guard alive
				theif->turnTowards(guard->getModel());
			}

			// Look At keys
			if (myEngine->KeyHit(Key_O) || myEngine->KeyHeld(Key_O))
			{
				if (!theif->isDead() && !guard->isDead())
				{
					guard->lookTowards(theif->getModel());
				}
			}
			if (myEngine->KeyHit(Key_P) || myEngine->KeyHeld(Key_P))
			{
				if (!theif->isDead() && !guard->isDead())
				{
					theif->lookTowards(guard->getModel());
				}
			}
		}
	}

	/**** Clean up ****/
	delete guard, theif, states;

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}