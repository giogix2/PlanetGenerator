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
}

HeightMap::~HeightMap()
{

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

/* Returns vertex-, texture- and index-arrays through a pointer.
 * Assumes correctly sized arrays are already allocated. */
void HeightMap::generateMeshData(Ogre::Vector3 *vArray, Ogre::Vector2 *texArray,
								 unsigned int *idxArray, float scalingFactor)
{
	unsigned int x, y, idx;

	idx = 0;
	for(x=0; x < mapSize; x++)
	{
		for(y=0; y < mapSize; y++)
		{
			// Project height-map location to a sphere
			vArray[idx] = projectToSphere(x, y) * scalingFactor;

			// Calculate texture-coordinate for the vertex
			texArray[idx] = convertCartesianToPlateCarree(vArray[idx]);

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
			idxArray[idx]=x*mapSize+y+mapSize+1;
			idx++;
			idxArray[idx]=x*mapSize+y;
			idx++;
			idxArray[idx]=x*mapSize+y+mapSize;
			idx++;

			// Triangle 2, in other words, a quad.
			idxArray[idx]=x*mapSize+y;
			idx++;
			idxArray[idx]=x*mapSize+y+mapSize+1;
			idx++;
			idxArray[idx]=x*mapSize+y+1;
			idx++;
		}
	}
}
