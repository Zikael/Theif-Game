#ifndef _STATE_H_
#define _STATE_H_


class State
{
protected:
	const struct States {
		const int IDLE = 0;
		const int ALERT = 1;

		const int CREEP = 10;
		const int WALK = 11;
		const int RUN = 12;

		const int DEAD = 20;
	};

	States stateCollection;

	int pState;
	int pMovementState;

public:
	State();

	~State();

	int getState();

	int getMovementState();

	States getStateCollection();
};

#endif