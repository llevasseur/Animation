#ifndef MY_HERMITE_SPLINE_H
#define MY_HERMITE_SPLINE_H

/*
*	This is a Hermite Spline system. It accepts the commands:
*	"add point" followed by the the 3D position of the point and the tangent.
*	"set point" and "set tangent" followed by the index of the point/tangent, then the 3D vector.
*	"load" followed by the file in double quotes ("").
*	"export" followed by the desired output file name in double quotes ("").
*	"cr".
*	"getArcLength" followed by a t value between [0,1].
*/

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>
#include <vector>
#include <regex>

#include "shared/opengl.h"

// a Hermite Spline
class HermiteSpline : public BaseSystem
{
public:
	HermiteSpline(const std::string& name);
	void reset(double time);
	void setReset();
	void setPoint(int index, Vector point);
	void setTangent(int index, Vector tangent);
	void add(Vector point, Vector tangent);
	Vector* getPoints();
	void tokenize(std::string str, std::vector<std::string>& out);	
	std::string HermiteSpline::stripPath(std::string path);
	void loadPoints(float values[6], int index);
	int loadFile(std::string filename);
	int exportFile(std::string filename);
	void f(float t, float ti, float ti_1, Vector y_i, Vector y_i1, Vector s_i, Vector s_i1, Vector* f);
	void piecewiseApprox();
	void catmullRom();
	int get_i(double t);
	double arcLength(double t);
	Vector* get_u(int i);
	int command(int argc, myCONST_SPEC char** argv);
	void display(GLenum mode = GL_RENDER);
	void remake();

protected:
	std::string hermiteName;

	bool clearOnReset;

	Vector points[40];
	Vector pointTangents[40];

	double s[39 * 100];

	Vector u[39 * 100];
	Vector u_tangent[39 * 100]; // used as the tangent to u

	int numPoints = 0;
};
#endif