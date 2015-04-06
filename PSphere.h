#ifndef _PSphere_H_
#define _PSphere_H_

#include "HeightMap.h"

class PSphere
{
public:
	void create(Ogre::Real diameter, Ogre::Real seaFraction, Ogre::uint32 iters);

	void destroy();

	void loadToBuffers(const std::string &meshName, const std::string &textureName);

	Ogre::MeshPtr getMesh();

	void setObserverPosition(Ogre::Vector3 position);

	Ogre::Real getObserverDistanceToSurface();

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
	unsigned char		*image;
	Ogre::MeshPtr		mesh;
	Ogre::Vector3		observer;
	HeightMap			*faceYP;
	HeightMap			*faceXM;
	HeightMap			*faceYM;
	HeightMap			*faceXP;
	HeightMap			*faceZP;
	HeightMap			*faceZM;

	void calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour);

	void fixTextureSeam();

	void calculateSeaLevel(float &seaLevel, float &minElev, float &maxElev, float seaFraction);

	void smoothSeaArea(float seaHeight);

	void deform(HeightMap *map);

	void calculateNormals();

	Ogre::Vector3 calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord);

	Ogre::Real heightNoise(Ogre::uint32 octaves, Ogre::Real *amplitudes,
						   Ogre::Real *frequencys, Ogre::Vector3 Point);

	void generateImage(Ogre::Real seaHeight, Ogre::Real top, Ogre::Real bottom);

	void generateMeshData();

};

#endif
