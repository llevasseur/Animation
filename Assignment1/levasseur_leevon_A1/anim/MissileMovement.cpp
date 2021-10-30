#include "MissileMovement.h"

MissileMovement::MissileMovement(const std::string& name, BaseSystem* target) :
	BaseSimulator(name),
	m_object(target)
{
	normalized_t = 10.0; // hardcode set time of simulation
}	// MissileMovement

MissileMovement::~MissileMovement()
{
}	// MissileMovement::~TankMovement

double MissileMovement::currNormal_t(double time)
{
	return 0.0;
} // MissileMovement::currNormal_t

int MissileMovement::step(double time)
{
	// Did not have enough time to set up the missles movement. I apply a small gravity motion to demostrate
	// that the reset works.
	double g = -1.8; //
	Vector pos;
	m_object->getState(pos);
	pos[0] = m_pos0[0] + m_vel0[0] * time;
	pos[1] = m_pos0[1] + m_vel0[1] * time + 0.5 * g * time * time;
	pos[2] = m_pos0[2] + m_vel0[2] * time;

	m_object->setState(pos);

	return 0;

}	// MissileMovement::step*