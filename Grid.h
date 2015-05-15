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

#ifndef GRID_H
#define GRID_H

#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreMatrix3.h>

class Grid
{
public:
	enum Grid_neighbour {neighbour_XP, neighbour_XM, neighbour_YP, neighbour_YM};

	Grid(unsigned int size, const Ogre::Matrix3 face);
	~Grid();
	unsigned int getSize();
	void setValue(unsigned int x, unsigned int y, int val);
	int getValue(unsigned int x, unsigned int y);
	Ogre::Vector3 projectToSphere(unsigned int x, unsigned int y);
	void setNeighbours(Grid *xPlus, Grid *xMinus, Grid *yPlus, Grid *yMinus);
	Grid *getNeighbourPtr(Grid_neighbour neighbour);
	bool getNeighbourEntryCoordinates(Grid_neighbour neighbour, unsigned int &entry_x, unsigned int &entry_y);

protected:
	unsigned int	gridSize;
	Ogre::Matrix3	orientation;
	Grid		*xplusNeighbour;
	Grid		*xminusNeighbour;
	Grid		*yplusNeighbour;
	Grid		*yminusNeighbour;

private:
	int **value;

};

#endif // GRID_H
