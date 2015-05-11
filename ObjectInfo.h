#ifndef _Object_H_
#define _Object_H_
#include "OGRE/Ogre.h"
#include <string>

namespace std{
	class ObjectInfo {
	public:
		ObjectInfo(Ogre::Vector3 position, const std::string& objName, Ogre::SceneNode *rootNode);
		std::string getObjectName();
		Ogre::Vector3 getPosition();
		Ogre::Node *getNode();

	private:
		Ogre::Vector3 position;
		std::string name;
		Ogre::Node *node;

	};

}
#endif
