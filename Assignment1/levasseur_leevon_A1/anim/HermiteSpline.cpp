#include "HermiteSpline.h"
#include "fstream"
#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <regex>
#include <direct.h>

HermiteSpline::HermiteSpline(const std::string& name) :
	BaseSystem(name)
{
	hermiteName = name;
	clearOnReset = False;
} // HermiteSpline

void HermiteSpline::reset(double time)
{
	if (clearOnReset)
	{
		numPoints = 0;
		display(GL_RENDER);
	}
} // HermiteSpline::reset

void HermiteSpline::setReset()
{
	clearOnReset = True;
} // HermiteSpline::setReset

void HermiteSpline::setPoint(int index, Vector point)
{
	VecCopy(points[index], point);

	remake();
} // HermiteSpline::setPoint

void HermiteSpline::setTangent(int index, Vector tangent)
{
	VecCopy(pointTangents[index], tangent);

	remake();
} // HermiteSpline::setTangent

void HermiteSpline::add(Vector point, Vector tangent)
{
	VecCopy(points[numPoints], point);
	VecCopy(pointTangents[numPoints], tangent);
	numPoints += 1;

	remake();
} // HermiteSpline::add

Vector* HermiteSpline::getPoints()
{
	return points;
}

// Source: https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
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
} // HermiteSpline::tokenize

// Source: https://stackoverflow.com/questions/5254951/how-to-put-two-backslash-in-c
std::string HermiteSpline::stripPath(std::string path)
{
	for (int i = 0; path[i] != '\0'; i++)
	{
		if (path[i] == '\\')
		{
			path[i] += '\\';
			i++;
		}
	}
	return path;
} // HermiteSpline::stripPath

void HermiteSpline::loadPoints(float values[6], int index)
{
	setVector(points[index], values[0], values[1], values[2]);
	setVector(pointTangents[index], values[3], values[4], values[5]);
} // HermiteSpline::loadPoints

int HermiteSpline::loadFile(std::string filename)
{
	// Take a file, read it and send the data to 
	std::string line;
	std::ifstream inFile(filename);
	std::vector<std::string> out;
	int count = 0;

	if (!inFile)
	{
		// Test if this is an error with missing double '\'

		char* cwd = _getcwd(0, 0); // **** microsoft specific ****
		std::string working_directory(cwd);
		std::free(cwd);

		std::string cwdFile = working_directory + '\\' + filename;
		std::ifstream inFile(cwdFile);
		//inFile.open(stripPath(filename));
		if (!inFile)
		{
			// Syntax to output <filename> in animTcl::OutputMessage()
			std::string errorText = "Unable to open file: " + filename + "\nAlso tried cwd\file: " + cwdFile;
			char* charErrorText = new char[errorText.size() + 1];
			std::copy(errorText.begin(), errorText.end(), charErrorText);
			charErrorText[errorText.size()] = '\0';

			animTcl::OutputMessage(charErrorText);
			animTcl::OutputMessage("Make sure that the file you wish to load is in \\Build");
			delete[] charErrorText;
			return TCL_ERROR;
		}
	}

	while (std::getline(inFile, line))
	{
		tokenize(line, out);
		if (count == 0)
		{
			// Find number of control points
			int i = 0;
			for (std::string token : out)
			{
				if (i == 1)
				{
					numPoints = stoi(token);
					if (numPoints > 40)
					{
						animTcl::OutputMessage("Error: Number of control points out of range (%d).", numPoints);
						return TCL_ERROR;
					}
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
				i++;
			}
			loadPoints(values, count - 1);
		}
		count++;
	}
	inFile.close();
	remake();
	return TCL_OK;
} // HermiteSpline::loadFile

int HermiteSpline::exportFile(std::string filename)
{
	// Take a file, read it and send the data to 
	std::ofstream outFile(filename);

	for (int i = 0; i < numPoints; i++)
	{
		std::string line1;
		std::string line2;
		if (i == 0)
		{
			std::string line3;
			line3 = hermiteName + ' ' + std::to_string(numPoints);
			outFile << line3 << std::endl;
		}
		for (int j = 0; j < 3; j++)
		{
			std::string strPoint = std::to_string(points[i][j]);
			std::string strTangent = std::to_string(pointTangents[i][j]);
			line1 += strPoint.erase(strPoint.find_last_not_of('0') + 1, std::string::npos) + ' ';
			line2 += strTangent.erase(strTangent.find_last_not_of('0'), std::string::npos) + ' ';
			if (j < 2)
			{
				line2 += ' ';
			}
		}
		line1 += line2;
		outFile << line1 << std::endl;
	}
	outFile.close();
	// Syntax to output <filename> in animTcl::OutputMessage()
	std::string success = "Successfully exported spline to " + filename;
	char* charSuccess = new char[success.size() + 1];
	std::copy(success.begin(), success.end(), charSuccess);
	charSuccess[success.size()] = '\0';

	animTcl::OutputMessage(charSuccess);
	return TCL_OK;

} // HermiteSpline::exportFile

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
	float c0 = (2 * pow(tmp_dt, 3) / pow(dt, 3)) - (3 * pow(tmp_dt, 2) / pow(dt, 2)) + 1;
	float c1 = (-2 * pow(tmp_dt, 3) / pow(dt, 3)) + (3 * pow(tmp_dt, 2) / pow(dt, 2));
	float c2 = (pow(tmp_dt, 3) / pow(dt, 2)) - (2 * pow(tmp_dt, 2) / pow(dt, 2)) + tmp_dt;
	float c3 = (pow(tmp_dt, 3) / pow(dt, 2)) - (pow(tmp_dt, 2) / dt);


	// Apply coefficients
	VecScale(tmp_y_i, c0);
	VecScale(tmp_y_i1, c1);
	VecScale(tmp_s_i, c2);
	VecScale(tmp_s_i1, c3);

	// Add together and eventually solve for f(t)
	VecAdd(tmp_y_i, tmp_y_i, tmp_y_i1);
	VecAdd(tmp_s_i, tmp_s_i, tmp_s_i1);
	VecAdd(*f, tmp_y_i, tmp_s_i);

} // HermiteSpline::f

void HermiteSpline::piecewiseApprox()
{
	Vector tmp;
	s[0] = 0.0;
	int index = 0;
	for (int i = 0; i < numPoints; i++)
	{
		for (int j = 1; j <= 100; j++)
		{
			index = (i * 100) + j;
			VecSubtract(tmp, u[index], u[index - 1]);
			double value = VecLength(tmp) + s[index - 1];
			s[index] = value;
		}
	}
} // HermiteSpline::piecewiseApprox

void HermiteSpline::catmullRom()
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

	remake();
} // HermiteSpline::catmullRom

int HermiteSpline::get_i(double t)
{
	int i = (int)(t * (numPoints - 1.0) * 100 - 1.0 + 0.5);

	// Index out of range. lookup table is listed from [0, numPoints-1]
	// with s[(numPoints-1) * 100] being the arc length of entire curve.
	if (i >= (numPoints - 1.0) * 100 - 1.0)
	{
		i = (numPoints - 1.0) * 100 - 1.0;
	}
	return i;
} // HermiteSpline::get_i

double HermiteSpline::arcLength(double t)
{
	return s[get_i(t)];
} // HermiteSpline::arcLength

Vector* HermiteSpline::get_u(int i)
{
	return &u[i];
} // HermiteSpline::get_u

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
				if (atoi(argv[2]) < numPoints || atoi(argv[2]) > 40)
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
			return loadFile(argv[1]);
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
			return exportFile(argv[1]);
		}
		else
		{
			animTcl::OutputMessage("Usage: not enough values given.");
			return TCL_ERROR;
		}
	} // export command
	else if (strcmp(argv[0], "cr") == 0)
	{
		catmullRom();
		return TCL_OK;
	} // cr command
	else if (strcmp(argv[0], "getArcLength") == 0)
	{
		if (argc == 2)
		{
			double t = atof(argv[1]);
			animTcl::OutputMessage("(Note i=%d) Arc Length, s, at t=%f: %f", get_i(t), t, arcLength(t));
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: incorrect number of values given.");
			return TCL_ERROR;
		}
	} // getArcLength command
	else
	{
		animTcl::OutputMessage("Error: unknown command given.");
		return TCL_ERROR;
	}
} // HermiteSpline::command

void HermiteSpline::display(GLenum mode)
{
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(10);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);

	for (int i = 0; i < numPoints; i++)
	{
		glVertex3dv(points[i]);
	}
	glEnd();

	VecCopy(u[0], points[0]);
	Vector prevU = { u[0][0], u[0][1], u[0][2] };
	float tmp_i = 0.0;
	int tmp_j = 0;
	float ti = 0.0;
	float t = 0.0;
	float ti_1 = 0.0;

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 1; i < numPoints; i++)
	{
		for (int j = 1; j <= 100; j++)
		{
			tmp_i = (float)i;
			tmp_j = (float)j;
			ti = tmp_i - 1.0;
			t = tmp_j/100.0 + ti;
			ti_1 = tmp_i;
			
			int index = (i - 1) * 100 + j;
			setVector(u[index], 0.0, 0.0, 0.0); // initialize new u
			f(t, ti, ti_1, points[i - 1], points[i], pointTangents[i - 1], pointTangents[i], &u[index]);

			glVertex3dv(prevU);
			glVertex3dv(u[index]);
			VecCopy(prevU, u[index]);
		}
	}
	glVertex3dv(prevU);
	glVertex3dv(points[numPoints - 1]);
	glEnd();

	glPopMatrix();
	glPopAttrib();
} // HermiteSpline::display

void HermiteSpline::remake()
{
	// Redraw
	display(GL_RENDER);

	//Remake the lookup table
	piecewiseApprox();

	// Tell the system to redraw the window
	glutPostRedisplay();

} // HermiteSpline::remake