#ifndef MY_HERMITE_SPLINE_H
#define MY_HERMITE_SPLINE_H

/*
*	This is a Hermite Spline system. It accepts the command "" followed by the ___.
*/

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

// a Hermite Spline
class HermiteSpline : public BaseSystem
{
public:
	HermiteSpline(const std::string& name);

	void display(GLenum mode = GL_RENDER);

	void setTangent(int index, Vector tangent);

	void setPoint(int index, Vector point);

	void add(Vector point, Vector tangent);

	void reset(double time);

	void f(float t, float ti, float ti_1, Vector y_i, Vector y_i1, Vector s_i, Vector s_i1, Vector* f);

	int command(int argc, myCONST_SPEC char** argv);

protected:

	Vector points[40];
	Vector pointTangents[40];

	int numPoints = 0;
};
#endif