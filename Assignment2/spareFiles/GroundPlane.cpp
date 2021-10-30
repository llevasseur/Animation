#include "GroundPlane.h"

GroundPlane::GroundPlane(const std::string& name) :
	BaseSystem(name)
{
	//VecCopy(m_v1, v1);
	//VecCopy(m_v2, v2);
	//VecCopy(m_v3, v3);
	setVector(m_pos, 0, -10, 0);
} // GroundPlane

void GroundPlane::display(GLenum mode)
{
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslated(m_pos[0], m_pos[1], m_pos[2]);
	glutSolidCube(10.0);

	glPopMatrix();
	glPopAttrib();
}