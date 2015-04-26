/* Disclaimer: Come up with something here. */

#include <iostream>
#include <exception>

#include "initOgre.h"
#include "PSphere.h"
#include "Common.h"
#include "testui2/mainwindow.h"
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
	mySphere.create(15.0f, 0.6f, 100);

	initOgre rendering;

	rendering.start();
	rendering.setSceneAndRun(&mySphere);

	//Ogre::String source;
	//source = "C:\\Users\\giova\\Downloads\\Cube.mesh";
	//FILE* pFile = fopen( source.c_str(), "rb" );
	//if (!pFile)
	//	OGRE_EXCEPT(Exception::ERR_FILE_NOT_FOUND,"File " + source + " not found.", "OgreMeshLoaded");
	//struct stat tagStat;
	//stat( source.c_str(), &tagStat );
	//MemoryDataStream* memstream = new MemoryDataStream(source, tagStat.st_size, true);
	//fread( (void*)memstream->getPtr(), tagStat.st_size, 1, pFile );
	//fclose( pFile );
	//MeshPtr pMesh = MeshManager::getSingleton().createManual("LocalMesh",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	rendering.cleanup();

	mySphere.destroy();

	return 0;
}
