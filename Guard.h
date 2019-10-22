#ifndef  _GUARD_H_
#define _GUARD_H_

#include <TL-Engine.h>
#include <vector>

#include "Theif.h"
#include "Character.h"

class Theif;

class Guard : public Character
{
private:

	inline constexpr float ABOVE_X() { return 4.0f; };
	const float pPatrolSpeed = pMoveSpeed * 0.6f;
	tle::IModel* pStateModel;

	int patrolIndex = 0;
	const float patrolCoords[5][3] = {
		{  25.0f, 0.0f, 0.0f },
		{  25.0f, 0.0f, 25.0f },
		{  0.0f, 0.0f, 25.0f },
		{  -25.0f, 0.0f, 0.0f },
		{  0.0f, 0.0f, 0.0f },
	};

	float lastTime = 0.f;

	// Sound control
	std::vector<tle::IModel*> pSoundModels;

	// Constants
	const float VIEW_DISTANCE = 8.f;	// Distance the guard can see the theif
	const float ESCAPE_DISTANCE = 12.f;	// Distance theif has to be to change guard state to idle

	const float TIMER_SOUND_CHECK = 0.2f;	// Time between tile sound checks

	const float SOUND_RANGE_WALK = 4.0f;	// Tile sound amplification for walking
	const float SOUND_RANGE_RUN = 8.f;	// Tile sound amplification for running

public:
	Guard(tle::IMesh* guardMesh, tle::IMesh* stateMesh, float moveSpeed, float guardX, float guardY, float guardZ);

	~Guard();

	void setState(const int newState);

	bool isAlerted() noexcept;

	void chase(tle::IModel* mod1);

	void moveToNext();

	void patrol();

	void newSoundModel(tle::IMesh* mesh, float x, float y, float z);

	bool isWithinSoundRangeOfModel(tle::IModel* mod, float range);

	void play(Theif* target, float frameTimer);
};

#endif

