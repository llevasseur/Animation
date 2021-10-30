#include "TankMovement.h"

TankMovement::TankMovement(const std::string& name, BaseSystem* target, HermiteSpline* givenTankPath) :
	BaseSimulator(name),
	m_object(target)
{
	tankpath = givenTankPath;
	normalized_t = 10.0; // hardcode set time of simulation
}	// TankMovement

TankMovement::~TankMovement()
{
}	// TankMovement::~TankMovement

double TankMovement::currNormal_t(double time)
{
	return 0.0;
} // TankMovement::currNormal_t

int TankMovement::step(double time)
{
	// Set position based on time
	// Choose a length of time that you want the whole simulation to take, say 30 seconds.
	// normalize the input time to the desired time, so t exists in [0, 1]

	normalized_t = time / fullTime;

	if (normalized_t > 1) {
		normalized_t = 0; // stop moving, past 10s
	}

	// Use this normalized t to determine i in lookup table, get s[i]
	int i = tankpath->get_i(normalized_t);
	double s = tankpath->arcLength(normalized_t);
	Vector* u = tankpath->get_u(i); // Get u based on which s we are at, problem is s is not linear so speed is wrong.
	// the tank spends the same amount of time between each point.

	// Ran out of time to implement this properly, but I need u in terms of s... at least there is motion.

	Vector pos;
	m_object->getState(pos);

	VecCopy(pos, *u);
	m_object->setState(pos);

	return 0;

}	// TankMovement::step