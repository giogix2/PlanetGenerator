#ifndef _initOgre_H_
#define _initOgre_H_

#include "OGRE/Ogre.h"
#include "GeneratorFrameListener.h"

class initOgre{
 public:
	Ogre::Root              *Root;
	Ogre::SceneManager      *Scene;
	Ogre::SceneNode         *RootSceneNode;
	Ogre::RenderWindow      *Window;
	Ogre::Camera            *Camera;
	GeneratorFrameListener  *FrameListener;
	Ogre::OverlaySystem     *OverlaySystem;

	initOgre();

	int start();
	void CreateFrameListener();

};

#endif