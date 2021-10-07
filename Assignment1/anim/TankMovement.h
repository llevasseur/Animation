#ifndef MY_TANK_MOVEMENT_H
#define MY_TANK_MOVEMENT_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"

#include <string>

// a sample simulator

class TankMovement : public BaseSimulator
{
public:

	TankMovement(const std::string& name, BaseSystem* target);
	~TankMovement();

	int step(double time);
	int init(double time)
	{
		m_object->getState(m_pos0);
		setVector(m_vel0, 0, 0, 0);
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv) { return TCL_OK; }

protected:

	Vector m_pos0; // initial position
	Vector m_vel0; // initial velocity
	Vector m_pos;
	Vector m_vel;

	BaseSystem* m_object;

};


#endif