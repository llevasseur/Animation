#include "HermiteSpline.h"
#include "fstream"
#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <regex>

HermiteSpline::HermiteSpline(const std::string& name) :
	BaseSystem(name)
{
	
} // HermiteSpline

void HermiteSpline::reset(double time)
{
	
} // HermiteSpline::reset

void HermiteSpline::add(Vector point, Vector tangent)
{
	setVector(points[numPoints], point[0], point[1], point[2]);
	setVector(pointTangents[numPoints], tangent[0], tangent[1], tangent[2]);
	animTcl::OutputMessage("testing %f, %f, %f", points[numPoints][0], points[numPoints][1], points[numPoints][2]);
	numPoints += 1;
	display(GL_RENDER);

	//Remake the lookup table
	piecewiseApprox();

	// Tell the system to redraw the window
	glutPostRedisplay();
} // HermiteSpline::add

void HermiteSpline::setTangent(int index, Vector tangent)
{
	setVector(pointTangents[index], tangent[0], tangent[1], tangent[2]);
	display(GL_RENDER);

	//Remake the lookup table
	piecewiseApprox();

	// Tell the system to redraw the window
	glutPostRedisplay();
} // HermiteSpline::setTangent

void HermiteSpline::setPoint(int index, Vector point)
{
	setVector(points[index], point[0], point[1], point[2]);
	display(GL_RENDER);

	//Remake the lookup table
	piecewiseApprox();

	// Tell the system to redraw the window
	glutPostRedisplay();
} // HermiteSpline::setPoint

void HermiteSpline::piecewiseApprox()
{
	Vector tmp;
	s[0] = 0.0;
	for (int i = 0; i < numPoints; i++)
	{
		for (int j = 1; j < 100; j++)
		{
			VecSubtract(tmp, u[(i * 100) + j], u[((i * 100) + j) - 1]);
			s[(i * 100) + j] = VecLength(tmp) + s[((i * 100) + j) - 1];
		}
	}
} // HermiteSpline::piecewiseApprox

/**
 * @brief Tokenize the given vector
   according to the regex
 * and remove the empty tokens.
 *
 * @param str
 * @param re
 * @return std::vector<std::string>
 */
void HermiteSpline::tokenize(std::string str, std::vector<std::string>& out)
{
	const std::regex re(R"([\s|,]+)");
	std::sregex_token_iterator it{ str.begin(), str.end(), re, -1 };
	std::vector<std::string> tokenized{ it, {} };

	// Additional check to remove empty strings
	tokenized.erase(
		std::remove_if(tokenized.begin(),
			tokenized.end(),
			[](std::string const& s) {
				return s.size() == 0;
			}),
		tokenized.end());

	out = tokenized;
}

void HermiteSpline::loadPoints(float values[6], int index)
{
	animTcl::OutputMessage("value[0] %f", values[0]);
	animTcl::OutputMessage("index %d", index);
	setVector(points[index], values[0], values[1], values[2]);
	setVector(pointTangents[index], values[3], values[4], values[5]);
	animTcl::OutputMessage("points[0][0] load %f", points[0][0]);
}

int HermiteSpline::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "add") == 0)
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "point") == 0)
			{
				if (argc == 8)
				{
					if (numPoints > 39)
					{
						animTcl::OutputMessage("Usage: cannot add more than 40 control points.");
						return TCL_ERROR;
					}
					Vector point;
					Vector tanget;
					setVector(point, atof(argv[2]), atof(argv[3]), atof(argv[4]));
					setVector(tanget, atof(argv[5]), atof(argv[6]), atof(argv[7]));
					add(point, tanget);
					return TCL_OK;
				}
				else
				{
					animTcl::OutputMessage("Usage: not enough values for point and tangent.");
					return TCL_ERROR;
				}
			}
			else
			{
				animTcl::OutputMessage("Usage: missing 'point' after 'add'.");
				return TCL_ERROR;
			}
		}
		else
		{
			animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
			return TCL_ERROR;
		}
	} // add command
	else if (strcmp(argv[0], "set") == 0)
	{
		if (argc > 1)
		{
			if (argc == 6)
			{
				if (atoi(argv[2]) <= numPoints || atoi(argv[2]) > 40)
				{
					if (strcmp(argv[1], "point") == 0)
					{
						Vector point;
						setVector(point, atof(argv[3]), atof(argv[4]), atof(argv[5]));
						setPoint(atoi(argv[2]), point);
						return TCL_OK;
					}
					else if (strcmp(argv[1], "tangent") == 0)
					{
						Vector tangent;
						setVector(tangent, atof(argv[3]), atof(argv[4]), atof(argv[5]));
						setTangent(atoi(argv[2]), tangent);
						return TCL_OK;
					}
				}
				else
				{
					animTcl::OutputMessage("Usage: index out of range '%d'.", atoi(argv[2]));
					return TCL_ERROR;
				}
			}
			else
			{
				animTcl::OutputMessage("Usage: not enough values given.");
				return TCL_ERROR;
			}
		}
		else
		{
			animTcl::OutputMessage("Usage: missing 'point' or 'tangent' after 'set'.");
			return TCL_ERROR;
		}
	} // set command
	else if (strcmp(argv[0], "load") == 0)
	{
		if (argc > 1)
		{
			// Take a file, read it and send the data to 
			std::string line;
			std::ifstream inFile(argv[1]);
			std::string str;
			char split_char = ' ';
			int count = 0;
			//float values[6];

			std::vector<std::string> out;

			animTcl::OutputMessage("Open file: %s.", argv[1]);
			if (!inFile)
			{
				animTcl::OutputMessage("Unable to open file: %s.", argv[1]);
				return TCL_ERROR;
			}
			//for (std::string each; std::getline(split, each, split_char); tokens.push_back(each));
			while (std::getline(inFile, str))
			{
				//values = std::split(line, split_char);
				tokenize(str, out);
				if (count == 0)
				{
					int i = 0;
					for (std::string token : out)
					{
						if (i == 1)
						{
							numPoints = stoi(token);
							animTcl::OutputMessage("File n: %d", numPoints);
						}
						i++;
					}
				}
				else
				{
					float values[6];
					int i = 0;
					for (std::string token : out)
					{
						values[i] = stod(token);
						animTcl::OutputMessage("File contents in loop: %f", values[i]);
						i++;
					}
					loadPoints(values, count - 1);
				}
				count++;
			}
			inFile.close();

			animTcl::OutputMessage("points[0] %f", points[0][0]);
			//Remake the lookup table
			piecewiseApprox();

			// Tell the system to redraw the window
			glutPostRedisplay();
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: not enough values given.");
			return TCL_ERROR;
		}
	} // load command
	else if (strcmp(argv[0], "export") == 0)
	{
		if (argc > 1)
		{
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: not enough values given.");
			return TCL_ERROR;
		}
	} // export command
	else if (strcmp(argv[0], "cr") == 0)
	{
		// Initialize the pointTangents to be Catmull-Rom with second-order accurate boundary conditions
		Vector dy;
		Vector dy_2;
		VecSubtract(dy, points[1], points[0]);
		VecScale(dy, 2);

		VecSubtract(dy_2, points[2], points[0]);
		VecScale(dy_2, 0.5);

		VecSubtract(dy, dy, dy_2);
		setVector(pointTangents[0], dy[0], dy[1], dy[2]);
		for (int i = 1; i < numPoints - 2; i++)
		{
			VecSubtract(dy, points[i + 1], points[i - 1]);
			VecScale(dy, 0.5);
			setVector(pointTangents[i], dy[0], dy[1], dy[2]);
		}

		VecSubtract(dy, points[numPoints - 1], points[numPoints - 2]);
		VecScale(dy, 2);

		VecSubtract(dy_2, points[numPoints - 1], points[numPoints - 3]);
		VecScale(dy_2, 0.5);

		VecSubtract(dy, dy, dy_2);
		setVector(pointTangents[numPoints - 1], dy[0], dy[1], dy[2]);

		// Tell the system to redraw the window
		glutPostRedisplay();
		return TCL_OK;

	} // cr command
	else if (strcmp(argv[0], "getArcLength") == 0)
	{
		if (argc == 2)
		{
			float t = atof(argv[1]);
			int i = (int)(t * numPoints + 0.5);

			// Index out of range. lookup table is listed from [0, numPoints-1]
			// with s[numPoints-1] being the arc length of entire curve.
			if (i >= numPoints)
			{
				i = numPoints - 1;
			}

			animTcl::OutputMessage("(Note i=%d) Arc Length at t=%f: %f", i, t, s[i]);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: incorrect number of values given.");
			return TCL_ERROR;
		}
	} // getArcLength command

	else if (strcmp(argv[0], "print") == 0)
	{
		if (argc == 2)
		{
			animTcl::OutputMessage("Hello World!");
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: print <file_name>");
			return TCL_ERROR;
		}
	} // print command
	else
	{
		return TCL_ERROR;
	}
} // HermiteSpline::command

void HermiteSpline::f(float t, float ti, float ti_1, Vector y_i, Vector y_i1, Vector s_i, Vector s_i1, Vector* f)
{
	// Initialize tmp holders
	Vector tmp_y_i = { y_i[0], y_i[1], y_i[2] };
	Vector tmp_y_i1 = { y_i1[0], y_i1[1], y_i1[2] };
	Vector tmp_s_i = { s_i[0], s_i[1], s_i[2] };
	Vector tmp_s_i1 = { s_i1[0], s_i1[1], s_i1[2] };

	// Determine coefficients
	float dt = ti_1 - ti;
	float tmp_dt = t - ti;
	float a = (2 * pow(tmp_dt, 3) / pow(dt, 3)) - (3 * pow(tmp_dt, 2) / pow(dt, 2)) + 1;
	float b = (-2 * pow(tmp_dt, 3) / pow(dt, 3)) + (3 * pow(tmp_dt, 2) / pow(dt, 2));
	float c = (pow(tmp_dt, 3) / pow(dt, 2)) - (2 * pow(tmp_dt, 2) / pow(dt, 2)) + tmp_dt;
	float d = (pow(tmp_dt, 3) / pow(dt, 2)) - (pow(tmp_dt, 2) / dt);

	//animTcl::OutputMessage("tmp_y_i before: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	//animTcl::OutputMessage("tmp_y_i1 before: %f %f %f", tmp_y_i1[0], tmp_y_i1[1], tmp_y_i1[2]);
	//animTcl::OutputMessage("tmp_s_i before: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	//animTcl::OutputMessage("tmp_s_i1 before: %f %f %f", tmp_s_i1[0], tmp_s_i1[1], tmp_s_i1[2]);
	//animTcl::OutputMessage("a %f", a);
	//animTcl::OutputMessage("b %f", b);
	//animTcl::OutputMessage("c %f", c);
	//animTcl::OutputMessage("d %f", d);

	// Apply coefficients
	VecScale(tmp_y_i, a);
	VecScale(tmp_y_i1, b);
	VecScale(tmp_s_i, c);
	VecScale(tmp_s_i1, d);

	//animTcl::OutputMessage("tmp_y_i after: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	//animTcl::OutputMessage("tmp_y_i1 after: %f %f %f", tmp_y_i1[0], tmp_y_i1[1], tmp_y_i1[2]);
	//animTcl::OutputMessage("tmp_s_i after: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	//animTcl::OutputMessage("tmp_s_i1 after: %f %f %f", tmp_s_i1[0], tmp_s_i1[1], tmp_s_i1[2]);

	// Add together and eventually solve for f(t)
	VecAdd(tmp_y_i, tmp_y_i, tmp_y_i1);
	//animTcl::OutputMessage("tmp_y_i1 added to tmp_y_i: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	VecAdd(tmp_s_i, tmp_s_i, tmp_s_i1);
	//animTcl::OutputMessage("tmp_s_i1 added to tmp_s_i: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	VecAdd(*f, tmp_y_i, tmp_s_i);
	//animTcl::OutputMessage("tmp_y_i added to tmp_s_i: %f %f %f", f[0], f[1], f[2]);
} // HermiteSpline::f

void HermiteSpline::display(GLenum mode)
{
	glEnable(GL_NORMALIZE);
	glPointSize(10);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	animTcl::OutputMessage("%d", numPoints);
	for (int i = 0; i < numPoints; i++)
	{
		animTcl::OutputMessage("hello %f, %f, %f", points[i][0], points[i][1], points[i][2]);
		glVertex3dv(points[i]);
	}
	glEnd();

	setVector(u[0], points[0][0], points[0][1], points[0][2]);
	Vector prevU = { u[0][0], u[0][1], u[0][2] };
	float tmp_i = 0.0;
	int tmp_j = 0;

	glBegin(GL_LINE_STRIP);
	for (int i = 1; i < numPoints; i++)
	{
		//setVector(prevF, points[i - 1][0], points[i - 1][1], points[i - 1][2]);
		for (float j = 0.0; j < 1.0; j+=0.01)
		{
			tmp_i = (float)i;
			tmp_j = (int)j * 100;
			setVector(u[(i - 1) * 100 + tmp_j], 0.0, 0.0, 0.0);
			f(j + tmp_i - 1.0, tmp_i - 1.0, tmp_i, points[i - 1], points[i], pointTangents[i - 1], pointTangents[i], &u[(i - 1) * 100 + tmp_j]);
			//animTcl::OutputMessage("Test %f, %f, %f", F[0], F[1], F[2]);
			glVertex3dv(prevU);
			glVertex3dv(u[(i - 1) * 100 + tmp_j]);
			setVector(prevU, u[(i - 1) * 100 + tmp_j][0], u[(i - 1) * 100 + tmp_j][1], u[(i - 1) * 100 + tmp_j][2]);
		}
	}
	glVertex3dv(prevU);
	glVertex3dv(points[numPoints - 1]);
	glEnd();
} // HermiteSpline::display