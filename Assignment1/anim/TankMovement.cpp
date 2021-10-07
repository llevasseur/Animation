#include "TankMovement.h"

TankMovement::TankMovement(const std::string& name, BaseSystem* target) :
	BaseSimulator(name),
	m_object(target)
{
}	// TankMovement

TankMovement::~TankMovement()
{
}	// TankMovement::~TankMovement

int TankMovement::step(double time)
{
	double g = -1.8;
	Vector pos;
	m_object->getState(pos);
	pos[0] = m_pos0[0] + 2.0 * cos(3 * time);
	pos[1] = m_pos0[1] + 2.0 * sin(3 * time) + m_vel0[1] * time + 0.5 * g * time * time;
	//pos[2] = m_pos0[2] + m_vel0[2]*time;

 //   pos[1]= 2 * sin(2*3.14*time) ;

	m_object->setState(pos);

	return 0;

}	// TankMovement::step*