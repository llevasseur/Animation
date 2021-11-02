#include "Globules.h"

Globules::Globules(const std::string& name) :
	BaseSystem(name)
{

} // Globules

void Globules::getState(vector<float*> p, vector<float*> v)
{
	p = vector<float*>(globules);
	v = velocities;
}	// Globules::getState

void Globules::setState(vector<float*> p)
{
	prevGlobules = globules;
	globules = p;

}	// Globules::setState

void Globules::reset(double time)
{

} // Globules::reset

void Globules::setVel(vector<float*> v)
{
	velocities = v;
} // Globules::setVel

vector<float*> Globules::getGlobules()
{
	return globules;
} // Globules::getParticles

vector<float*> Globules::getVelocities()
{
	return velocities;
} // Globules::getVelocities

vector<float*> Globules::getPrevGlobules()
{
	return prevGlobules;
} // Globules::getParticles

vector<float> Globules::getMasses()
{
	return masses;
} // Globules::getMasses

vector<float> Globules::getRadii()
{
	return radii;
} // Globules::getRadii

float Globules::getR0()
{
	return r0;
} // Globules::getR0()


int Globules::command(int argc, myCONST_SPEC char** argv)
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
			numGlobules = std::stoi(argv[1]);
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'dim'.", m_name.c_str());
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "globule") == 0)
	{
		// initialize a particle; index, mass, initial position and velocity. Add to list of positions and velocities.
		if (argc == 10)
		{
			animTcl::OutputMessage("numGLOBS:", numGlobules);
			int index = std::stoi(argv[1]);
			if (index > numGlobules - 1)
			{
				animTcl::OutputMessage("system %s: index out of bounds in 'globule'.", m_name.c_str());
				return TCL_ERROR;
			}
			float mass = std::stof(argv[2]);
			masses.push_back(mass);

			float radius = std::stof(argv[3]);
			radii.push_back(radius);

			float* p = new float[3]{ std::stof(argv[4]), std::stof(argv[5]), std::stof(argv[6]) };
			float* v = new float[3]{ std::stof(argv[7]),  std::stof(argv[8]),  std::stof(argv[9]) };

			globules.push_back(p);
			velocities.push_back(v);
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'globule'.", m_name.c_str());
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

			for (int i = 0; i < numGlobules; i++)
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
	else if (strcmp(argv[0], "r0") == 0)
	{
		// Set all velocities to specified velocity.
		if (argc == 2)
		{
			r0 = std::stof(argv[1]);
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params for command 'r0'.", m_name.c_str());
			return TCL_ERROR;
		}
	}

	glutPostRedisplay();
	return TCL_OK;
}	// Globules::command

void Globules::display(GLenum mode)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 0.0);
	// Draw Particles
	animTcl::OutputMessage("globules.size():", globules.size());
	glBegin(GL_POLYGON);
	for (int i = 0; i < globules.size(); i++)
	{
		glTranslatef(globules[i][0], globules[i][1], globules[i][2]);
		animTcl::OutputMessage("GLOB Y:", globules[i][1]);
		glutSolidSphere(radii[i], 20, 20);
	}
	glEnd();
	

	// Draw Ground Plane
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3d(-100, -5, -100);
	glVertex3d(100, -5, -100);
	glVertex3d(100, -5, 100);
	glVertex3d(-100, -5, 100);

	glEnd();
}	// Globules::display