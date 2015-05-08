#ifndef _PSphere_H_
#define _PSphere_H_

#include "ObjectInfo.h"
#include "HeightMap.h"
#include "ResourceParameter.h"
using namespace std;


class PSphere
{
public:
	enum Direction {PSPHERE_GRID_YPLUS, PSPHERE_GRID_YMINUS, PSPHERE_GRID_XPLUS, PSPHERE_GRID_XMINUS};

	void create(Ogre::uint32 iters, ResourceParameter resourceParameter);

	void destroy();

	void loadToBuffers(const std::string &meshName, const std::string &textureName);

	void loadMeshFile(const std::string &path, const std::string &meshName);

	bool checkIfObjectIsIn (std::string &objectName);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real x, Ogre::Real y, Ogre::Real z);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude);

	void attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real latitude, Ogre::Real longitude);

	void attachMeshOnGround(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude);



	Ogre::MeshPtr getMesh();

	void setObserverPosition(Ogre::Vector3 position);

	Ogre::Real getObserverDistanceToSurface();

	Ogre::Real PSphere::getSurfaceHeight(Ogre::Vector3 Position);

	bool checkAccessibility(Ogre::Vector3 location);

	Ogre::Real getRadius();

	bool getGridLocation(Ogre::Vector3 location, HeightMap **face, unsigned int &x, unsigned int &y);

	Ogre::Vector3 nextPosition(Ogre::Vector3 location, PSphere::Direction dir);

	PSphere();

private:
	Ogre::Vector3		*vertexes;
	Ogre::Vector3		*vNorms;
	Ogre::ColourValue	*colours;
	Ogre::Vector2		*texCoords;
	Ogre::uint32		*indexes;
	Ogre::uint32		vertexCount;
	Ogre::uint32		indexCount;
	Ogre::Real			radius;
	Ogre::Real			seaHeight;
	unsigned char		*image;
	Ogre::MeshPtr		mesh;
	Ogre::Vector3		observer;
	HeightMap			*faceYP;
	HeightMap			*faceXM;
	HeightMap			*faceYM;
	HeightMap			*faceXP;
	HeightMap			*faceZP;
	HeightMap			*faceZM;
	ResourceParameter	RParameter;
	Ogre::Vector3		randomTranslate;
	vector<ObjectInfo>		objects;

	void calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour);

	void fixTextureSeam();

	void calculateSeaLevel(float &minElev, float &maxElev, float seaFraction);

	void smoothSeaArea();

	void deform(HeightMap *map);

	void calculateNormals();

	Ogre::Vector3 calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord);

	Ogre::Real heightNoise(vector<float> amplitude,
						   vector<float> frequency, Ogre::Vector3 Point);

	void generateImage(Ogre::Real top, Ogre::Real bottom);

	void generateMeshData();

};

#endif
