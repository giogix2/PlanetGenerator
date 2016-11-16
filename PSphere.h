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

    void load(Ogre::SceneNode *parent, Ogre::SceneManager *scene, const std::string &planetName, const std::string &textureName);

    void unload(Ogre::SceneManager *scene);

	void loadMeshFile(const std::string &path, const std::string &meshName);

	bool checkIfObjectIsIn (std::string &objectName);

    void attachMeshSphereCoord(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

    void attachMeshSphereCoord(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

    void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude, Ogre::Real dist = 0.0f);

    void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real latitude, Ogre::Real longitude, Ogre::Real dist = 0.0f);

    /* To skip collision detection while moving object, I try to attach it on the ground and set the initial orientation
    *Maybe later we need object in air or outer space, so I leave attachmesh() and create this function which could put the object on ground. */
	void attachMeshOnGround(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude);

    void attachAstroParent(PSphere *object);

    /* Attach another astrological object (star, planet, satellite, etc.) to this one.
    The astrological object attached will be connected to this one with a childNode
    Two node are put between the actual planet and the astrological object*/
    void attachAstroChild(PSphere *object, Ogre::Real x, Ogre::Real y, Ogre::Real z);

    void setNode(Ogre::SceneNode *node);

    void setEntity(Ogre::Entity *entity);

    Ogre::SceneNode* getNode();

    Ogre::Entity* getEntity();

    void moveObject(const std::string &objectName, int direction, float pace);

    void moveObjectRevolution(const std::string &objectName, int direction, float pace);

    void moveAstroChild(const std::string &objectName, Ogre::Real pitch, Ogre::Real yaw, Ogre::Real roll);

    /* Set position for the observer. This must be position vector in modelspace,
     * not in worldspace. In other words, one must undo rotations. */
	void setObserverPosition(Ogre::Vector3 position);

    /* Gives observer distance to the point on surface that is directly between
     * observer and planet origo.
     * Negative values mean that the observer is inside the planet */
	Ogre::Real getObserverDistanceToSurface();

    /* Get the Height of a particular position of the surface.
     * Position is a 3-d vector over the surface which you need to know the height
     * return the height */
	Ogre::Real getSurfaceHeight(Ogre::Vector3 Position);

    /* Checks if position is water or solid ground.
     * Returns:
     *  On ground, return true.
     *  On water or has an object, return false. */
	bool checkAccessibility(Ogre::Vector3 location);

    /* return radius */
	Ogre::Real getRadius();

    string getMeshName();

    PSphere* getAstroChild(const std::string &objectName);

    string getTextureName();

    /* Figures which one of the cubefaces 3D-location lands, and what 2D-coordinates
     * that face has.
     * Returns:
     *	On success, pointer to a pointer of HeightMap location lands,
     *	HeightMap-coordinates x and y, and function return value true.
     *	On failure, return function value is false. */
	bool getGridLocation(Ogre::Vector3 location, Grid **face, unsigned int &x, unsigned int &y);

	Ogre::Vector3 nextPosition(Ogre::Vector3 location, PSphere::Direction dir);

	vector<ObjectInfo> *getObjects();

	void setCollisionManager(CollisionManager	*CDM);

    /* Saves map to the given filename. With MapType MAP_CUBE ignores height-variable */
	bool exportMap(unsigned short width, unsigned short height, string fileName, MapType type);

    /* Generates a map and gives pointer to array that has rgb-image
     * information. Then MapType is MAP_CUBE, ignores height variable.
     * With wrong type, returns NULL-pointer. */
	unsigned char *exportMap(unsigned short width, unsigned short height, MapType type);

	PSphere(Ogre::uint32 iters, Ogre::uint32 gridSize, Ogre::uint16 textureWidth, Ogre::uint16 textureHeight, ResourceParameter resourceParameter);

	ResourceParameter *getParameters();

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
    Ogre::SceneNode     *node;
    Ogre::Entity        *entity[6];
	unsigned char		*surfaceTexture;
	unsigned short		surfaceTextureWidth;
	unsigned short		surfaceTextureHeight;
	unsigned char		*exportImage;
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
    vector<PSphere*>    astroObjectsParent;
    vector<PSphere*>    astroObjectsChild;
	CollisionManager	*CollisionDetectionManager;
	Ogre::Real			maximumHeight;
	Ogre::Real			minimumHeight;
    string              meshName[6];
    string              textureName[6];

    // Makes a sphere out of a cube that is made of 6 squares
	void create(Ogre::uint32 iters, Ogre::uint32 gridSize, ResourceParameter resourceParameter);

    void deform(HeightMap *map);

    void calculateSeaLevel(float &minElev, float &maxElev, float seaFraction);

    /* Generates surface-texturemap using noise-generated height differences.
     * Expects pointer to be already correctly allocated. */
    void generateImage(unsigned short width, unsigned short height, unsigned char *image);

    void setGridLandInfo(Grid *grid);

    void smoothSeaArea();

	void calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour);

    /* Fix a seam by adding vertex duplicates with texture u going over 1.0 */
	void fixTextureSeam();

	Ogre::Vector3 calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord);

	Ogre::Real heightNoise(vector<float> &amplitude,
						   vector<float> &frequency, Ogre::Vector3 Point);

    // Returns colour-values for one pixel
	Ogre::ColourValue generatePixel(Ogre::Real height,
									   Ogre::ColourValue water1st, Ogre::ColourValue water2nd, Ogre::ColourValue terrain1st,
									   Ogre::ColourValue terrain2nd, Ogre::ColourValue mountain1st, Ogre::ColourValue mountain2nd);

	void generateMeshData();

    void bufferMesh(const std::string &meshName, Ogre::Vector3 *verts, Ogre::Vector3 *norms, Ogre::Vector2 *txCrds, Ogre::uint32 *indxs, Ogre::uint32 vCount, Ogre::uint32 iCount);

    void bufferTexture(const std::string &textureName);

};

#endif
