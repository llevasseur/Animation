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
	virtual void getState(vector<float*> p, vector<float*> v);
	virtual void setState(vector<float*> p);
	void reset(double time);
	void setVel(vector<float*> v);

	vector<float*> getGlobules();
	vector<float*> getVelocities();
	vector<float*> getPrevGlobules();
	vector<float> getMasses();
	vector<float> getRadii();
	float getR0();

	int command(int argc, myCONST_SPEC char** argv);
	void display(GLenum mode = GL_RENDER);

	int numGlobules = 0;

	vector<float> masses;
	vector<float> radii;

	vector<float*> globules;
	vector<float*> velocities;
	vector<float*> prevGlobules;

	float r0 = 2.0;

protected:

};
#endif
