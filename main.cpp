/* Disclaimer: Come up with something here. */

#include <iostream>
#include <exception>

#include "initOgre.h"
#include "PSphere.h"
#include "Common.h"
#include "mainwindow.h"
#include <QtWidgets\QApplication>


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif


//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
//#else
int main(int argc, char *argv[])
//#endif
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
	Ogre::Vector3 test = convertSphericalToCartesian(50.0, 50.0);
	PSphere mySphere;
	mySphere.create(15.0f, 0.0f, 100);

	initOgre rendering;

	rendering.start();
	rendering.setSceneAndRun(&mySphere);

	rendering.cleanup();

	mySphere.destroy();

	return 0;
}
