#include <iostream>
#include <exception>
#include <string>
#include "OGRE/Ogre.h"
#include "Object.h"

namespace std {

	Object::Object(Ogre::Vector3 pos, const string& objName, Ogre::SceneNode *rootNode) 
	{
		position = pos;
		name = objName;
		node = rootNode->getChild(objName);
	}


}