#ifndef MY_TANK_H
#define MY_TANK_H

/*
*	This is a Tank system. It accepts the command "" followed by the ___.
*/

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>
#include <vector>
#include "HermiteSpline.h"

#include "shared/opengl.h"

// a Tank
class Tank : public BaseSystem
{

public:
	Tank(const std::string& name, char* modelFile, HermiteSpline* tankpath);
	virtual void getState(double* p);
	virtual void setState(double* p);
	void reset(double time);

	void display(GLenum mode = GL_RENDER);

	void readModel(char* fname) { m_model.ReadOBJ(fname); }
	void flipNormals(void) { glmReverseWinding(&m_model); }
	int command(int argc, myCONST_SPEC char** argv);

protected:

	float m_sx;
	float m_sy;
	float m_sz;

	Vector m_pos;

	GLMmodel m_model;

	HermiteSpline* tankpath;

};
#endif