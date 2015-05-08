#ifndef _initOgre_H_
#define _initOgre_H_

#include "OGRE/Ogre.h"
#include "GeneratorFrameListener.h"
#include "PSphere.h"
#include <vector>

class initOgre{
 public:
	initOgre();
	~initOgre();

	int start();
	void setSceneAndRun(PSphere *planet);
	void cleanup();
private:
	Ogre::Root              *Root;
	Ogre::SceneManager      *Scene;
	Ogre::SceneNode         *RootSceneNode;
	Ogre::RenderWindow      *Window;
	Ogre::Camera            *Camera;
	GeneratorFrameListener  *FrameListener;
	Ogre::OverlaySystem     *OverlaySystem;

	void CreateFrameListener(PSphere *pSphere);
};

#endif
