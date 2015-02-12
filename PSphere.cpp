#include "OGRE/Ogre.h"
#include "PSphere.h"
#include "simplexnoise1234.h"

// Let's set texture dimensions this way for a time being
#define TEX_WIDTH 1024
#define TEX_HEIGHT 512

PSphere::PSphere(){
	vertexes = NULL;
	vNorms = NULL;
	colours = NULL;
	texCoords = NULL;
	indexes = NULL;
	image = NULL;
}

void PSphere::calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour)
{
	Ogre::Vector3   vertexXYprojection, LongitudeRef, LatitudeRef;
	Ogre::Real      u, v;

	// Make vector as long as sphere radius
	vertex = vertex * radius/vertex.length();

	vertexes[vertexCount] = vertex;

	colours[vertexCount] = colour;


	/* Calculates equirectangular projection AKA plate carrée
	 * Coordinate convention for textures:
	 * texture(0.0 <= u <= 1.0, 0.0 <= v <= 1.0)
	 * z-direction is polar. model(rnd, rnd, +z) => texture(rnd, >0.5)
	 * model(+x, 0, rnd) => texture(0, rnd) and model(0, +y, rnd) => texture(0.25, rnd)
	 * and model(0, -y, rnd) => texture(0.75, rnd) */

	vertexXYprojection = Ogre::Vector3(vertex.x, vertex.y, 0.0f);   // vertex projection in xy-plane

	// if x and y are both zero, result for u is nan. Guarding against it.
	if(vertexXYprojection.length() != 0)
	{
		LongitudeRef = Ogre::Vector3(1.0f, 0.0f, 0.0f);
		u = acos( vertexXYprojection.dotProduct(LongitudeRef)
				  /(vertexXYprojection.length()*LongitudeRef.length()) );
		if(vertex.y < 0)
			u = Ogre::Math::TWO_PI - u;
		u = u/Ogre::Math::TWO_PI;
	}
	else
	{
		u = 0.0f;   // x = 0 and y = 0, set u = 0;
	}

	// -z is defined as southpole
	LatitudeRef = Ogre::Vector3(0.0f, 0.0f, -1.0f);
	v = acos(vertex.dotProduct(LatitudeRef)/(vertex.length()*LatitudeRef.length()));

	v = v/Ogre::Math::PI;

	texCoords[vertexCount] = Ogre::Vector2(u, v);

	// increase count
	vertexCount++;
}

/* Get spherical coordinates from texture coordinate */
Ogre::Vector3 PSphere::calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord)
{
	Ogre::Vector3 sphereCoord;
	Ogre::Real alfa, beta;

	alfa = texCoord->x * Ogre::Math::TWO_PI;
	beta = texCoord->y * Ogre::Math::PI;

	sphereCoord.x = radius * cos(alfa)*sin(beta);
	sphereCoord.y = radius * sin(alfa)*sin(beta);
	sphereCoord.z = radius * cos(beta);

	return sphereCoord;
}

Ogre::Real PSphere::heightNoise(Ogre::uint32 octaves, Ogre::Real *amplitudes,
							   Ogre::Real *frequencys, Ogre::Vector3 Point)
{
	Ogre::uint32 i;
	Ogre::Real height = 0.0f;

	// Run through the octaves
	for(i=0; i < octaves; i++)
	{
		height += amplitudes[i] * SimplexNoise1234::noise(Point.x/frequencys[i],
														  Point.y/frequencys[i],
														  Point.z/frequencys[i]);
	}

	return height;
}

/* Fix a seam by adding vertex duplicates with texture u going over 1.0 */
void PSphere::fixTextureSeam()
{
	Ogre::uint32 i, j, affectedTriangleCount=0, vCntBeforeFix;
	Ogre::Real absDiff1, absDiff2, absDiff3;

	vCntBeforeFix = vertexCount;

	for(i=0; i < indexCount; i = i + 3)
	{
		absDiff1 = Ogre::Math::Abs(texCoords[indexes[i]].x - texCoords[indexes[i+1]].x);
		absDiff2 = Ogre::Math::Abs(texCoords[indexes[i]].x - texCoords[indexes[i+2]].x);
		absDiff3 = Ogre::Math::Abs(texCoords[indexes[i+1]].x - texCoords[indexes[i+2]].x);

		/* Check for an abrupt change in triangles u-coordinates
		 * (texture-coordinate(u, v)). */
		if(absDiff1 > 0.3f || absDiff2 > 0.3f || absDiff3 > 0.3f)
		{
			for(j=0; j < 3; j++)
			{
				if(texCoords[indexes[i+j]].x < 0.3f)
				{
					// Duplicate offending vertex data
					vertexes[vertexCount] = vertexes[indexes[i+j]];
					vNorms[vertexCount] = vNorms[indexes[i+j]];
					colours[vertexCount] = colours[indexes[i+j]];
					texCoords[vertexCount] = texCoords[indexes[i+j]];

					// Give correct u
					texCoords[vertexCount].x += 1.0f;

					// uddate index to point to the new vertex
					indexes[i+j] = vertexCount;

					vertexCount++;

				}
			}
			affectedTriangleCount++;
		}
	}

	/* FIXME: Might still have some problems in the poles. Revise if necessary */

	std::cout << "fixTextureSeam:" << std::endl
			  << "  number of fixed triangles "
			  << affectedTriangleCount << std::endl
			  << "  number of individual vertexes duplicated "
			  << vertexCount - vCntBeforeFix << std::endl;
}

/* Example that shows procedural generation of textures */
void PSphere::generateImage(Ogre::uint32 octaves, Ogre::Real *amplitudes, Ogre::Real *frequencys,
						   Ogre::Real height, Ogre::Real seaHeight, Ogre::Real top, Ogre::Real bottom)
{
	Ogre::Vector3 spherePoint;
	Ogre::Vector2 texCoords;
	Ogre::uint32 x, y;
	Ogre::uint8 red, green, blue, tempVal;

	// Guard against multiple memory allocations to avoid memory leaks
	if(image != NULL)
		delete[] image;
	image = new unsigned char[TEX_WIDTH*TEX_HEIGHT*3];

	for(y=0; y < TEX_HEIGHT; y++)
	{
		for(x=0; x < TEX_WIDTH; x++)
		{
			texCoords.x = (Ogre::Real(x)+0.5f)/TEX_WIDTH;
			texCoords.y = (Ogre::Real(y)+0.5f)/TEX_HEIGHT;

			// Get a point that corresponds to a given pixel
			spherePoint = calculateSphereCoordsFromTexCoords(&texCoords);

			// Get height of a point
			height = heightNoise(octaves, amplitudes, frequencys, spherePoint);

			// Set sea-colors, deeper part is slighly deeper blue.
			if(height < seaHeight)
			{
				red = 100-Ogre::uchar((seaHeight-height)/(seaHeight-bottom)*50.0f);
				green = 255-Ogre::uchar((seaHeight-height)/(seaHeight-bottom)*50.0f);
				blue = 255;
			}
			else
			{
				// Set low elevations green and higher brown
				red = 0+Ogre::uchar((height-seaHeight)/(top-seaHeight)*250.0f);
				green = 255-Ogre::uchar((height-seaHeight)/(top-seaHeight)*250.0f);
				blue = 0;
				// Highest elevations are bright grey and go toward white
				if(Ogre::uchar((height-seaHeight)/(top-seaHeight)*250.0f) > 150)
				{
					tempVal = Ogre::uchar((height-seaHeight)/(top-seaHeight)*250.0f)-150;
					if(tempVal > 75)
						tempVal = 75;
					red = 180+tempVal;
					green = 180+tempVal;
					blue = 180+tempVal;
				}
			}

			// Write pixel to image
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3] = red;
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3+1] = green;
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3+2] = blue;
		}
	}
}

// Experimenting with some noise
void PSphere::deform(Ogre::Real seaFraction)
{
	Ogre::uint32 i, j, octaves, histogram[100] = {0}, histoTotal = 0;
	Ogre::Real height, top, bottom, seaHeight;
	Ogre::Vector3 vertex, unity;
	Ogre::Real frequencys[2], amplitudes[2];
	Ogre::uint32 accumulator = 0;

	// Variate height of a point in a sphere.
	octaves = 2;
	// Amplitude controls height of features
	amplitudes[0] = 0.15f;
	amplitudes[1] = 0.05f;
	/* frequency controls how wide features are. This is now actually
	 * inverse of frequency */
	frequencys[0] = 3.0f;
	frequencys[1] = 0.5f;

	/* highest possible feature height, assuming noise range exactly
	 * [-1 >= noise <= 1]. */
	top = 0.0f;
	for(i=0; i < octaves; i++)
		top += amplitudes[i];
	bottom = -top;

	for(i=0; i < vertexCount; i++)
	{
		vertex = vertexes[i];

		height = heightNoise(octaves, amplitudes, frequencys, vertex);

		unity = vertex;
		unity.normalise();  // direction vector that scalar height multiplies
		vertexes[i] = vertex + (unity * height);

		// Any better ways to create histograms?
		j = 0;
		while( height > (j/99.0f*(top-bottom) + bottom) )
			j++;
		histogram[j]++;

		histoTotal++;
	}

	// Find out histogram-bracket that exceeds wanted fraction of all values
	for(i=0; i < 100; i++)
	{
		accumulator += histogram[i];
		if(Ogre::Real(accumulator) > Ogre::Real(histoTotal)*seaFraction)
			break;
	}
	// Figure out offset with i
	seaHeight = Ogre::Real(i) / 99.0f * (top-bottom) + bottom;

	// Raise low elevations to sealevel
	for(i=0; i < vertexCount; i++)
	{
		if( vertexes[i].length() < (seaHeight + radius) )
		{
			// First make vertex unit length, then scale it by a scalar value.
			vertexes[i].normalise();
			vertexes[i] = vertexes[i]*(seaHeight+radius);
		}
	}

	// Procedurally generated image for texturing
	generateImage(octaves, amplitudes, frequencys, height, seaHeight, top, bottom);
}

void PSphere::calculateNormals()
{
	Ogre::uint32 i;
	Ogre::Vector3 p1, p2, normal;

	// Simple normal calculation, 1 normal for 3 vertices of a triangle
 /*   for(i=0; i < indexCount; i = i + 3)
	{
		p1 = vertexes[indexes[i]] - vertexes[indexes[i+1]];
		p2 = vertexes[indexes[i+1]] - vertexes[indexes[i+2]];
		normal = p1.crossProduct(p2);
		normal.normalise();
		vNorms[indexes[i]] = normal;
		vNorms[indexes[i+1]] = normal;
		vNorms[indexes[i+2]] = normal;
	}*/

	// Blended normals for a vertex
	// first, zero our normals, so that we can use it as a accumulator
	for(i=0; i < vertexCount; i++)
	{
		vNorms[i].x = 0.0f;
		vNorms[i].y = 0.0f;
		vNorms[i].z = 0.0f;
	}
	/* calculate normals for every triangle. Multiple normals are added together
	 * for each vertex, so the result is average of all the normals. */
	for(i=0; i < indexCount; i = i + 3)
	{
		p1 = vertexes[indexes[i]] - vertexes[indexes[i+1]];
		p2 = vertexes[indexes[i+1]] - vertexes[indexes[i+2]];
		normal = p1.crossProduct(p2);
		// FIXME: Probably more correct to normalize before adding, investigate later.
		vNorms[indexes[i]] += normal;
		vNorms[indexes[i+1]] += normal;
		vNorms[indexes[i+2]] += normal;
	}
	// Normalization pass
	for(i=0; i < vertexCount; i++)
	{
		vNorms[i].normalise();
	}
}

// Makes a sphere out of a cube that is made of 6 squares
void PSphere::create(Ogre::Real diameter, Ogre::Real seaFraction, Ogre::uint32 iters)
{
	Ogre::Vector3 vertex;
	Ogre::uint32 i, j, idx = 0;
	Ogre::uint32 *indexBuf;

	// Iters less than 3 are pointless
	if(iters < 3)
	{
		iters = 3;
		std::cout << "Sphere needs atleast 3 iters" <<std::endl;
	}

	radius = diameter/2.0f;

	vertexCount = 0;
	indexCount = 0;

	/* Calling Sphere::create more than once would cause memory leak if we
	 * wouldn't delete old allocations first */
	if(vertexes != NULL)
		delete[] vertexes;
	if(vNorms != NULL)
		delete[] vNorms;
	if(colours != NULL)
		delete[] colours;
	if(texCoords != NULL)
		delete[] texCoords;
	if(indexes != NULL)
		delete[] indexes;

	/* +iter*8 is for texture seam fix, duplicating some vertexes.
	 * Approximate, but should be on a safe side */
	vertexes =  new Ogre::Vector3[iters*iters*6 + iters*8];
	vNorms =    new Ogre::Vector3[iters*iters*6 + iters*8];
	colours =   new Ogre::ColourValue[iters*iters*6 + iters*8];
	texCoords = new Ogre::Vector2[iters*iters*6 + iters*8];
	indexes =   new Ogre::uint32[(iters-1)*(iters-1)*6*6];

	Ogre::Vector3 *vBuf = new Ogre::Vector3[iters*iters];   // Allocate memory for the square buffer
	indexBuf = new Ogre::uint32[(iters-1)*(iters-1)*6];     // Allocate index buffer for the square




	// Make a square in xy-plane, elevated in z-axis by diameter/2
	vertex.z = diameter/2.0f;

	for(i=0; i < iters; i++)
	{
		vertex.x = -diameter/2.0f + diameter * Ogre::Real(i) / Ogre::Real(iters-1);

		for(j=0; j < iters; j++)
		{
			vertex.y = -diameter/2.0f + diameter * Ogre::Real(j) / Ogre::Real(iters-1);

			vBuf[idx] = vertex;
			idx++;
		}
	}

	// Create indexes to build trianges. To make other side of a square visible reverse indexes.
	idx = 0;
	for(i=0; i < iters-1; i++)
	{
		for(j=0; j < iters-1; j++)
		{
			// Triangle 1
			indexBuf[idx]=i*iters+j;
			idx++;
			indexBuf[idx]=i*iters+j+iters+1;
			idx++;
			indexBuf[idx]=i*iters+j+1;
			idx++;

			// Triangle 2, in other words, a quad.
			indexBuf[idx]=i*iters+j+iters+1;
			idx++;
			indexBuf[idx]=i*iters+j;
			idx++;
			indexBuf[idx]=i*iters+j+iters;
			idx++;
		}
	}


	// Now just rotate the square to create a cube

	// Red face, +z
	for(i=0; i < iters*iters; i++)
	{
		calculate(vBuf[i], diameter/2.0f, Ogre::ColourValue(1.0f, 0.0f, 0.0f));
	}
	for(i=0; i < (iters-1)*(iters-1)*6; i++)
	{
		indexes[indexCount] = indexBuf[i];
		indexCount++;
	}

	// Green face, -z
	for(i=0; i < iters*iters; i++)
	{
		vertex.x =  vBuf[i].x;
		vertex.y =  vBuf[i].y;
		vertex.z = -vBuf[i].z;

		calculate(vertex, diameter/2.0f, Ogre::ColourValue(0.0f, 1.0f, 0.0f));
	}
	// Reverse
	// Iterate to 1 instead of zero to avoid unsigned number wraparound
	for(i=(iters-1)*(iters-1)*6; i >= 1; i--)
	{
		indexes[indexCount] = iters*iters+indexBuf[i-1];
		indexCount++;
	}

	// Blue face, +x
	for(i=0; i < iters*iters; i++)
	{
		vertex.x =  vBuf[i].z;
		vertex.y =  vBuf[i].x;
		vertex.z =  vBuf[i].y;

		calculate(vertex, diameter/2.0f, Ogre::ColourValue(0.0f, 0.0f, 1.0f));
	}
	for(i=0; i < (iters-1)*(iters-1)*6; i++)
	{
		indexes[indexCount] = iters*iters*2+indexBuf[i];
		indexCount++;
	}

	// Violet face, -x
	for(i=0; i < iters*iters; i++)
	{
		vertex.x = -vBuf[i].z;
		vertex.y =  vBuf[i].x;
		vertex.z =  vBuf[i].y;

		calculate(vertex, diameter/2.0f, Ogre::ColourValue(1.0f, 0.0f, 1.0f));
	}
	// Reverse
	for(i=(iters-1)*(iters-1)*6; i >= 1; i--)
	{
		indexes[indexCount] = iters*iters*3+indexBuf[i-1];
		indexCount++;
	}

	// Cyan face, +y
	for(i=0; i < iters*iters; i++)
	{
		vertex.x =  vBuf[i].x;
		vertex.y =  vBuf[i].z;
		vertex.z =  vBuf[i].y;

		calculate(vertex, diameter/2.0f, Ogre::ColourValue(0.0f, 1.0f, 1.0f));
	}
	// Reverse
	for(i=(iters-1)*(iters-1)*6; i >= 1; i--)
	{
		indexes[indexCount] = iters*iters*4+indexBuf[i-1];
		indexCount++;
	}

	// Yellow face, -y
	for(i=0; i < iters*iters; i++)
	{
		vertex.x =  vBuf[i].x;
		vertex.y = -vBuf[i].z;
		vertex.z =  vBuf[i].y;

		calculate(vertex, diameter/2.0f, Ogre::ColourValue(1.0f, 1.0f, 0.0f));
	}
	for(i=0; i < (iters-1)*(iters-1)*6; i++)
	{
		indexes[indexCount] = iters*iters*5+indexBuf[i];
		indexCount++;
	}

	// Release square buffers
	delete[] vBuf;
	delete[] indexBuf;

	deform(seaFraction);
	calculateNormals();
	fixTextureSeam(); // Call this after calculateNormals
}

/* Release resources allocated by Sphere class */
void PSphere::destroy()
{
	/* FIXME: Should we consider failing graciously in case of incorrect use like:
	 *
	 * Sphere oneSphere;
	 * oneSphere.destroy(); */

	delete[] colours;
	delete[] indexes;
	delete[] texCoords;
	delete[] vertexes;
	delete[] vNorms;
	delete[] image;
}

void PSphere::pushToOgre(Ogre::ManualObject *manual)
{
	Ogre::uint32 i, j;

	/* FIXME: Should probably bang* hardware buffers directly here */
	/* bang == use */
	manual->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for(i=0; i < vertexCount; i++)
	{
		manual->position(vertexes[i]);
		manual->normal(vNorms[i]);
		manual->colour(colours[i]);
		manual->textureCoord(texCoords[i]);
	}
	for(i=0; i < indexCount; i++)
	{
		manual->index(indexes[i]);
	}

	manual->end();

	// Texture stuff
	/* FIXME: pointers and stuff defined in here are lost after function returns.
	 * But do we need them/is it correct? */
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual("sphereTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, TEX_WIDTH, TEX_HEIGHT, 0, Ogre::PF_R8G8B8, Ogre:: TU_DYNAMIC);
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8 *surfaceTexture = static_cast<Ogre::uint8*>(pixelBox.data);

	for(i=0; i < TEX_HEIGHT; i++)
	{
		for(j=0; j < TEX_WIDTH; j++)
		{
			/* FIXME: Might be unnecessary memory copy, but was convenient. */
			/* TextureManager did not honor Ogre::PF_R8G8B8, so need to swap red and blue,
			 * plus hardware wants alfa channel values too */
			surfaceTexture[(i*TEX_WIDTH+j)*4]   = image[(i*TEX_WIDTH+j)*3+2];   // blue
			surfaceTexture[(i*TEX_WIDTH+j)*4+1] = image[(i*TEX_WIDTH+j)*3+1];   // green
			surfaceTexture[(i*TEX_WIDTH+j)*4+2] = image[(i*TEX_WIDTH+j)*3];     // red
			surfaceTexture[(i*TEX_WIDTH+j)*4+3] = 255;                          // Alfa
		}
	}

	pixelBuffer->unlock();

}

