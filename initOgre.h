#ifndef _initOgre_H_
#define _initOgre_H_

#include "OGRE/Ogre.h"
#include "GeneratorFrameListener.h"
#include "PSphere.h"

<<<<<<< HEAD



=======
>>>>>>> 062accafceac2949391be7cd92fb89409a4a9010
class initOgre{
 public:
	initOgre();

	int start();
	void setSceneAndRun(PSphere *planet);
	void cleanup();
private:
<<<<<<< HEAD
	Ogre::Root				*Root;
	Ogre::SceneManager		*Scene;
	Ogre::SceneNode			*RootSceneNode;
	Ogre::RenderWindow		*Window;
	Ogre::Camera			*Camera;
	GeneratorFrameListener	*FrameListener;
	Ogre::OverlaySystem		*OverlaySystem;

	
=======
	Ogre::Root              *Root;
	Ogre::SceneManager      *Scene;
	Ogre::SceneNode         *RootSceneNode;
	Ogre::RenderWindow      *Window;
	Ogre::Camera            *Camera;
	GeneratorFrameListener  *FrameListener;
	Ogre::OverlaySystem     *OverlaySystem;
>>>>>>> 062accafceac2949391be7cd92fb89409a4a9010

	void CreateFrameListener();
};

#endif
