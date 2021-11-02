#pragma once
#ifndef MY_GLOBULES_H
#define MY_GLOBULES_H

/*

	This is a Globule particle system.

*/

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"
#include <vector>
using std::vector;

// a spring mass system
class Globules : public BaseSystem
{

public:
	Globules(const std::string& name);
	virtual void getState(vector<double*> p, vector<double*> v);
	virtual void setState(vector<double*> p);
	void reset(double time);
	void setVel(vector<double*> v);

	vector<double*> getGlobules();
	vector<double*> getVelocities();
	vector<double*> getPrevGlobules();
	vector<double> getMasses();
	vector<double> getRadii();
	double getR0();

	int command(int argc, myCONST_SPEC char** argv);
	void display(GLenum mode = GL_RENDER);

	int numGlobules = 0;

	vector<double> masses;
	vector<double> radii;

	vector<double*> globules;
	vector<double*> velocities;
	vector<double*> prevGlobules;

	double r0 = 2.0;

protected:

};
#endif
