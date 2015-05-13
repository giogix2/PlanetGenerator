#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreMatrix3.h>
#include "Grid.h"

class HeightMap: public Grid
{
public:
	HeightMap(unsigned int size, const Ogre::Matrix3 face);
	~HeightMap();
	void setHeight(unsigned int x, unsigned int y, float elevation);
	float getHeight(unsigned int x, unsigned int y);
	void setToMinimumHeight(float minimumHeight);
	void getHistogram(unsigned int histogram[], unsigned short brackets);
	void getMinMax(float &min, float &max);
	Ogre::Vector3 projectToSphere(unsigned int x, unsigned int y);

	void generateMeshData(float scalingFactor);
	Ogre::Vector3 getNormal(unsigned short x,unsigned short y);
	void setNormal(Ogre::Vector3 normal, unsigned short x, unsigned short y);
	void blendNormalsWithNeighbours();
	void outputMeshData(Ogre::Vector3 *verArray, Ogre::Vector3 *norArray, Ogre::Vector2 *texArray,
								   unsigned int *idxArray);

private:
	float	**height;
	float	minHeight;
	float	maxHeight;

	Ogre::Vector3	*vertexes;
	Ogre::Vector3	*verNorms;
	Ogre::Vector2	*txCoords;
	Ogre::uint32	*indexes;

	void calculateNormals();
};

#endif // HEIGHTMAP_H
