#include "SpringMass.h"

SpringMass::SpringMass(const std::string& name) :
	BaseSystem(name)
{

} // SpringMass

void SpringMass::getState(vector<float*> p, vector<float*> v)
{
	p = vector<float*>(particles);
	v = velocities;
}	// ParticleSystem::getState

void SpringMass::setState(vector<float*> p)
{
	prevParticles = particles;
	particles = p;

}	// ParticleSystem::setState

void SpringMass::reset(double time)
{

} // SpringMass::reset

void SpringMass::setVel(vector<float*> v)
{
	velocities = v;
} // SpringMass::setVel

vector<float*> SpringMass::getParticles()
{
	return particles;
} // SpringMass::getParticles

vector<float*> SpringMass::getVelocities()
{
	return velocities;
} // SpringMass::getVelocities

vector<float*> SpringMass::getPrevParticles()
{
	return prevParticles;
} // SpringMass::getParticles

vector<vector<int>> SpringMass::getConnections()
{
	return myConnections;
} // SpringMass::getConnections

vector<vector<float*>> SpringMass::getSpringParams()
{
	return mySpringParams;
} // SpringMass::getSpringParams

vector<float> SpringMass::getMasses()
{
	return masses;
} // SpringMass::getMasses

bool* SpringMass::getNailed()
{
	return nailed;
} // SpringMass::getNailed

bool SpringMass::isNailed(int index)
{
	return nailed[index];
} // SpringMass::isNailded



void SpringMass::setSpringID(int index1, int index2)
{
	int* indexPair = new int[2];
	indexPair[0] = index1;
	indexPair[1] = index2;
	springs.push_back(indexPair);
} // SpringMass::setSpringID

void SpringMass::addConnection(int index1, int index2)
{
	vector<int> connections = myConnections.at(index1); // The initially empty list in myConnections[index1].
	connections.push_back(index2);
	myConnections[index1] = connections;
} // SpringMass::addConnection

void SpringMass::addSpringParams(int index, float ks, float kd, float restlength)
{
	vector<float*> params = mySpringParams.at(index); // The initially empty list in mySpringParams[index].
	float* currentParams = new float[3]{ ks, kd, restlength };
	params.push_back(currentParams);
	mySpringParams[index] = params;
} // SpringMass::addSpringParams

void SpringMass::spring(int index1, int index2, float ks, float kd, float restlength)
{
	// Set up spring
	if (restlength < 0)
	{
		// Set the restlength of the spring to the distance between the corresponding particles
		float* p1 = particles[index1];
		float* p2 = particles[index2];

		restlength = sqrt(pow(p2[0] - p1[0], 2) + pow(p2[1] - p1[1], 2) + pow(p2[2] - p1[2], 2));
	}

	setSpringID(index1, index2);
	addConnection(index1, index2);
	addConnection(index2, index1);
	addSpringParams(index1, ks, kd, restlength);
	addSpringParams(index2, ks, kd, restlength);
} // SpringMass::spring

void SpringMass::nailParticle(int index)
{
	nailed[index] = true;
} // SpringMass::nailParticle

int SpringMass::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "dim") == 0)
	{
		// Determine number of particles and initialize variable sizes accordingly.
		if (argc == 2)
		{
			numParticles = std::stoi(argv[1]);
			nailed = new bool[numParticles];

			for (int i = 0; i < numParticles; i++)
			{
				vector<int> emptyConnection;
				vector<float*> params;
				myConnections.push_back(emptyConnection);
				mySpringParams.push_back(params);
				nailed[i] = false;
			}
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'dim'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "particle") == 0)
	{
		// initialize a particle; index, mass, initial position and velocity. Add to list of positions and velocities.
		if (argc == 9)
		{
			int index = std::stoi(argv[1]);
			if (index > numParticles - 1)
			{
				animTcl::OutputMessage("system %s: index out of bounds in 'particle'.", m_name.c_str());
				return TCL_ERROR;
			}
			int mass = std::stof(argv[2]);
			masses.push_back(mass);

			float* p = new float[3]{ std::stof(argv[3]), std::stof(argv[4]), std::stof(argv[5]) };
			float* v = new float[3]{ std::stof(argv[6]),  std::stof(argv[7]),  std::stof(argv[8]) };

			particles.push_back(p);
			velocities.push_back(v);
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'particle'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "all_velocities") == 0)
	{
		// Set all velocities to specified velocity.
		if (argc == 4)
		{
			float* v = new float[3];
			v[0] = std::stof(argv[1]);
			v[1] = std::stof(argv[2]);
			v[2] = std::stof(argv[3]);

			for (int i = 0; i < numParticles; i++)
			{
				velocities[i] = v;
			}
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'all_velocities'.", m_name.c_str());
			return TCL_ERROR;
		}
	}

	glutPostRedisplay();
	return TCL_OK;
}	// SpringMass::command

void SpringMass::display(GLenum mode)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 0.0);
	// Draw Particles
	glBegin(GL_POINTS);
	for (int i = 0; i < particles.size(); i++)
	{
		glVertex3d(particles[i][0], particles[i][1], particles[i][2]);
	}
	glEnd();

	// Draw Springs (if they exist)
	if (springs.size() > 0)
	{
		for (int i = 0; i < springs.size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1.0, 1.0, 1.0);
			float* p1 = particles[springs[i][0]];
			float* p2 = particles[springs[i][1]];
			glVertex3d(p1[0], p1[1], p1[2]);
			glVertex3d(p2[0], p2[1], p2[2]);
			glEnd();
		}
	}
	
	// Draw Ground Plane
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3d(-100, -5, -100);
	glVertex3d(100, -5, -100);
	glVertex3d(100, -5, 100);
	glVertex3d(-100, -5, 100);

	glEnd();
}	// SampleParticle::display