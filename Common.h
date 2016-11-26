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

#ifndef COMMON_H
#define COMMON_H

#include <Ogre.h>


Ogre::Vector3 convertSphericalToCartesian (Ogre::Real latitude, Ogre::Real longitude);

Ogre::Vector2 convertCartesianToPlateCarree(Ogre::Vector3 position);

Ogre::Real heightNoise(std::vector<float> &amplitude,
                       std::vector<float> &frequency, Ogre::Vector3 Point);

Ogre::ColourValue generatePixel(Ogre::Real height,
                                Ogre::Real seaHeight,
                                Ogre::Real minimumHeight,
                                Ogre::Real maximumHeight,
                                Ogre::ColourValue water1st,
                                Ogre::ColourValue water2nd,
                                Ogre::ColourValue terrain1st,
                                Ogre::ColourValue terrain2nd,
                                Ogre::ColourValue mountain1st,
                                Ogre::ColourValue mountain2nd);

/* 2d-array allocator. Because of templating, the whole definition must be in a header */
template <typename T>
T **allocate2DArray(unsigned short sizeY, unsigned short sizeX)
{
	int y;
	T *memArea, **ptrArea;

	// memArea is destination for all the values, ptrArea are pointers to areas of memArea.
	memArea = new T [sizeX*sizeY];
	ptrArea = new T*[sizeY];

	// Populate pointers for individual scanlines of x
	for(y=0; y < sizeY; y++)
	{
		ptrArea[y] = &memArea[y*sizeX];
	}

	return ptrArea;
}

template <typename T>
void free2DArray(T *ptr)
{
	delete[] ptr[0];
	delete[] ptr;
}


#endif
