////////////////////////////////////////////////////
// // Template code for  CS 174C
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <direct.h>
#include "fstream"

#include <shared/defs.h>

#include "shared/opengl.h"

#include <string.h>
#include <util/util.h>
#include <GLModel/GLModel.h>
#include "anim.h"
#include "animTcl.h"
#include "myScene.h"
#include "SampleParticle.h"
#include "SampleGravitySimulator.h"
#include "SampleForceSimulator.h"
#include "HermiteSpline.h"
#include "Tank.h"
#include "Missile.h"
#include "TankMovement.h"
#include "MissileMovement.h"
//#include <util/jama/tnt_stopwatch.h>
//#include <util/jama/jama_lu.h>

// register a sample variable with the shell.
// Available types are:
// - TCL_LINK_INT 
// - TCL_LINK_FLOAT

int g_testVariable = 10;

SETVAR myScriptVariables[] = {
	"testVariable", TCL_LINK_INT, (char *) &g_testVariable,
	"",0,(char *) NULL
};


//---------------------------------------------------------------------------------
//			Hooks that are called at appropriate places within anim.cpp
//---------------------------------------------------------------------------------

// start or end interaction
void myMouse(int button, int state, int x, int y)
{

	// let the global resource manager know about the new state of the mouse 
	// button
	GlobalResourceManager::use()->setMouseButtonInfo( button, state );

	if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		animTcl::OutputMessage(
			"My mouse received a mouse button press event\n");

	}
	if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
	{
		animTcl::OutputMessage(
			"My mouse received a mouse button release event\n") ;
	}
}	// myMouse

// interaction (mouse motion)
void myMotion(int x, int y)
{

	GLMouseButtonInfo updatedMouseButtonInfo = 
		GlobalResourceManager::use()->getMouseButtonInfo();

	if( updatedMouseButtonInfo.button == GLUT_LEFT_BUTTON )
	{
		animTcl::OutputMessage(
			"My mouse motion callback received a mousemotion event\n") ;
	}

}	// myMotion


void MakeScene(void)
{
}	// MakeScene

// OpenGL initialization
void myOpenGLInit(void)
{
	animTcl::OutputMessage("Initialization routine was called.");

}	// myOpenGLInit

void myIdleCB(void)
{
	
	return;

}	// myIdleCB

void myKey(unsigned char key, int x, int y)
{
	 animTcl::OutputMessage("My key callback received a key press event\n");
	return;

}	// myKey

static int testGlobalCommand(ClientData clientData, Tcl_Interp *interp, int argc, myCONST_SPEC char **argv)
{

	animTcl::OutputMessage("This is a test command!");
    animTcl::OutputResult("100") ;
	return TCL_OK;

}	// testGlobalCommand

static int part1GlobalCommand(ClientData clientData, Tcl_Interp* interp, int argc, myCONST_SPEC char** argv)
{
	bool success;

	HermiteSpline* hermite = new HermiteSpline("hermite");
	success = GlobalResourceManager::use()->addSystem(hermite, true);
	assert(success);

	hermite->setReset();
	animTcl::OutputMessage("Created HermiteSpline \"hermite\"");

	return TCL_OK;
} // part1GlobalCommand

static int part2GlobalCommand(ClientData clientData, Tcl_Interp* interp, int argc, myCONST_SPEC char** argv)
{
	bool success;

	HermiteSpline* tankpath = new HermiteSpline("tankpath");
	success = GlobalResourceManager::use()->addSystem(tankpath, true);
	assert(success);
	animTcl::OutputMessage("Created HermiteSpline \"tankpath\"");

	Tank* tank = new Tank("tank", "data\\tank.obj", tankpath);
	success = GlobalResourceManager::use()->addSystem(tank, true);
	assert(success);
	animTcl::OutputMessage("Created Tank \"tank\"");

	Missile* missile = new Missile("missile", "data\\missile.obj");
	success = GlobalResourceManager::use()->addSystem(missile, true);
	assert(success);
	animTcl::OutputMessage("Created Missile \"missile\"");

	TankMovement* tankMovement =
		new TankMovement( "tankMovement", tank, tankpath);
	success = GlobalResourceManager::use()->addSimulator(tankMovement);
	// make sure it was registered successfully
	assert( success );

	MissileMovement* missileMovement =
		new MissileMovement("missileMovement", missile);
	success = GlobalResourceManager::use()->addSimulator(missileMovement);
	assert(success);

	return TCL_OK;
} // part1GlobalCommand

void mySetScriptCommands(Tcl_Interp *interp)
{

	// here you can register additional generic (they do not belong to any object) 
	// commands with the shell

	Tcl_CreateCommand(interp, "test", testGlobalCommand, (ClientData) NULL,
					  (Tcl_CmdDeleteProc *)	NULL);

	Tcl_CreateCommand(interp, "part1", part1GlobalCommand, (ClientData)NULL,
		(Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "part2", part2GlobalCommand, (ClientData)NULL,
		(Tcl_CmdDeleteProc*)NULL);

}	// mySetScriptCommands