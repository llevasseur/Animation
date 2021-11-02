#ifndef MY_SMALL_PARTICLE_H
#define MY_SMALL_PARTICLE_H

/*

	This is a sample system. It accepts the command "read" followed by the
	path to an OBJ model.

*/


#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

// a sample system
class SmallParticle : public BaseSystem
{

public:
	SmallParticle(const std::string& name);
	virtual void getState(double* p, double* v);
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

	float m_mass;

	Vector initial_pos;
	Vector initial_vel;

	Vector m_pos;
	Vector m_vel;

	GLMmodel m_model;

};
#endif
