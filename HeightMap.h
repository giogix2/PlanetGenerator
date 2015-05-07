#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreMatrix3.h>

class HeightMap
{
public:
	enum HeightMap_neighbour {neighbour_XP, neighbour_XM, neighbour_YP, neighbour_YM};
	HeightMap(unsigned int size, const Ogre::Matrix3 face);
	~HeightMap();
	unsigned int getSize();
	void setHeight(unsigned int x, unsigned int y, float elevation);
	float getHeight(unsigned int x, unsigned int y);
	void setToMinimumHeight(float minimumHeight);
	void getHistogram(unsigned int histogram[], unsigned short brackets);
	void getMinMax(float &min, float &max);
	Ogre::Vector3 projectToSphere(unsigned int x, unsigned int y);
	void setNeighbours(HeightMap *xPlus, HeightMap *xMinus, HeightMap *yPlus, HeightMap *yMinus);
	HeightMap *getNeighbourPtr(HeightMap_neighbour neighbour);
	bool getNeighbourEntryCoordinates(HeightMap_neighbour neighbour, unsigned int &entry_x, unsigned int &entry_y);
	void generateMeshData(Ogre::Vector3 *vArray, Ogre::Vector2 *texArray,
						  unsigned int *idxArray, float scalingFactor);
private:
	float **height;
	float minHeight;
	float maxHeight;
	unsigned int mapSize;
	Ogre::Matrix3 orientation;
	HeightMap *xplusNeighbour;
	HeightMap *xminusNeighbour;
	HeightMap *yplusNeighbour;
	HeightMap *yminusNeighbour;
};

#endif // HEIGHTMAP_H
