#ifndef MY_GROUND_PLANE_H
#define MY_GROUND_PLANE_H

/*

	This is a ground plane.

*/


#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

// a ground plane
class GroundPlane : public BaseSystem
{

public:
	GroundPlane(const std::string& name);
	//Vector v1, Vector v2, Vector v3
	//virtual void getState(double* p);
	//virtual void setState(double* p);
	//void reset(double time);

	void display(GLenum mode = GL_RENDER);

	//void readModel(char* fname) { m_model.ReadOBJ(fname); }
	//void flipNormals(void) { glmReverseWinding(&m_model); }
	//int command(int argc, myCONST_SPEC char** argv);

protected:

	//float m_sx;
	//float m_sy;
	//float m_sz;

	Vector m_pos;

	Vector m_v1;
	Vector m_v2;
	Vector m_v3;

	//GLMmodel m_model;

};
#endif
