#ifndef MY_BEZIER_H
#define MY_BEZIER_H

/*
*	This is a Bezier curve system. It accepts the command "" followed by the ___.
*/

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

// a Bezier curve
class Bezier : public BaseSystem
{
public:
	Bezier(const std::string& name);

	void display(GLenum mode = GL_RENDER);

	void setTangent(int index, Vector tangent);

	void setPoint(int index, Vector point);

	//void piecewiseApprox();

	//void add(Vector point, Vector tangent);

	void reset(double time);

	void f(float t, Vector* f);

	int command(int argc, myCONST_SPEC char** argv);

protected:

	Vector points[40];
	Vector pointTangents[40];

	double s[40];

	int numPoints = 0;
};
#endif