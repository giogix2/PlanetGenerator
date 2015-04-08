
#include "Common.h"


Ogre::Vector3 convertSpephicalToCartesian (Ogre::Real latitude, Ogre::Real longitude)   {
	Ogre::Vector3 sphereCoord;
	Ogre::Real alfa, beta;

	Ogre::Real lat_radian;
	Ogre::Real long_radian;
	lat_radian = (latitude/180)*Ogre::Math::PI;
	long_radian = (longitude/180)*Ogre::Math::PI;

	sphereCoord.x = cosf(lat_radian)*cosf(long_radian);
	sphereCoord.y = cosf(lat_radian)*sinf(long_radian);
	sphereCoord.z = sinf(lat_radian);

	return sphereCoord;
}

