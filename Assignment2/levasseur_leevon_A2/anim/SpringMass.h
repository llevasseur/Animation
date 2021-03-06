#ifndef MY_SPRING_MASS_H
#define MY_SPRING_MASS_H

/*

	This is a Spring mass system.

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
class SpringMass : public BaseSystem
{

public:
	SpringMass(const std::string& name);
	virtual void getState(vector<float*> p, vector<float*> v);
	virtual void setState(vector<float*> p);
	void reset(double time);
	void setVel(vector<float*> v);

	vector<float*> getParticles();
	vector<float*> getVelocities();
	vector<float*> getPrevParticles();
	vector<vector<int>> getConnections();
	vector<vector<float*>> getSpringParams();
	vector<float> getMasses();
	bool* getNailed();
	bool isNailed(int index);


	void setSpringID(int index1, int index2);
	void addConnection(int index1, int index2);
	void addSpringParams(int index, float ks, float kd, float restlength);
	void spring(int index1, int index2, float ks, float kd, float restlength);

	void nailParticle(int index);


	int command(int argc, myCONST_SPEC char** argv);
	void display(GLenum mode = GL_RENDER);

	int numParticles = 0;
	vector<vector<int>> myConnections;
	vector<vector<float*>> mySpringParams;

	vector<float> masses;

	vector<float*> particles;
	vector<float*> velocities;
	vector<float*> prevParticles;

	vector<int*> springs; // list of index pairs connected by a spring

	bool* nailed;

protected:

};
#endif
