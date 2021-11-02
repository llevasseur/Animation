#pragma once
#ifndef MY_GLOBULE_SIM_H
#define MY_GLOBULE_SIM_H

#include <GLModel/GLModel.h>
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include "BaseSimulator.h"
#include "BaseSystem.h"
#include "Globules.h"

#include <string>

// a Globule simulator

class GlobuleSim : public BaseSimulator
{
public:

	GlobuleSim(const std::string& name, Globules* target);
	~GlobuleSim();

	float Cr(float r0);
	float Cd(float r0);
	float b2(float r0);

	float* solveF(int index, vector<float*> particles, vector<float*> velocities, vector<float> radii, float r0, float mass);

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

	Globules* mySystem;

	char* material = new char[10]{ "liquid" }; // Default to liquid
	float myTime = 0.0001;

	float groundKs = 0.0;
	float groundKd = 0.0;

	double gravity = 0.0;

	double drag = 0.0;

	float m = 5.0;
	float n = 3.0;
	float b1 = 1.0;
};


#endif