#include <iostream>
#include <exception>
#include "OGRE/Ogre.h"
#include "PSphere.h"
#include "simplexnoise1234.h"
#include <OgreMeshSerializer.h>
#include <OgreDataStream.h>
#include <OgreException.h>
#include "OgreConfigFile.h"
#include "Common.h"
#include "ResourceParameter.h"
using namespace std;

// Let's set texture dimensions this way for a time being
#define TEX_WIDTH 1024
#define TEX_HEIGHT 512

PSphere::PSphere(){
	vertexes =	NULL;
	vNorms =	NULL;
	colours =	NULL;
	texCoords =	NULL;
	indexes =	NULL;
	image =		NULL;
	observer =	Ogre::Vector3(0.0f, 0.0f, 0.0f);
}

/* Set position for the observer. This must be position vector in modelspace,
 * not in worldspace. In other words, one must undo rotations. */
void PSphere::setObserverPosition(Ogre::Vector3 position)
{
	observer = position;
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

					// update index to point to the new vertex
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

/* Gives observer distance to the point on surface that is directly between
 * observer and planet origo.
 * Negative values mean that the observer is inside the planet */
Ogre::Real PSphere::getObserverDistanceToSurface()
{
	Ogre::Real frequencys[2], amplitudes[2], height;
	Ogre::Vector3 direction, surfacePos;
	Ogre::Real distance;

	// Hardcode these values for now, waiting for parameter class.
	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();
	amplitudes[0] = amplitude[0];
	amplitudes[1] = amplitude[1];
	frequencys[0] = frequency[0];
	frequencys[1] = frequency[1];

	// normal vector that points from the origo to the observer
	direction = observer.normalisedCopy();
	/* Get position of the surface along the line that goes from
	 * the planet origo to the observer */
	height = heightNoise(2, amplitudes, frequencys, direction);
	surfacePos = direction*(height*radius + radius);

	distance = fabsf(observer.length()) - fabsf(surfacePos.length());

	return distance;
}

/* Example that shows procedural generation of textures */
void PSphere::generateImage(Ogre::Real seaHeight, Ogre::Real top, Ogre::Real bottom)
{
	Ogre::Vector3 spherePoint;
	Ogre::Real latitude, longitude;
	Ogre::Real height, amplitudes[2], frequencys[2];
	Ogre::uint32 x, y, octaves;
	Ogre::uint8 red, green, blue, tempVal;
	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	unsigned char waterFirstColorblue = (unsigned char)"";
	unsigned char waterFirstColorgreen = (unsigned char)"";
	unsigned char waterFirstColorred = (unsigned char)"";
	unsigned char waterSecondColorblue = (unsigned char)"";
	unsigned char waterSecondColorgreen = (unsigned char)"";
	unsigned char waterSecondColorred = (unsigned char)"";
	RParameter.getWaterFirstColor(waterFirstColorred,waterFirstColorgreen,waterFirstColorblue);
	RParameter.getWaterFirstColor(waterSecondColorblue,waterSecondColorgreen,waterSecondColorblue);

	// Variate height of a point in a sphere.
	octaves = 2;
	// Amplitude controls height of features
	amplitudes[0] = amplitude[0];
	amplitudes[1] = amplitude[1];
	/* frequency controls how wide features are. This is now actually
	 * inverse of frequency */
	frequencys[0] = frequency[0];
	frequencys[1] = frequency[1];

	// Guard against multiple memory allocations to avoid memory leaks
	if(image != NULL)
		delete[] image;
	image = new unsigned char[TEX_WIDTH*TEX_HEIGHT*3];

	for(y=0; y < TEX_HEIGHT; y++)
	{
		for(x=0; x < TEX_WIDTH; x++)
		{
			longitude = (Ogre::Real(x)+0.5f)/TEX_WIDTH*360.0f;
			latitude = (90.0f-0.5f/TEX_HEIGHT) - (Ogre::Real(y)+0.5f)/TEX_HEIGHT*180.0f;

			// Get a point that corresponds to a given pixel
			spherePoint = convertSphericalToCartesian(latitude, longitude);

			// Get height of a point
			height = heightNoise(octaves, amplitudes, frequencys, spherePoint);

			// Set sea-colors, deeper part is slighly deeper blue.
			if(height < seaHeight)
			{
				red = 100-Ogre::uchar((seaHeight-height)/(seaHeight-bottom)*50.0f);
				green = 255-Ogre::uchar((seaHeight-height)/(seaHeight-bottom)*50.0f);
				blue = waterFirstColorblue;
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
					blue = 180+waterSecondColorblue;
				}
			}

			// Write pixel to image
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3] = red;
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3+1] = green;
			image[((TEX_HEIGHT-1-y)*TEX_WIDTH+x)*3+2] = blue;
		}
	}
}

void PSphere::deform(HeightMap *map)
{
	unsigned int x, y, octaves;
	Ogre::Vector3 spherePos;
	Ogre::Real height;
	Ogre::Real frequencys[2], amplitudes[2];

	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	// Variate height of a point in a sphere.
	octaves = 2;
	// Amplitude controls height of features
	amplitudes[0] = amplitude[0];
	amplitudes[1] = amplitude[1];
	/* frequency controls how wide features are. This is now actually
	 * inverse of frequency */
	frequencys[0] = frequency[0];
	frequencys[1] = frequency[1];

	for(x=0; x < map->getSize(); x++)
	{
		for(y=0; y < map->getSize(); y++)
		{
			spherePos = map->projectToSphere(x, y);
			height = heightNoise(octaves, amplitudes, frequencys, spherePos);
			map->setHeight(x, y, height);
		}

	}
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

void PSphere::calculateSeaLevel(float &seaLevel, float &minElev, float &maxElev, float seaFraction)
{
	Ogre::uint32 i, accumulator=0, histoTotal;

	unsigned int histogram[100]={0};
	float min[6], max[6];

	// Assume all faces have similar height variations
	faceYP->getHistogram(histogram, 100);
	faceXM->getHistogram(histogram, 100);
	faceYM->getHistogram(histogram, 100);
	faceXP->getHistogram(histogram, 100);
	faceZP->getHistogram(histogram, 100);
	faceZM->getHistogram(histogram, 100);

	faceYP->getMinMax(min[0], max[0]);
	faceXM->getMinMax(min[1], max[1]);
	faceYM->getMinMax(min[2], max[2]);
	faceXP->getMinMax(min[3], max[3]);
	faceZP->getMinMax(min[4], max[4]);
	faceZM->getMinMax(min[5], max[5]);

	minElev = min[0];
	maxElev = max[0];
	for(i=1; i < 6; i++)
	{
		if(minElev > min[i])
			minElev = min[i];
		if(maxElev < max[i])
			maxElev = max[i];
	}
	// Find out histogram-bracket that exceeds wanted fraction of all values
	histoTotal = faceYP->getSize();
	histoTotal *= histoTotal*6;

	for(i=0; i < 100; i++)
	{
		accumulator += histogram[i];
		if(Ogre::Real(accumulator) > Ogre::Real(histoTotal)*seaFraction)
			break;
	}
	// Figure out offset with i
	seaLevel = Ogre::Real(i) / 99.0f * (maxElev-minElev) + minElev;

}

void PSphere::smoothSeaArea(float seaHeight)
{
	faceYP->setToMinimumHeight(seaHeight);
	faceXM->setToMinimumHeight(seaHeight);
	faceYM->setToMinimumHeight(seaHeight);
	faceXP->setToMinimumHeight(seaHeight);
	faceZP->setToMinimumHeight(seaHeight);
	faceZM->setToMinimumHeight(seaHeight);
}


// Makes a sphere out of a cube that is made of 6 squares
void PSphere::create(Ogre::Real diameter, Ogre::Real seaFraction, Ogre::uint32 iters, ResourceParameter resourceParameter)
{
	RParameter = resourceParameter;
	// Iters less than 3 are pointless
	if(iters < 3)
	{
		iters = 3;
		std::cout << "Sphere needs atleast 3 iters" << std::endl;
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
	vertexes =	new Ogre::Vector3[iters*iters*6 + iters*8];
	vNorms =	new Ogre::Vector3[iters*iters*6 + iters*8];
	colours =	new Ogre::ColourValue[iters*iters*6 + iters*8];
	texCoords =	new Ogre::Vector2[iters*iters*6 + iters*8];
	indexes =	new Ogre::uint32[(iters-1)*(iters-1)*6*6];

	// No rotation
	faceYP = new HeightMap(iters, Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	// 90 degrees through z-axis
	faceXM = new HeightMap(iters, Ogre::Matrix3(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	// 180 degrees through z-axis
	faceYM = new HeightMap(iters, Ogre::Matrix3(-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	// 270 degrees through z-axis
	faceXP = new HeightMap(iters, Ogre::Matrix3(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	// 90 degrees through x-axis
	faceZP = new HeightMap(iters, Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));
	// 270 degrees through x-axis
	faceZM = new HeightMap(iters, Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f));

	deform(faceYP);
	deform(faceXM);
	deform(faceYM);
	deform(faceXP);
	deform(faceZP);
	deform(faceZM);

	float seaHeight, min, max;

	calculateSeaLevel(seaHeight, min, max, seaFraction);
	generateImage(seaHeight, max, min);
	smoothSeaArea(seaHeight);
}

/* Release resources allocated by Sphere class */
void PSphere::destroy()
{
	delete[] colours;
	delete[] indexes;
	delete[] texCoords;
	delete[] vertexes;
	delete[] vNorms;
	delete[] image;
}

void PSphere::generateMeshData()
{
	unsigned int iters;

	faceYP->generateMeshData(vertexes, texCoords, indexes, radius);
	// Assuming all faces have same size
	iters = faceYP->getSize();
	indexCount = (iters-1)*(iters-1)*6;
	vertexCount = iters*iters;
	faceXM->generateMeshData(&vertexes[vertexCount], &texCoords[vertexCount],
							 &indexes[indexCount], radius);
	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceYM->generateMeshData(&vertexes[vertexCount], &texCoords[vertexCount],
							 &indexes[indexCount], radius);
	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceXP->generateMeshData(&vertexes[vertexCount], &texCoords[vertexCount],
							 &indexes[indexCount], radius);
	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceZP->generateMeshData(&vertexes[vertexCount], &texCoords[vertexCount],
							 &indexes[indexCount], radius);
	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceZM->generateMeshData(&vertexes[vertexCount], &texCoords[vertexCount],
							 &indexes[indexCount], radius);
	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;

	// Modify index-values, because same vertex-array is shared between faces.
	unsigned int faceNum, i;
	for(i=0; i < indexCount; i++)
	{
		faceNum = i / ( (iters-1)*(iters-1)*6 );
		indexes[i] += faceNum*iters*iters;
	}

	calculateNormals();
	fixTextureSeam(); // Call this after calculateNormals
}

void PSphere::loadToBuffers(const std::string &meshName, const std::string &textureName)
{
	Ogre::uint32 i, j;

	generateMeshData();

	// Create mesh and subMesh
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton()
			.createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SubMesh *subMesh = mesh->createSubMesh();

	mesh->sharedVertexData = new Ogre::VertexData;

	// Pointer to declaration of vertexData
	Ogre::VertexDeclaration* vertexDecl = mesh->sharedVertexData->vertexDeclaration;

	mesh->sharedVertexData->vertexCount = vertexCount;

	// define elements position, normal and tex coordinate
	vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vertexDecl->addElement(0, sizeof(float)*3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	vertexDecl->addElement(0, sizeof(float)*6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

	// Vertex buffer
	Ogre::HardwareVertexBufferSharedPtr vBuf = Ogre::HardwareBufferManager::getSingleton()
			.createVertexBuffer(8*sizeof(float), vertexCount,
								Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	mesh->sharedVertexData->vertexBufferBinding->setBinding(0, vBuf);

	// Lock the buffer and write vertex data to it
	float *pVertex = static_cast<float *>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	for(i=0; i < vertexCount; i++)
	{
		pVertex[i*8+0] = vertexes[i].x;
		pVertex[i*8+1] = vertexes[i].y;
		pVertex[i*8+2] = vertexes[i].z;

		pVertex[i*8+3] = vNorms[i].x;
		pVertex[i*8+4] = vNorms[i].y;
		pVertex[i*8+5] = vNorms[i].z;

		pVertex[i*8+6] = texCoords[i].x;
		pVertex[i*8+7] = texCoords[i].y;
	}
	vBuf->unlock();

	// Index buffer
	Ogre::HardwareIndexBufferSharedPtr iBuf = Ogre::HardwareBufferManager::getSingleton()
			.createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT, indexCount,
							   Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	// Lock index buffer
	unsigned int *pIdx = static_cast<unsigned int *>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	for(i=0; i < indexCount; i++)
	{
		pIdx[i] = indexes[i];
	}
	iBuf->unlock();

	subMesh->useSharedVertices = true;
	subMesh->indexData->indexBuffer = iBuf;
	subMesh->indexData->indexCount = indexCount;
	subMesh->indexData->indexStart = 0;

	mesh->_setBounds(Ogre::AxisAlignedBox(-radius, -radius, -radius, radius, radius, radius));
	mesh->_setBoundingSphereRadius(radius);

	mesh->load();

	this->mesh = mesh;

	// Texture stuff
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton()
			.createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						  Ogre::TEX_TYPE_2D, TEX_WIDTH, TEX_HEIGHT, 0, Ogre::PF_R8G8B8, Ogre:: TU_DYNAMIC);
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

void PSphere::loadMeshFile(const std::string &path, const std::string &meshName) {
	Ogre::String source;
	source = path;
	FILE* pFile = fopen( source.c_str(), "rb" );
	if (!pFile)
		OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND,"File " + source + " not found.", "OgreMeshLoaded");
	struct stat tagStat;
	stat( source.c_str(), &tagStat );
	Ogre::MemoryDataStream* memstream = new Ogre::MemoryDataStream(source, tagStat.st_size, true);
	fread( (void*)memstream->getPtr(), tagStat.st_size, 1, pFile );
	fclose( pFile );
	Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton().createManual(meshName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::MeshSerializer meshSerializer;
	Ogre::DataStreamPtr stream(memstream);
	meshSerializer.importMesh(stream, pMesh.getPointer());
}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &objectName, Ogre::Real x, Ogre::Real y, Ogre::Real z) {
	Ogre::Entity *entity = scene->createEntity("LocalMesh_Ent", objectName);
	Ogre::SceneNode *cube = node->createChildSceneNode(objectName, Ogre::Vector3(x, y, z));
	cube->attachObject(entity);
}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude) {
	Ogre::Vector3 cart_coord = convertSphericalToCartesian(latitude, longitude);
	Ogre::Real x = radius*1.2*cart_coord.x;
	Ogre::Real y = radius*1.2*cart_coord.y;
	Ogre::Real z = radius*1.2*cart_coord.z;
	this->attachMesh(node, scene, objectName, x, y, z);

}

Ogre::MeshPtr PSphere::getMesh(){
	return mesh;
}

