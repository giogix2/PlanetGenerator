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

#include <assert.h>

#include "HeightMap.h"
#include "Common.h"

HeightMap::HeightMap(unsigned int size, const Ogre::Matrix3 face)
	: Grid(size, face)
{
	height = allocate2DArray<float>(size, size);
	memset(height[0], 0, sizeof(float)*size*size);

	minHeight = std::numeric_limits<float>::max();
	maxHeight = std::numeric_limits<float>::min();

	vertexes = new Ogre::Vector3[gridSize*gridSize];
	verNorms = new Ogre::Vector3[gridSize*gridSize];
	txCoords = new Ogre::Vector2[gridSize*gridSize];
	indexes = new Ogre::uint32[(gridSize-1)*(gridSize-1)*6];
}

HeightMap::~HeightMap()
{
	free2DArray(height);

	delete[] vertexes;
	delete[] verNorms;
	delete[] txCoords;
	delete[] indexes;
}

void HeightMap::getHistogram(unsigned int histogram[], unsigned short brackets)
{
	unsigned int x, y, slot;
	float mult;

	for(y=0; y < gridSize; y++)
	{
		for(x=0; x < gridSize; x++)
		{
			// Calculate histogram-slot for a given height
			mult = static_cast<float>(brackets-1) + 0.5f;
			slot = static_cast<unsigned int>((height[y][x]-minHeight) / (maxHeight-minHeight) * mult);

			// Let's be sure
			assert(slot < brackets);
			histogram[slot] += 1;
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

	for(y=0; y < gridSize; y++)
		for(x=0; x < gridSize; x++)
			if(height[y][x] < minimumHeight)
				height[y][x] = minimumHeight;

	minHeight = minimumHeight;
}

/* Return vector for a normalized (radius 1.0) sphere */
Ogre::Vector3 HeightMap::projectToSphere(unsigned int x, unsigned int y)
{
	Ogre::Vector3 pos;
	float mSizeFloat, xFloat, yFloat;

	mSizeFloat = static_cast<float>(gridSize-1);
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

// scalingFactor scales size of the mesh
void HeightMap::generateMeshData(float scalingFactor)
{
	unsigned int x, y, idx;

	idx = 0;
	for(x=0; x < gridSize; x++)
	{
		for(y=0; y < gridSize; y++)
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
	for(x=0; x < gridSize-1; x++)
	{
		for(y=0; y < gridSize-1; y++)
		{
			// Triangle 1
			indexes[idx] = x*gridSize+y+gridSize+1;
			idx++;
			indexes[idx] = x*gridSize+y;
			idx++;
			indexes[idx] = x*gridSize+y+gridSize;
			idx++;

			// Triangle 2, in other words, a quad.
			indexes[idx] = x*gridSize+y;
			idx++;
			indexes[idx] = x*gridSize+y+gridSize+1;
			idx++;
			indexes[idx] = x*gridSize+y+1;
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
	for(i=0; i < gridSize*gridSize; i++)
	{
		verNorms[i].x = 0.0f;
		verNorms[i].y = 0.0f;
		verNorms[i].z = 0.0f;
	}
	/* calculate normals for every triangle. Multiple normals are added together
	 * for each vertex, so result is average of all the normals. */
	indexCount = (gridSize-1)*(gridSize-1)*6;
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
	for(i=0; i < gridSize*gridSize; i++)
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

	/* Get neighbours pointer. Needs a cast, because getNeighbourPtr
	 * returns Grid *, but we need HeightMap * to manipulate normal-data */
	neighbour = static_cast<HeightMap *>(getNeighbourPtr(neighbour_XM));
	if (neighbour != NULL)
	{
		for(i=0; i < gridSize; i++)
		{
			entry_x = 0;
			entry_y = i;
			// get neighboring coordinate
			if (getNeighbourEntryCoordinates(neighbour_XM, entry_x, entry_y))
			{
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
	}

	neighbour = static_cast<HeightMap *>(getNeighbourPtr(neighbour_XP));
	if (neighbour != NULL)
	{
		for(i=0; i < gridSize; i++)
		{
			entry_x = gridSize-1;
			entry_y = i;
			if (getNeighbourEntryCoordinates(neighbour_XP, entry_x, entry_y))
			{
				normNeighbour = neighbour->getNormal(entry_x, entry_y);
				norm = getNormal(gridSize-1, i);

				blendedNorm = norm+normNeighbour;
				blendedNorm.normalise();
				setNormal(blendedNorm, gridSize-1, i);
				neighbour->setNormal(blendedNorm, entry_x, entry_y);
			}
		}
	}

	neighbour = static_cast<HeightMap *>(getNeighbourPtr(neighbour_YM));
	if (neighbour != NULL)
	{
		for(i=0; i < gridSize; i++)
		{
			entry_x = i;
			entry_y = 0;
			if (getNeighbourEntryCoordinates(neighbour_YM, entry_x, entry_y))
			{
				normNeighbour = neighbour->getNormal(entry_x, entry_y);
				norm = getNormal(i, 0);

				blendedNorm = norm+normNeighbour;
				blendedNorm.normalise();
				setNormal(blendedNorm, i, 0);
				neighbour->setNormal(blendedNorm, entry_x, entry_y);
			}
		}
	}

	neighbour = static_cast<HeightMap *>(getNeighbourPtr(neighbour_YP));
	if (neighbour != NULL)
	{
		for(i=0; i < gridSize; i++)
		{
			entry_x = i;
			entry_y = gridSize-1;
			if (getNeighbourEntryCoordinates(neighbour_YP, entry_x, entry_y))
			{
				normNeighbour = neighbour->getNormal(entry_x, entry_y);
				norm = getNormal(i, gridSize-1);

				blendedNorm = norm+normNeighbour;
				blendedNorm.normalise();
				setNormal(blendedNorm, i, gridSize-1);
				neighbour->setNormal(blendedNorm, entry_x, entry_y);
			}
		}
	}
}

Ogre::Vector3 HeightMap::getNormal(unsigned short x,unsigned short y)
{
	return verNorms[x*gridSize+y];
}

void HeightMap::setNormal(Ogre::Vector3 normal, unsigned short x, unsigned short y)
{
	verNorms[x*gridSize+y] = normal;
}

/* Returns vertex-, normal-, texture- and index-arrays through a pointer.
 * Assumes correctly sized arrays are already allocated. */
void HeightMap::outputMeshData(Ogre::Vector3 *verArray, Ogre::Vector3 *norArray, Ogre::Vector2 *texArray,
							   unsigned int *idxArray)
{
	unsigned int i;

	for(i=0; i < gridSize*gridSize; i++)
	{
		verArray[i] = vertexes[i];
		norArray[i] = verNorms[i];
		texArray[i] = txCoords[i];
	}
	for(i=0; i < (gridSize-1)*(gridSize-1)*6; i++)
	{
		idxArray[i] = indexes[i];
	}
}
