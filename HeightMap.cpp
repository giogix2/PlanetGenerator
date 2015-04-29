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

			Ogre::Vector3	XYprojection, LongitudeRef, LatitudeRef;
			Ogre::Real		u, v;

			/* Calculates equirectangular projection AKA plate carrée
			 * Coordinate convention for textures:
			 * texture(0.0 <= u <= 1.0, 0.0 <= v <= 1.0)
			 * z-direction is polar. model(rnd, rnd, +z) => texture(rnd, >0.5)
			 * model(+x, 0, rnd) => texture(0, rnd) and
			 * model(0, +y, rnd) => texture(0.25, rnd)
			 * and model(0, -y, rnd) => texture(0.75, rnd) */

			// vertex projection in xy-plane
			XYprojection = Ogre::Vector3(vArray[idx].x, vArray[idx].y, 0.0f);

			// if x and y are both zero, result for u is nan. Guard against it.
			if(XYprojection.length() != 0)
			{
				LongitudeRef = Ogre::Vector3(1.0f, 0.0f, 0.0f);
				u = acosf( XYprojection.dotProduct(LongitudeRef)
						  /(XYprojection.length()*LongitudeRef.length()) );
				if(vArray[idx].y < 0)
					u = Ogre::Math::TWO_PI - u;
				u = u/Ogre::Math::TWO_PI;
			}
			else
			{
				u = 0.0f;   // x = 0 and y = 0, set u = 0;
			}

			// -z is defined as southpole
			LatitudeRef = Ogre::Vector3(0.0f, 0.0f, -1.0f);
			v = acosf(vArray[idx].dotProduct(LatitudeRef)
					  /(vArray[idx].length()*LatitudeRef.length()));

			v = v/Ogre::Math::PI;

			texArray[idx] = Ogre::Vector2(u, v);

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
