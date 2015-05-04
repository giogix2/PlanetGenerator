#ifndef _Object_H_
#define _Object_H_
#include "OGRE/Ogre.h"
#include <string>

namespace std{
	class Object {
	public:
		Object(Ogre::Vector3 position, const std::string& objName, Ogre::SceneNode *rootNode);

	private:
		Ogre::Vector3 position;
		std::string name;
		Ogre::Node *node;

	};

}
#endif