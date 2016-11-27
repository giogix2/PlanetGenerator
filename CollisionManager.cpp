/* The MIT License (MIT)
 * 
 * Copyright (c) 2015 Giovanni Ortolani, Taneli Mikkonen, Pingjiang Li, Tommi Puolamaa, Mitra Vahida
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. */

#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<std::ObjectInfo> *Obs, Ogre::Camera *cam)
{
	Objects=Obs;
	camera=cam;	
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
	Ogre::AxisAlignedBox *spbox = new Ogre::AxisAlignedBox(	camera->getPosition().x-0.1f,camera->getPosition().y-0.1f,camera->getPosition().z-0.1f,
																camera->getPosition().x+0.1f,camera->getPosition().y+0.1f,camera->getPosition().z+0.1f);
	for (std::vector<std::ObjectInfo>::iterator iter = Objects->begin(); iter != Objects->end(); iter++)
    {
		Ogre::AxisAlignedBox cbbox=static_cast<Ogre::SceneNode*>(iter->getNode())->_getWorldAABB(); 

		if(spbox->intersects(cbbox))//collided
		{
			answer.amount++;
			answer.collided=true;
			answer.collidedList.push_back(*iter);
		}
    }
    delete spbox;

	return answer;
}
