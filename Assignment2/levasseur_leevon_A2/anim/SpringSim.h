#ifndef MY_SPRING_SIM_H
#define MY_SPRING_SIM_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include "SpringMass.h"

#include <string>

// a spring simulator

class SpringSim : public BaseSimulator
{
public:

	SpringSim(const std::string& name, SpringMass* target);
	~SpringSim();

	float* solveF(int index, vector<float*> particles, vector<float*> velocities, vector<int> connections, vector<float*> springParams, float mass);

	int step(double time);
	int init(double time)
	{
		// No init necessary for this sim.
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv);

protected:

	Vector m_pos0; // initial position
	Vector m_vel0; // initial velocity
	Vector m_pos;
	Vector m_vel;

	SpringMass* mySystem;

	char* integration = new char[10];
	double myTime = 0.0001;

	float groundKs = 0.0;
	float groundKd = 0.0;

	double gravity = -9.8;

	double drag = 0.0;
};


#endif