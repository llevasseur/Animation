#include "SpringSim.h"

SpringSim::SpringSim(const std::string& name, SpringMass* target) :
	BaseSimulator(name),
	mySystem(target)
{
}	// SpringSim

SpringSim::~SpringSim()
{
}	// SpringSim::~SpringSim

float* SpringSim::solveF(int index, vector<float*> particles, vector<float*> velocities, vector<int> connections, vector<float*> springParams, float mass)
{
	// Determine F total
	float* FTotal = new float[3]{ 0.0f, 0.0f, 0.0f };
	//animTcl::OutputMessage("FTotal is: %f %f %f", FTotal[0], FTotal[1], FTotal[2]);
	float* p1 = new float[3]{ particles[index][0], particles[index][1], particles[index][2] };
	float* v1 = new float[3]{ velocities[index][0], velocities[index][1], velocities[index][2] };

	for (int j = 0; j < connections.size(); j++)
	{
		// Identify a connected particle to compare position and velocity with.
		int p2Index = connections[j];
		float* p2 = new float[3]{ particles[p2Index][0], particles[p2Index][1], particles[p2Index][2] };
		//animTcl::OutputMessage("p2 is: %f %f %f", particles[p2Index][0], particles[p2Index][1], particles[p2Index][2]);
		float* v2 = new float[3]{ velocities[p2Index][0], velocities[p2Index][1], velocities[p2Index][2] };

		// Calculate Spring Force
		float ks = springParams[j][0];
		float restlength = springParams[j][2];
		float dxLength = sqrt(pow(p2[0] - p1[0], 2) + pow(p2[1] - p1[1], 2) + pow(p2[2] - p1[2], 2));
		float* dxNormalized = new float[3]{ (p1[0] - p2[0]) / dxLength , (p1[1] - p2[1]) / dxLength, (p1[2] - p2[2]) / dxLength };
		//animTcl::OutputMessage("dxNormalized is: %f %f %f", dxNormalized[0], dxNormalized[1], dxNormalized[2]);
		float restMinusDxLength = restlength - dxLength;
		float coefficient = ks * restMinusDxLength;
		float* Fsp = new float[3]{ coefficient * dxNormalized[0], coefficient * dxNormalized[1], coefficient * dxNormalized[2] };

		// Calculate Damping Force
		float kd = -(springParams[j][1]);
		float* dv = new float[3]{ v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2] };
		float dvDotDxNorm = dv[0] * dxNormalized[0] + dv[1] * dxNormalized[1] + dv[2] * dxNormalized[2];
		float coefficient2 = kd * dvDotDxNorm;
		float* Fd = new float[3]{ coefficient2 * dxNormalized[0], coefficient2 * dxNormalized[1], coefficient2 * dxNormalized[2] }; // negative of kd is already applied

		// Sum forces
		FTotal[0] = FTotal[0] + Fsp[0] + Fd[0];
		FTotal[1] = FTotal[1] + Fsp[1] + Fd[1];
		FTotal[2] = FTotal[2] + Fsp[2] + Fd[2];
	}

	// Environmental forces, drag and gravity
	FTotal[0] = FTotal[0] - drag * v1[0];
	FTotal[1] = FTotal[1] - (drag * v1[1]) + (mass * gravity);
	FTotal[2] = FTotal[2] - drag * v1[2];

	return FTotal;
}

int SpringSim::step(double time)
{
	// Determine which integration method to use to determine new velocity and new position based on set time step
	// retrive data from system
	vector<float*> newP;
	vector<float*> newV;
	vector<float*> p = mySystem->getParticles();
	vector<float*> v = mySystem->getVelocities();
	vector<float*> prevP = mySystem->getPrevParticles();

	vector<vector<int>> connections = mySystem->getConnections();
	vector<vector<float*>> springParams = mySystem->getSpringParams();

	vector<float> masses = mySystem->getMasses();
	bool* nailed = mySystem->getNailed();
	// end retrive data

	// initialize constants
	float* groundP = new float[3]{ 0.0, -5.0, 0.0 }; // initialized in system
	float* groundN = new float[3]{ 0.0, 1.0, 0.0 };

	// use user requested time
	time = myTime;

	for (int i = 0; i < p.size(); i++)
	{
		bool isNailed = nailed[i];
		if (!isNailed)
		{
			// Save current position and velocity of the particle
			float* currParticle = new float[3]{ p[i][0], p[i][1], p[i][2] };
			float* currVelocity = new float[3]{ v[i][0], v[i][1], v[i][2] };

			// Solve for the total Force acting on the particle
			float* FTotal = solveF(i, p, v, connections[i], springParams[i], masses[i]);

			// Determine integration method
			if (strcmp(integration, "verlet") == 0)
			{
				// Check if prevParticles exist, ie. is this the first iteration.
				float* prevPos;
				if (prevP.size() == 0)
				{
					// Assuming constant velocity to start so v(t-h) = v0;
					prevPos = new float[3]{ p[i][0] - v[i][0], p[i][1] - v[i][1], p[i][2] - v[i][2] };
				}
				else
				{
					prevPos = new float[3]{ prevP[i][0], prevP[i][1], prevP[i][2] };
				}

				// Was getting an error unless I created individual floats.
				float item1 = 2 * currParticle[0] - prevPos[0] + (FTotal[0] / masses[i]) * pow(time, 2);
				float item2 = 2 * currParticle[1] - prevPos[1] + (FTotal[1] / masses[i]) * pow(time, 2);
				float item3 = 2 * currParticle[2] - prevPos[2] + (FTotal[2] / masses[i]) * pow(time, 2);
				float* nextPos = new float[3]{ item1, item2, item3 };

				float* newVelocity = new float[3]{
					(nextPos[0] - prevPos[0]) / (2 * (float)time),
					(nextPos[1] - prevPos[1]) / (2 * (float)time),
					(nextPos[2] - prevPos[2]) / (2 * (float)time)
				};

				// Collision detection with ground plane
				if (nextPos[1] < groundP[1])
				{
					// Collision with plane, solve for Fnormal
					float* dx = new float[3]{ currParticle[0], currParticle[1] - groundP[1], currParticle[2] };
					float dxDotN = dx[1] * groundN[1]; // N is the normal of ground plane, only contributes in the y direction
					// Define -ks(dxDotN * N), where N = groundN = {0.0, 1.0, 0.0}, ks=groundKs
					dxDotN = -groundKs * dxDotN;
					float* term1 = new float[3]{ 0.0, dxDotN, 0.0 };

					float vDotN = newVelocity[1] * groundN[1] * groundKd; // N only contributes in the y direction. kd(vDotN * N), kd=groundKd
					float* term2 = new float[3]{ 0.0, vDotN, 0.0 };

					// Finally, solve Fnormal
					float Fn = term1[1] - term2[1]; // x and z terms come out to 0.0, thus a single float is sufficient.

					FTotal[1] = FTotal[1] + Fn;
					// Solve and update velocity
					// Solve and update velocity 
					newVelocity[0] = time * FTotal[0] / masses[i];
					newVelocity[1] = time * FTotal[1] / masses[i];
					newVelocity[2] = time * FTotal[2] / masses[i];

					nextPos[0] = 2 * currParticle[0] - prevPos[0] + (FTotal[0] / masses[i]) * pow(time, 2);
					nextPos[1] = 2 * currParticle[1] - prevPos[1] + (FTotal[1] / masses[i]) * pow(time, 2);
					nextPos[2] = 2 * currParticle[2] - prevPos[2] + (FTotal[2] / masses[i]) * pow(time, 2);

					// If particle is moving down, place it on the ground plane
					if (nextPos[1] < groundP[1])
					{
						nextPos[1] = groundP[1];
					}
					// Append new velocity to list
					newV.push_back(newVelocity);
					// Append new position to list
					newP.push_back(nextPos);
				}
				else
				{
					// Append new position to list
					newP.push_back(nextPos);
					// Append new velocity to list
					newV.push_back(newVelocity);
				}
			}
			else // Euler can be inferred (either forward or symplectic, default: forward).
			{

				// Solve for new velocity after force applied (Euler)
				float* newVelocity = new float[3]{ v[i][0], v[i][1], v[i][2] };
				newVelocity[0] = newVelocity[0] + time * FTotal[0] / masses[i];
				newVelocity[1] = newVelocity[1] + time * FTotal[1] / masses[i];
				newVelocity[2] = newVelocity[2] + time * FTotal[2] / masses[i];

				// Collision detection with ground plane
				if (currParticle[1] + newVelocity[1] * time <= groundP[1])
				{
					// Collision with plane, solve for Fnormal
					float* dx = new float[3]{ currParticle[0], currParticle[1] - groundP[1], currParticle[2] };
					float dxDotN = dx[1] * groundN[1]; // N is the normal of ground plane, only contributes in the y direction
					// Define -ks(dxDotN * N), where N = groundN = {0.0, 1.0, 0.0}, ks=groundKs
					dxDotN = -groundKs * dxDotN;
					float* term1 = new float[3]{ 0.0, dxDotN, 0.0 };

					float vDotN = newVelocity[1] * groundN[1] * groundKd; // N only contributes in the y direction. kd(vDotN * N), kd=groundKd
					float* term2 = new float[3]{ 0.0, vDotN, 0.0 };

					// Finally, solve Fnormal
					float Fn = term1[1] - term2[1]; // x and z terms come out to 0.0, thus a single float is sufficient.

					FTotal[1] = FTotal[1] + Fn;
					// Solve and update velocity 
					newVelocity[0] = time * FTotal[0] / masses[i];
					newVelocity[1] = time * FTotal[1] / masses[i];
					newVelocity[2] = time * FTotal[2] / masses[i];

					// Solve and update position of particle
					currParticle[0] = currParticle[0] + newVelocity[0] * time;
					currParticle[1] = currParticle[1] + newVelocity[1] * time;
					currParticle[2] = currParticle[2] + newVelocity[2] * time;
					// If particle is moving down, place it on the ground plane
					if (currParticle[1] < groundP[1])
					{
						currParticle[1] = groundP[1];
					}
					// Append new velocity to list
					newV.push_back(newVelocity);
					// Append new position to list
					newP.push_back(currParticle);

				}
				else
				{
					// No collision
					if (strcmp(integration, "symplectic"))
					{
						currParticle[0] = currParticle[0] + newVelocity[0] * time;
						currParticle[1] = currParticle[1] + newVelocity[1] * time;
						currParticle[2] = currParticle[2] + newVelocity[2] * time;
					}
					else
					{
						currParticle[0] = currParticle[0] + currVelocity[0] * time;
						currParticle[1] = currParticle[1] + currVelocity[1] * time;
						currParticle[2] = currParticle[2] + currVelocity[2] * time;
					}
					// Append new position to list
					newP.push_back(currParticle);
					// Append new velocity to list
					newV.push_back(newVelocity);
				}
			}
		}
		else
		{
			float* particle = new float[3]{ p[i][0], p[i][1], p[i][2] };
			float* velocity = new float[3]{ v[i][0], v[i][1], v[i][2] };
			newP.push_back(particle);
			newV.push_back(velocity);
		}
	}

	mySystem->setState(newP);
	mySystem->setVel(newV);
	return 0;

}	// SpringSim::step

int SpringSim::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("simulator %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "link") == 0)
	{
		if (argc == 3)
		{

		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'link'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "spring") == 0)
	{
		if (argc == 6)
		{
			int index1 = std::stoi(argv[1]);
			int index2 = std::stoi(argv[2]);
			float ks = std::stof(argv[3]);;
			float kd = std::stof(argv[4]);;
			float restlength = std::stof(argv[5]);;

			mySystem->spring(index1, index2, ks, kd, restlength);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'spring'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "fix") == 0)
	{
		if (argc == 2)
		{
			int index = std::stoi(argv[1]);
			mySystem->nailParticle(index);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'fix'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "integration") == 0)
	{
		if (argc == 3)
		{
			strcpy(integration, argv[1]);
			myTime = std::stod(argv[2]);
		}
		else
		{
			animTcl::OutputMessage("simulator %s: wrong number of params for command 'integration'.", m_name.c_str());
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
} // SpringSim::command
