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

#include "Common.h"


Ogre::Vector3 convertSphericalToCartesian (Ogre::Real latitude, Ogre::Real longitude)   {
	Ogre::Vector3 sphereCoord;

	Ogre::Real lat_radian;
	Ogre::Real long_radian;
	lat_radian = (latitude/180)*Ogre::Math::PI;
	long_radian = (longitude/180)*Ogre::Math::PI;

	sphereCoord.x = cosf(lat_radian)*cosf(long_radian);
	sphereCoord.y = cosf(lat_radian)*sinf(long_radian);
	sphereCoord.z = sinf(lat_radian);

	return sphereCoord;
}

/* Calculates equirectangular projection AKA plate carrée
 * Coordinate convention for textures:
 * texture(0.0 <= u <= 1.0, 0.0 <= v <= 1.0)
 * z-direction is polar. model(rnd, rnd, +z) => texture(rnd, >0.5)
 * model(+x, 0, rnd) => texture(0, rnd) and
 * model(0, +y, rnd) => texture(0.25, rnd)
 * and model(0, -y, rnd) => texture(0.75, rnd) */
Ogre::Vector2 convertCartesianToPlateCarree(Ogre::Vector3 position) {

	Ogre::Vector3	XYprojection, LongitudeRef, LatitudeRef;
	Ogre::Real		u, v;

	// vertex projection in xy-plane
	XYprojection = Ogre::Vector3(position.x, position.y, 0.0f);

	// if x and y are both zero, result for u is nan. Guard against it.
	if(XYprojection.length() != 0)
	{
		LongitudeRef = Ogre::Vector3(1.0f, 0.0f, 0.0f);
		u = acosf( XYprojection.dotProduct(LongitudeRef)
				  /(XYprojection.length()*LongitudeRef.length()) );
		if(position.y < 0)
			u = Ogre::Math::TWO_PI - u;
		u = u/Ogre::Math::TWO_PI;
	}
	else
	{
		u = 0.0f;   // x = 0 and y = 0, set u = 0;
	}

	// -z is defined as southpole
	LatitudeRef = Ogre::Vector3(0.0f, 0.0f, -1.0f);
	v = acosf(position.dotProduct(LatitudeRef)
			  /(position.length()*LatitudeRef.length()));

	v = v/Ogre::Math::PI;

	return Ogre::Vector2(u, v);
}
