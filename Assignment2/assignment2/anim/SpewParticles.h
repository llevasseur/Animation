#ifndef MY_SPEW_PARTICLES_H
#define MY_SPEW_PARTICLES_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include "SmallParticle.h"

#include <string>

// a sample simulator

class SpewParticles : public BaseSimulator
{
public:

	SpewParticles(const std::string& name, BaseSystem* target, BaseSystem** targets, int number);
	~SpewParticles();

	int step(double time);
	int init(double time)
	{
		for (int i = 0; i < numParticles; i++)
		{
			m_object->getState(m_pos0[i], m_vel0[i]);
		}
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv) { return TCL_OK; }

protected:

	Vector m_pos0[50]; // initial position
	Vector m_vel0[50]; // initial velocity
	Vector m_pos;
	Vector m_vel;

	SmallParticle* particles;

	BaseSystem* m_object;

	BaseSystem** m_objects;

	int numParticles;

};


#endif