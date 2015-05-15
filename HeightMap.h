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
