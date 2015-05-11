#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<std::ObjectInfo> *Obs)
{
	Objects=Obs;
}
CheckCollisionAnswer CollisionManager::checkCollisionAABB(std::ObjectInfo object)
{
	CheckCollisionAnswer answer;
	answer.amount=0;
	answer.objectName=object.getObjectName();
	answer.collided=false;
	Ogre::AxisAlignedBox spbox = static_cast<Ogre::SceneNode*>(object.getNode())->_getWorldAABB();  
	for (std::vector<std::ObjectInfo>::iterator iter = Objects->begin(); iter != Objects->end(); iter++)
    {

		if(object.getObjectName() != iter->getObjectName() )
		{
			Ogre::AxisAlignedBox cbbox=static_cast<Ogre::SceneNode*>(iter->getNode())->_getWorldAABB(); 

			if(spbox.intersects(cbbox))//collided
			{
				answer.amount++;
				answer.collided=true;
				answer.collidedList.push_back(*iter);
			}
		}
    }
	return answer;
}

CheckCollisionAnswer CollisionManager::checkCameraCollision()
{
	CheckCollisionAnswer answer;
	answer.amount=0;
	answer.objectName="camera";
	answer.collided=false;

	//Ogre::AxisAlignedBox spbox = new Ogre::AxisAlignedBox(
	//for (std::vector<std::ObjectInfo>::iterator iter = Objects->begin(); iter != Objects->end(); iter++)
 //   {

	//	if(object.getObjectName() != iter->getObjectName() )
	//	{
	//		Ogre::AxisAlignedBox cbbox=static_cast<Ogre::SceneNode*>(iter->getNode())->_getWorldAABB(); 

	//		if(spbox.intersects(cbbox))//collided
	//		{
	//			answer.amount++;
	//			answer.collided=true;
	//			answer.collidedList.push_back(*iter);
	//		}
	//	}
 //   }
	return answer;
}
