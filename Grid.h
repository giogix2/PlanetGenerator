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
