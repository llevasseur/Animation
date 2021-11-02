#include "GlobuleSim.h"

GlobuleSim::GlobuleSim(const std::string& name, Globules* target) :
	BaseSimulator(name),
	mySystem(target)
{
}	// GlobuleSim

GlobuleSim::~GlobuleSim()
{
}	// GlobuleSim::~GlobuleSim

float GlobuleSim::Cr(float r0)
{
	if (strcmp(material, "solid"))
	{
		return 2 * r0;
	}
	else // infer powder or liquid (Cr the same for both)
	{
		return r0;
	}
} // GlobuleSim::Cr

float GlobuleSim::Cd(float r0)
{
	if (strcmp(material, "powder"))
	{
		return r0;
	}
	else // infer solid or liquid (Cd the same for both)
	{
		return 2 * r0;
	}
} // GlobuleSim::Cd

float GlobuleSim::b2(float r0)
{
	return b1 * pow(r0, n - m);
} // GlobuleSim::b2

float* GlobuleSim::solveF(int index, vector<float*> globs, vector<float*> vel, vector<float> radii, float r0, float mass)
{
	// Determine F total
	float* FTotal = new float[3]{ 0.0f, 0.0f, 0.0f };

	// Find globule p at index
	float* currGlob = new float[3]{ globs[index][0], globs[index][1], globs[index][2] };
	float* currVel = new float[3]{ vel[index][0], vel[index][1], vel[index][2] };
	float currRadius = radii[index];

	for (int i = 0; i < globs.size(); i++)
	{
		float* Fip = new float[3]{ 0.0f, 0.0f, 0.0f };
		if (i != index)
		{
			// Globule i acting on p
			float* otherGlob = new float[3]{ globs[i][0], globs[i][1], globs[i][2] };
			float* otherVel = new float[3]{ vel[i][0], vel[i][1], vel[i][2] };
			float otherRadius = radii[i];

			float repulsionTerm = 0;
			float dragTerm = 0;
			float thisCr = Cr(r0);
			float thisCd = Cd(r0);

			float D = sqrt(pow(otherGlob[0] - currGlob[0], 2) + pow(otherGlob[1] - currGlob[1], 2) + pow(otherGlob[2] - currGlob[2], 2));

			float* pHat = new float[3]{
				otherGlob[0] - currGlob[0],
				otherGlob[1] - currGlob[1],
				otherGlob[2] - currGlob[2]
			}; // Vector difference between globules

			// solve for Sr and Sd
			float secondTermSr = pow(D, 2) / pow(thisCr, 2) * pow(otherRadius - currRadius, 2);
			float Sr = 1 - secondTermSr;

			float secondTermSd = pow(D, 2) / pow(thisCd, 2) * pow(otherRadius - currRadius, 2);
			float Sd = 1 - secondTermSd;

			// Check for negligibal force, ie the globules are too far away to influence
			if (Sr < 0.0)
			{
				Sr = 0.0;
			}
			else
			{
				float thisB2 = b2(r0);
				repulsionTerm = Sr * ((b1 / pow(D, m)) - (thisB2 / pow(D, n)));

			}
			if (Sd < 0.0)
			{
				Sd = 0.0;
			}
			else
			{
				float* vHat = new float[3]{
				otherVel[0] - currVel[0],
				otherVel[1] - currVel[1],
				otherVel[2] - currVel[2]
				}; // Vector differnece between globule velocities

				// find dot product of vHat and pHat
				float vDotp = vHat[0] * pHat[0] + vHat[1] * pHat[1] + vHat[2] * pHat[2];

				dragTerm = Sd * (vDotp / pow(D, 2));
			}

			// Solve for Repulsion/Attraction - Drag
			float influence = repulsionTerm - dragTerm;

			// Finally, solve for Fip
			Fip[0] = pHat[0] * influence;
			Fip[1] = pHat[1] * influence;
			Fip[2] = pHat[2] * influence;

		}
		// Sum forces
		FTotal[0] = FTotal[0] + Fip[0];
		FTotal[1] = FTotal[1] + Fip[1];
		FTotal[2] = FTotal[2] + Fip[2];
	}
	// Environmental forces, drag and gravity
	//FTotal[0] = FTotal[0] - drag * v1[0];
	//FTotal[1] = FTotal[1] - (drag * v1[1]) + (mass * gravity);
	//FTotal[2] = FTotal[2] - drag * v1[2];

	return FTotal;
} // GlobuleSim::solveF


int GlobuleSim::step(double time)
{
	// Determine which integration method to use to determine new velocity and new position based on set time step
	// retrive data from system
	vector<float*> newGlobs;
	vector<float*> newV;
	vector<float*> globs = mySystem->getGlobules();
	vector<float*> vel = mySystem->getVelocities();
	vector<float*> prevGlobs = mySystem->getPrevGlobules();

	vector<float> masses = mySystem->getMasses();
	vector<float> radii = mySystem->getRadii();
	float r0 = mySystem->getR0();
	// end retrive data

	// initialize constants
	float* groundP = new float[3]{ 0.0, -5.0, 0.0 }; // initialized in system
	float* groundN = new float[3]{ 0.0, 1.0, 0.0 };

	// use user requested time
	time = myTime;

	for (int p = 0; p < globs.size(); p++)
	{
		float* currGlob = new float[3]{ globs[p][0], globs[p][1] , globs[p][2] };
		float* currVel = new float[3]{ vel[p][0], vel[p][1], vel[p][2] };
		float* FTotal = solveF(p, globs, vel, radii, r0, masses[p]);
		
		// forward euler
		float* newVel = new float[3]{ 0.0f, 0.0f, 0.0f };
		newVel[0] = currVel[0] + time * FTotal[0] / masses[p];
		newVel[1] = currVel[0] + time * FTotal[1] / masses[p];
		newVel[2] = currVel[0] + time * FTotal[2] / masses[p];

		currGlob[0] = currGlob[0] + newVel[0] * time;
		currGlob[1] = currGlob[1] + newVel[1] * time;
		currGlob[2] = currGlob[2] + newVel[2] * time;
		// Collision detection with ground plane
		// TO DO
		if (currGlob[1] + newVel[1] * time <= groundP[1])
		{
			currGlob[1] = groundP[1];
		}
		// Append new position to list
		newGlobs.push_back(currGlob);
		// Append new velocity to list
		newV.push_back(newVel);
	}

	mySystem->setState(newGlobs);
	mySystem->setVel(newV);
	return 0;

}	// GlobuleSim::step

int GlobuleSim::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("simulator %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "constants") == 0)
	{
		// m, n, b1
		if (argc == 4)
		{
			m = std::stof(argv[1]);
			n = std::stof(argv[2]);
			b1 = std::stof(argv[3]);;

		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'constants'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "material") == 0)
	{
		if (argc == 3)
		{
			strcpy(material, argv[1]);
			myTime = std::stof(argv[2]);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'material'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "ground") == 0)
	{
		if (argc == 3)
		{
			groundKs = std::stof(argv[1]);
			groundKd = std::stof(argv[2]);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'ground'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "gravity") == 0)
	{
		if (argc == 2)
		{
			gravity = std::stod(argv[1]);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'gravity'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "drag") == 0)
	{
		if (argc == 2)
		{
			drag = std::stod(argv[1]);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'drag'.", m_name.c_str());
			return TCL_ERROR;
		}
	}

	glutPostRedisplay();
	return TCL_OK;
} // GlobuleSim::command
