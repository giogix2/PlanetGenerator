#ifndef COMMON_H
#define COMMON_H

#include <Ogre.h>


Ogre::Vector3 convertSphericalToCartesian (Ogre::Real latitude, Ogre::Real longitude);

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
