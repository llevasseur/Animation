#include "Bezier.h"
#include "fstream"
#include <string>
#include <iostream>
#include <cmath>

Bezier::Bezier(const std::string& name) :
	BaseSystem(name)
{
	setVector(points[0], -4.104, 2.664, 0);
	setVector(pointTangents[0], 2.64, -2.112, 0);
	setVector(points[1], -0.336, 2.376, 0);
	setVector(pointTangents[1], 3.312, -1.8, 0);
	setVector(points[2], -0.792, 0.864, 0);
	setVector(pointTangents[2], -2.592, -2.184, 0);
	setVector(points[3], -2.928, 0.192, 0);
	setVector(pointTangents[3], -1.392, -1.968, 0);
	setVector(points[4], -2.184, -1.104, 0);

	numPoints = 4;
} // Bezier

void Bezier::reset(double time)
{

} // Bezier::reset

void Bezier::setTangent(int index, Vector tangent)
{
	setVector(pointTangents[index], tangent[0], tangent[1], tangent[2]);
	display(GL_RENDER);

	// Tell the system to redraw the window
	glutPostRedisplay();
} // Bezier::setTangent

void Bezier::setPoint(int index, Vector point)
{
	setVector(points[index], point[0], point[1], point[2]);
	display(GL_RENDER);

	// Tell the system to redraw the window
	glutPostRedisplay();
} // Bezier::setPoint

int Bezier::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
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
	else
	{
		return TCL_ERROR;
	}
} // Bezier::command

void Bezier::f(float t, Vector* f)
{
	// Initialize tmp holders
	Vector tmp_p_0 = { points[0][0], points[0][1], points[0][2] };
	Vector tmp_p_1 = { points[1][0], points[1][1], points[1][2] };
	Vector tmp_p_2 = { points[2][0], points[2][1], points[2][2] };
	Vector tmp_p_3 = { points[3][0], points[3][1], points[3][2] };

	// Determine coefficients
	float a = pow((1 - t), 3);
	float b = 3 * pow((1 - t), 2) * t;
	float c = 3 * (1 - t) * pow(t, 2);
	float d = pow(t, 3);

	//animTcl::OutputMessage("tmp_y_i before: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	//animTcl::OutputMessage("tmp_y_i1 before: %f %f %f", tmp_y_i1[0], tmp_y_i1[1], tmp_y_i1[2]);
	//animTcl::OutputMessage("tmp_s_i before: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	//animTcl::OutputMessage("tmp_s_i1 before: %f %f %f", tmp_s_i1[0], tmp_s_i1[1], tmp_s_i1[2]);
	//animTcl::OutputMessage("a %f", a);
	//animTcl::OutputMessage("b %f", b);
	//animTcl::OutputMessage("c %f", c);
	//animTcl::OutputMessage("d %f", d);

	// Apply coefficients
	VecScale(tmp_p_0, a);
	VecScale(tmp_p_1, b);
	VecScale(tmp_p_2, c);
	VecScale(tmp_p_3, d);

	//animTcl::OutputMessage("tmp_y_i after: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	//animTcl::OutputMessage("tmp_y_i1 after: %f %f %f", tmp_y_i1[0], tmp_y_i1[1], tmp_y_i1[2]);
	//animTcl::OutputMessage("tmp_s_i after: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	//animTcl::OutputMessage("tmp_s_i1 after: %f %f %f", tmp_s_i1[0], tmp_s_i1[1], tmp_s_i1[2]);

	// Add together and eventually solve for f(t)
	VecAdd(tmp_p_0, tmp_p_0, tmp_p_1);
	//animTcl::OutputMessage("tmp_y_i1 added to tmp_y_i: %f %f %f", tmp_y_i[0], tmp_y_i[1], tmp_y_i[2]);
	VecAdd(tmp_p_2, tmp_p_2, tmp_p_3);
	//animTcl::OutputMessage("tmp_s_i1 added to tmp_s_i: %f %f %f", tmp_s_i[0], tmp_s_i[1], tmp_s_i[2]);
	VecAdd(*f, tmp_p_0, tmp_p_2);
	//animTcl::OutputMessage("tmp_y_i added to tmp_s_i: %f %f %f", f[0], f[1], f[2]);
} // Bezier::f

void Bezier::display(GLenum mode)
{
	glEnable(GL_NORMALIZE);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i = 0; i < numPoints; i++)
	{
		animTcl::OutputMessage("plot point %f, %f, %f", points[i][0], points[i][1], points[i][2]);
		glVertex3dv(points[i]);
	}
	glEnd();

	Vector prevF = { points[0][0], points[0][1], points[0][2] };
	Vector F;

	glBegin(GL_LINE_STRIP);
	for (float i = 0.01; i <= 1; i += 0.01)
	{
		setVector(F, 0.0, 0.0, 0.0);
		f(i, &F);
		glVertex3dv(prevF);
		glVertex3dv(F);
		setVector(prevF, F[0], F[1], F[2]);
	}
	glEnd();
} // Bezier::display