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

	ResourceParameter resourceParameter = ResourceParameter((string)"#ffea00",(string)"#16e04f",(string)"#c116e0"
		,(string)"#e03c16",(string)"#177b73",(string)"#e50621",1.2f,2.2f,50,frequency,amplitude);
	PSphere mySphere;
	initOgre rendering;

	mySphere.create(15.0f, 0.6f, 100, resourceParameter);

	rendering.start();
	rendering.setSceneAndRun(&mySphere);
	rendering.cleanup();

	mySphere.destroy();

	return 0;
}
