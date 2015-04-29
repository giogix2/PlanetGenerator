/* Disclaimer: Come up with something here. */

#include <iostream>
#include <exception>

#include "initOgre.h"
#include "PSphere.h"
#include "Common.h"
#include "testui2/mainwindow.h"
#include <QApplication>
#include "ResourceParameter.h"

using namespace std;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

	vector<float> frequency;
	frequency.push_back(0.4f);
	frequency.push_back(0.06666f);
	vector <float> amplitude;
	amplitude.push_back(0.02f);
	amplitude.push_back(0.006666f);

	ResourceParameter resourceParameter = ResourceParameter((string)"#00FF00",(string)"#FACD00",(string)"#32CDFF"
		,(string)"#64FFFF",(string)"#B4B4B4",(string)"#FFFFFF",0.6f,7.5f,60,frequency,amplitude);
	PSphere mySphere;
	initOgre rendering;

	mySphere.create(100, resourceParameter);

	rendering.start();
	rendering.setSceneAndRun(&mySphere);
	rendering.cleanup();

	mySphere.destroy();

	return 0;
}
