/* Disclaimer: Come up with something here. */

#include <iostream>
#include <exception>

#include "initOgre.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	initOgre rendering;

	rendering.start();
	rendering.setSceneAndRun();
	rendering.cleanup();

	return 0;
}
