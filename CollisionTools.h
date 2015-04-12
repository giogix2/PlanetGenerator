/******************************************************************************************
MOC - Minimal Ogre Collision v 1.0
The MIT License

Copyright (c) 2008, 2009 MouseVolcano (Thomas Gradl, Karolina Sefyrin), Esa Kylli

Thanks to Erik Biermann for the help with the Videos, SEO and Webwork

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
******************************************************************************************/
#ifndef COLLISIONTOOLS_H
#define COLLISIONTOOLS_H

#include <Ogre.h>

// uncomment if you want to use ETM as terrainmanager
//#define ETM_TERRAIN

#ifdef ETM_TERRAIN
#include "ETTerrainInfo.h"
#endif

namespace MOC {

class CollisionTools {
public:
	Ogre::RaySceneQuery *mRaySceneQuery;
	Ogre::RaySceneQuery *mTSMRaySceneQuery;

	Ogre::SceneManager *mSceneMgr;

#ifdef ETM_TERRAIN
	const ET::TerrainInfo* mTerrainInfo;
	CollisionTools(Ogre::SceneManager *sceneMgr, const ET::TerrainInfo* terrainInfo);
#endif

	CollisionTools(Ogre::SceneManager *sceneMgr);
	~CollisionTools();

	bool raycastFromCamera(Ogre::RenderWindow* rw, Ogre::Camera* camera, const Ogre::Vector2 &mousecoords, Ogre::Vector3 &result, Ogre::MovableObject* &target,float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);
	// convenience wrapper with Ogre::Entity to it:
	bool raycastFromCamera(Ogre::RenderWindow* rw, Ogre::Camera* camera, const Ogre::Vector2 &mousecoords, Ogre::Vector3 &result, Ogre::Entity* &target,float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);

	bool collidesWithEntity(const Ogre::Vector3& fromPoint, const Ogre::Vector3& toPoint, const float collisionRadius = 2.5f, const float rayHeightLevel = 0.0f, const Ogre::uint32 queryMask = 0xFFFFFFFF);

	void calculateY(Ogre::SceneNode *n, const bool doTerrainCheck = true, const bool doGridCheck = true, const float gridWidth = 1.0f, const Ogre::uint32 queryMask = 0xFFFFFFFF);

	float getTSMHeightAt(const float x, const float z);
	
	bool raycastFromPoint(const Ogre::Vector3 &point, const Ogre::Vector3 &normal, Ogre::Vector3 &result,Ogre::MovableObject* &target,float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);
	// convenience wrapper with Ogre::Entity to it:
	bool raycastFromPoint(const Ogre::Vector3 &point, const Ogre::Vector3 &normal, Ogre::Vector3 &result,Ogre::Entity* &target,float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);
	
	bool raycast(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::MovableObject* &target,	float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);
	// convenience wrapper with Ogre::Entity to it:
	bool raycast(const Ogre::Ray &ray, Ogre::Vector3 &result, Ogre::Entity* &target,		float &closest_distance, const Ogre::uint32 queryMask = 0xFFFFFFFF);

	void setHeightAdjust(const float heightadjust);
	float getHeightAdjust(void);

private:

	float _heightAdjust;

	void GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                Ogre::uint32* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale);

};

};

#endif