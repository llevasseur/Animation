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

	double Cr(double r0);
	double Cd(double r0);
	double b2(double r0);

	double* solveF(int index, vector<double*> globules, vector<double*> velocities, vector<double> radii, double r0, double mass);

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
	double myTime = 0.0001;

	double groundKs = 0.0;
	double groundKd = 0.0;

	double gravity = 0.0;

	double drag = 0.0;

	double m = 5.0;
	double n = 3.0;
	double b1 = 1.0;
};


#endif