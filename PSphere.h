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

#ifndef _PSphere_H_
#define _PSphere_H_

#include "ObjectInfo.h"
#include "Grid.h"
#include "HeightMap.h"
#include "ResourceParameter.h"
#include "CollisionManager.h"
using namespace std;


class PSphere
{
public:
	enum Direction {PSPHERE_GRID_YPLUS, PSPHERE_GRID_YMINUS, PSPHERE_GRID_XPLUS, PSPHERE_GRID_XMINUS};

	enum MapType {MAP_EQUIRECTANGULAR, MAP_CUBE};

	void loadToBuffers(const std::string &meshName, const std::string &textureName);

	void loadMeshFile(const std::string &path, const std::string &meshName);

	bool checkIfObjectIsIn (std::string &objectName);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real latitude, Ogre::Real longitude);

	void attachMeshOnGround(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude);

	void moveObject(const std::string &objectName, int direction, float pace);

	Ogre::MeshPtr getMesh();

	void setObserverPosition(Ogre::Vector3 position);

	Ogre::Real getObserverDistanceToSurface();

	Ogre::Real getSurfaceHeight(Ogre::Vector3 Position);

	bool checkAccessibility(Ogre::Vector3 location);

	Ogre::Real getRadius();

	bool getGridLocation(Ogre::Vector3 location, Grid **face, unsigned int &x, unsigned int &y);

	Ogre::Vector3 nextPosition(Ogre::Vector3 location, PSphere::Direction dir);

	vector<ObjectInfo> *getObjects();

	void setCollisionManager(CollisionManager	*CDM);

	void exportMap(unsigned short width, unsigned short height, string fileName, MapType type);

	unsigned char *exportMap(unsigned short width, unsigned short height, MapType type);

	PSphere(Ogre::uint32 iters, Ogre::uint32 gridSize, ResourceParameter resourceParameter);

	~PSphere();

private:
	Ogre::Vector3		*vertexes;
	Ogre::Vector3		*vNorms;
	Ogre::Vector2		*texCoords;
	Ogre::uint32		*indexes;
	Ogre::uint32		vertexCount;
	Ogre::uint32		indexCount;
	Ogre::Real			radius;
	Ogre::Real			seaHeight;
	unsigned char		*surfaceTexture;
	unsigned char		*exportImage;
	Ogre::MeshPtr		mesh;
	Ogre::Vector3		observer;
	HeightMap			*faceYP;
	HeightMap			*faceXM;
	HeightMap			*faceYM;
	HeightMap			*faceXP;
	HeightMap			*faceZP;
	HeightMap			*faceZM;
	Grid			*gridYP;
	Grid			*gridXM;
	Grid			*gridYM;
	Grid			*gridXP;
	Grid			*gridZP;
	Grid			*gridZM;
	ResourceParameter	RParameter;
	Ogre::Vector3		randomTranslate;
	vector<ObjectInfo>	objects;
	CollisionManager	*CollisionDetectionManager;
	Ogre::Real			maximumHeight;
	Ogre::Real			minimumHeight;

	void create(Ogre::uint32 iters, Ogre::uint32 gridSize, ResourceParameter resourceParameter);

	void calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour);

	void fixTextureSeam();

	void calculateSeaLevel(float &minElev, float &maxElev, float seaFraction);

	void smoothSeaArea();

	void deform(HeightMap *map);

	void setGridLandInfo(Grid *grid);

	Ogre::Vector3 calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord);

	Ogre::Real heightNoise(vector<float> amplitude,
						   vector<float> frequency, Ogre::Vector3 Point);

	void generatePixel(unsigned short textureHeight, unsigned short stride, unsigned char *image, unsigned short x, unsigned short y, Ogre::Real height,
									   unsigned char water1stblue, unsigned char water1stgreen, unsigned char water1stred,
									   unsigned char water2ndblue, unsigned char water2ndgreen, unsigned char water2ndred,
									   unsigned char terrain1stblue, unsigned char terrain1stgreen, unsigned char terrain1stred,
									   unsigned char terrain2ndblue, unsigned char terrain2ndgreen, unsigned char terrain2ndred,
									   unsigned char mountain1stblue, unsigned char mountain1stgreen, unsigned char mountain1stred,
									   unsigned char mountain2ndblue, unsigned char mountain2ndgreen, unsigned char mountain2ndred);

	void generateImage(unsigned short width, unsigned short height, unsigned char *image);

	void generateMeshData();

};

#endif
