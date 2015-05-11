#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <Ogre.h>
#include "ObjectInfo.h"
#include <vector>

struct CheckCollisionAnswer
{
	bool				collided;
	std::string			objectName;
	int					amount; //how many object have collided with this one
	std::vector<std::ObjectInfo>	collidedList;
};

class CollisionManager
{
public:
	CollisionManager(std::vector<std::ObjectInfo> *Obs,Ogre::Camera *cam);

	std::vector<std::ObjectInfo> *Objects;
	
	CheckCollisionAnswer checkCollisionAABB(std::ObjectInfo object);

	CheckCollisionAnswer checkCameraCollision();

private:
	Ogre::Camera            *camera;
};

#endif