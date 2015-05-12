#include <assert.h>

#include "HeightMap.h"
#include "Common.h"

HeightMap::HeightMap(unsigned int size, const Ogre::Matrix3 face)
{
	height = allocate2DArray<float>(size, size);
	memset(height[0], 0, sizeof(float)*size*size);

	mapSize = size;
	orientation = face;

	minHeight =  1.0e9f;
	maxHeight = -1.0e9f;

	xplusNeighbour = NULL;
	xminusNeighbour = NULL;
	yplusNeighbour = NULL;
	yminusNeighbour = NULL;

	vertexes = new Ogre::Vector3[mapSize*mapSize];
	verNorms = new Ogre::Vector3[mapSize*mapSize];
	txCoords = new Ogre::Vector2[mapSize*mapSize];
	indexes = new Ogre::uint32[(mapSize-1)*(mapSize-1)*6];
}

HeightMap::~HeightMap()
{
	free2DArray(height);

	delete[] vertexes;
	delete[] verNorms;
	delete[] txCoords;
	delete[] indexes;
}

unsigned int HeightMap::getSize()
{
	return mapSize;
}

void HeightMap::getHistogram(unsigned int histogram[], unsigned short brackets)
{
	unsigned int x, y, i;
	/* Subtract brackets with 1.5 to make sure iterator can't go over 99.
	 * Add assert to catch possible overflows in the future. */
	float divider = static_cast<float>(brackets) - 1.5f;

	for(x=0; x < mapSize; x++)
	{
		for(y=0; y < mapSize; y++)
		{
			// Any better ways to create histograms?
			i = 0;
			while(height[y][x] > (i/divider*(maxHeight-minHeight) + minHeight))
				i++;
			// Check overflow with assert
			assert(i < brackets);
			histogram[i] += 1;
		}
   }
}

void HeightMap::getMinMax(float &min, float &max)
{
	min = minHeight;
	max = maxHeight;
}

void HeightMap::setHeight(unsigned int x, unsigned int y, float elevation)
{
	height[y][x] = elevation;

	if(elevation < minHeight)
		minHeight = elevation;
	if(elevation > maxHeight)
		maxHeight = elevation;
}

float HeightMap::getHeight(unsigned int x, unsigned int y)
{
	return height[y][x];
}

void HeightMap::setToMinimumHeight(float minimumHeight)
{
	unsigned int x, y;

	for(y=0; y < mapSize; y++)
		for(x=0; x < mapSize; x++)
			if(height[y][x] < minimumHeight)
				height[y][x] = minimumHeight;

	minHeight = minimumHeight;
}

/* Return vector for a normalized (radius 1.0) sphere */
Ogre::Vector3 HeightMap::projectToSphere(unsigned int x, unsigned int y)
{
	Ogre::Vector3 pos;
	float mSizeFloat, xFloat, yFloat;

	mSizeFloat = static_cast<float>(mapSize-1);
	xFloat = static_cast<float>(x)/mSizeFloat;
	yFloat = static_cast<float>(y)/mSizeFloat;

	// For convenience treat xy-heightmap as a xz-plane in sphere-coordinates
	pos.x = 1.0f - xFloat*2.0f;
	pos.z = -1.0f + yFloat*2.0f;
	pos.y = 1.0f;
	// Simple re-orientation
	pos = orientation*pos;
	// project heightMap to sphere
	pos.normalise();
	/* add height */
	pos = pos + pos*height[y][x];
	return pos;
}

/* Function to set neighboring HeightMaps */
void HeightMap::setNeighbours(HeightMap *xPlus, HeightMap *xMinus, HeightMap *yPlus, HeightMap *yMinus)
{
	xplusNeighbour = xPlus;
	xminusNeighbour = xMinus;
	yplusNeighbour = yPlus;
	yminusNeighbour = yMinus;
}

/* Get pointer for neighboring grid by using an enumerator */
HeightMap *HeightMap::getNeighbourPtr(HeightMap_neighbour neighbour)
{
	if (neighbour == neighbour_XP)
		return xplusNeighbour;

	else if (neighbour == neighbour_XM)
		return xminusNeighbour;

	else if (neighbour == neighbour_YP)
		return yplusNeighbour;

	else if (neighbour == neighbour_YM)
		return yminusNeighbour;
	/* Currently else below can't be entered. Used to silence warning about
	 * reaching end of non-void function */
	else
		return NULL;
}

/* When entry_x or entry_y, which are used to point positions on 2D-grids,
 * is on grid-boundary and switch to neighboring HeightMap is desired,
 * this can be used to output new values of entry_x and entry_y for referenced
 * HeightMap.
 * Return values:
 *	On success, returns new entry_x, entry_y and function return value true.
 *	On failure, entry_x, entry_y are not modified and function return value is false. */
bool HeightMap::getNeighbourEntryCoordinates(HeightMap_neighbour neighbour
											 , unsigned int &entry_x, unsigned int &entry_y)
{
	HeightMap *gridNeighbour;

	// Is entry coordinates at the edge of map
	if ( !( (entry_x == this->getSize()-1) || (entry_x == 0)
			|| (entry_y == this->getSize()-1) || entry_y == 0 ) )
		return false;

	gridNeighbour = this->getNeighbourPtr(neighbour);

	/* Assuming both square-grids face their frontface in same direction,
	 * there are 16 different neighboring combinations. */

	// If current HeightMap is x-minus neighbour for neighbour of interest
	if (this == gridNeighbour->getNeighbourPtr(HeightMap::neighbour_XM))
	{
		// Check all 4 side-orientations for a square.
		if (neighbour == neighbour_XP)
		{
			entry_x = 0;
		}
		else if (neighbour == neighbour_XM)
		{
			//entry_x == gridN->getSize()-1;
			std::cerr << "This should not happen! XM and Neighbour XM" << std::endl;
		}
		else if (neighbour == neighbour_YP)
		{
			entry_y = gridNeighbour->getSize() - entry_x - 1;
			entry_x = 0;
		}
		else if (neighbour == neighbour_YM)
		{
			entry_y = entry_x;
			entry_x = 0;
		}
	}
	else if (this == gridNeighbour->getNeighbourPtr(HeightMap::neighbour_XP))
	{
		if (neighbour == neighbour_XP)
		{
			//entry_x = gridN->getSize()-1;
			std::cerr << "This should not happen! XP and Neighbour XP" << std::endl;
		}
		else if (neighbour == neighbour_XM)
		{
			entry_x = gridNeighbour->getSize()-1;
		}
		else if (neighbour == neighbour_YP)
		{
			entry_y = entry_x;
			entry_x = gridNeighbour->getSize()-1;
		}
		else if (neighbour == neighbour_YM)
		{
			entry_y = gridNeighbour->getSize()-entry_x-1;
			entry_x = gridNeighbour->getSize()-1;
		}
	}
	else if (this == gridNeighbour->getNeighbourPtr(HeightMap::neighbour_YM))
	{
		if (neighbour == neighbour_XP)
		{
			entry_x = gridNeighbour->getSize() - entry_y -1;
			entry_y = 0;
		}
		else if (neighbour == neighbour_XM)
		{
			entry_x = entry_y;
			entry_y = 0;
		}
		else if (neighbour == neighbour_YP)
		{
			entry_y = 0;
		}
		else if (neighbour == neighbour_YM)
		{
			entry_x = gridNeighbour->getSize() - entry_x -1;
			entry_y = 0;
		}
	}
	else if (this == gridNeighbour->getNeighbourPtr(HeightMap::neighbour_YP))
	{
		if (neighbour == neighbour_XP)
		{
			entry_x = entry_y;
			entry_y = gridNeighbour->getSize()-1;
		}
		else if (neighbour == neighbour_XM)
		{
			entry_x = gridNeighbour->getSize() - entry_y - 1;
			entry_y = gridNeighbour->getSize() - 1;
		}
		else if (neighbour == neighbour_YP)
		{
			entry_x = gridNeighbour->getSize()-entry_x-1;
			entry_y = gridNeighbour->getSize()-1;
		}
		else if (neighbour == neighbour_YM)
		{
			entry_y = gridNeighbour->getSize() - 1;
		}
	}

	return true;
}

// scalingFactor scales size of the mesh
void HeightMap::generateMeshData(float scalingFactor)
{
	unsigned int x, y, idx;

	idx = 0;
	for(x=0; x < mapSize; x++)
	{
		for(y=0; y < mapSize; y++)
		{
			// Project height-map location to a sphere
			vertexes[idx] = projectToSphere(x, y) * scalingFactor;

			// Calculate texture-coordinate for the vertex
			txCoords[idx] = convertCartesianToPlateCarree(vertexes[idx]);
			idx++;
		}
	}

	// Create indexes to build triangles.
	idx = 0;
	for(x=0; x < mapSize-1; x++)
	{
		for(y=0; y < mapSize-1; y++)
		{
			// Triangle 1
			indexes[idx] = x*mapSize+y+mapSize+1;
			idx++;
			indexes[idx] = x*mapSize+y;
			idx++;
			indexes[idx] = x*mapSize+y+mapSize;
			idx++;

			// Triangle 2, in other words, a quad.
			indexes[idx] = x*mapSize+y;
			idx++;
			indexes[idx] = x*mapSize+y+mapSize+1;
			idx++;
			indexes[idx] = x*mapSize+y+1;
			idx++;
		}
	}

	// Generate normals
	calculateNormals();
}

void HeightMap::calculateNormals()
{
	Ogre::uint32 i, indexCount;
	Ogre::Vector3 p1, p2, normal;

	// Blended normals for a vertex
	// first, zero our normals, so that we can use it as a accumulator
	for(i=0; i < mapSize*mapSize; i++)
	{
		verNorms[i].x = 0.0f;
		verNorms[i].y = 0.0f;
		verNorms[i].z = 0.0f;
	}
	/* calculate normals for every triangle. Multiple normals are added together
	 * for each vertex, so result is average of all the normals. */
	indexCount = (mapSize-1)*(mapSize-1)*6;
	for(i=0; i < indexCount; i = i + 3)
	{
		p1 = vertexes[indexes[i]] - vertexes[indexes[i+1]];
		p2 = vertexes[indexes[i+1]] - vertexes[indexes[i+2]];
		normal = p1.crossProduct(p2);

		verNorms[indexes[i]] += normal;
		verNorms[indexes[i+1]] += normal;
		verNorms[indexes[i+2]] += normal;
	}
	// Normalization pass
	for(i=0; i < mapSize*mapSize; i++)
	{
		verNorms[i].normalise();
	}
}

/* Make sure you have valid normals on your neighbours before calling this */
void HeightMap::blendNormalsWithNeighbours()
{
	unsigned int i;
	unsigned int entry_x, entry_y;
	HeightMap *neighbour;
	Ogre::Vector3 norm, normNeighbour, blendedNorm;

	// Get neighbours pointer
	neighbour = getNeighbourPtr(neighbour_XM);
	if (neighbour != NULL)
	{
		for(i=0; i < mapSize; i++)
		{
			entry_x = 0;
			entry_y = i;
			// get neighboring coordinate
			getNeighbourEntryCoordinates(neighbour_XM, entry_x, entry_y);

			// Get normals
			normNeighbour = neighbour->getNormal(entry_x, entry_y);
			norm = getNormal(0, i);

			// Blend and save
			blendedNorm = norm+normNeighbour;
			blendedNorm.normalise();
			setNormal(blendedNorm, 0, i);
			neighbour->setNormal(blendedNorm, entry_x, entry_y);
		}
	}

	neighbour = getNeighbourPtr(neighbour_XP);
	if (neighbour != NULL)
	{
		for(i=0; i < mapSize; i++)
		{
			entry_x = mapSize-1;
			entry_y = i;
			getNeighbourEntryCoordinates(neighbour_XP, entry_x, entry_y);

			normNeighbour = neighbour->getNormal(entry_x, entry_y);
			norm = getNormal(mapSize-1, i);

			blendedNorm = norm+normNeighbour;
			blendedNorm.normalise();
			setNormal(blendedNorm, mapSize-1, i);
			neighbour->setNormal(blendedNorm, entry_x, entry_y);
		}
	}

	neighbour = getNeighbourPtr(neighbour_YM);
	if (neighbour != NULL)
	{
		for(i=0; i < mapSize; i++)
		{
			entry_x = i;
			entry_y = 0;
			getNeighbourEntryCoordinates(neighbour_YM, entry_x, entry_y);

			normNeighbour = neighbour->getNormal(entry_x, entry_y);
			norm = getNormal(i, 0);

			blendedNorm = norm+normNeighbour;
			blendedNorm.normalise();
			setNormal(blendedNorm, i, 0);
			neighbour->setNormal(blendedNorm, entry_x, entry_y);
		}
	}

	neighbour = getNeighbourPtr(neighbour_YP);
	if (neighbour != NULL)
	{
		for(i=0; i < mapSize; i++)
		{
			entry_x = i;
			entry_y = mapSize-1;
			getNeighbourEntryCoordinates(neighbour_YP, entry_x, entry_y);

			normNeighbour = neighbour->getNormal(entry_x, entry_y);
			norm = getNormal(i, mapSize-1);

			blendedNorm = norm+normNeighbour;
			blendedNorm.normalise();
			setNormal(blendedNorm, i, mapSize-1);
			neighbour->setNormal(blendedNorm, entry_x, entry_y);
		}
	}
}

Ogre::Vector3 HeightMap::getNormal(unsigned short x,unsigned short y)
{
	return verNorms[x*mapSize+y];
}

void HeightMap::setNormal(Ogre::Vector3 normal, unsigned short x, unsigned short y)
{
	verNorms[x*mapSize+y] = normal;
}

/* Returns vertex-, normal-, texture- and index-arrays through a pointer.
 * Assumes correctly sized arrays are already allocated. */
void HeightMap::outputMeshData(Ogre::Vector3 *verArray, Ogre::Vector3 *norArray, Ogre::Vector2 *texArray,
							   unsigned int *idxArray)
{
	unsigned int i;

	for(i=0; i < mapSize*mapSize; i++)
	{
		verArray[i] = vertexes[i];
		norArray[i] = verNorms[i];
		texArray[i] = txCoords[i];
	}
	for(i=0; i < (mapSize-1)*(mapSize-1)*6; i++)
	{
		idxArray[i] = indexes[i];
	}
}
