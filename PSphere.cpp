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

#include <iostream>
#include <exception>
#include <string>
#include <stdlib.h>
#include "ObjectInfo.h"
#include <vector>
#include "OGRE/Ogre.h"
#include "PSphere.h"
#include "simplexnoise1234.h"
#include <OgreMeshSerializer.h>
#include <OgreDataStream.h>
#include <OgreException.h>
#include "OgreConfigFile.h"
#include "Common.h"
#include "ResourceParameter.h"
#include <qdebug.h>
#define FREEIMAGE_LIB
#include "FreeImage.h"

using namespace std;

// Let's set texture dimensions this way for a time being
#define TEX_WIDTH 1024
#define TEX_HEIGHT 512
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

PSphere::PSphere(Ogre::uint32 iters, Ogre::uint32 gridSize, ResourceParameter resourceParameter){
	vertexes =	NULL;
	vNorms =	NULL;
	texCoords =	NULL;
	indexes =	NULL;
	surfaceTexture =		NULL;
	exportImage =	NULL;
	observer =	Ogre::Vector3(0.0f, 0.0f, 0.0f);

	create(iters, gridSize, resourceParameter);
}

PSphere::~PSphere()
{
	delete[] indexes;
	delete[] texCoords;
	delete[] vertexes;
	delete[] vNorms;
	delete[] surfaceTexture;
	delete[] exportImage;

	delete faceXM;
	delete faceXP;
	delete faceYM;
	delete faceYP;
	delete faceZM;
	delete faceZP;
}

/* Set position for the observer. This must be position vector in modelspace,
 * not in worldspace. In other words, one must undo rotations. */
void PSphere::setObserverPosition(Ogre::Vector3 position)
{
	observer = position;
}

Ogre::Real PSphere::heightNoise(vector<float> amplitude,
							   vector<float> frequency, Ogre::Vector3 Point)
{
	Ogre::uint32 i;
	Ogre::Real height = 0.0f;

	// Run through the amplitude.size
	for(i=0; i < amplitude.size(); i++)
	{
		height += amplitude[i] * SimplexNoise1234::noise(Point.x/frequency[i],
														  Point.y/frequency[i],
														  Point.z/frequency[i]);
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
	Ogre::Real height;
	Ogre::Vector3 direction, surfacePos;
	Ogre::Real distance;

	// Hardcode these values for now, waiting for parameter class.
	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	// normal vector that points from the origo to the observer
	direction = observer.normalisedCopy();
	/* Get position of the surface along the line that goes from
	 * the planet origo to the observer */
	height = heightNoise(amplitude, frequency, direction + randomTranslate);
	surfacePos = direction*(height*radius + radius);

	distance = fabsf(observer.length()) - fabsf(surfacePos.length());

	return distance;
}

/* Get the Height of a particular position of the surface.
 * Position is a 3-d vector over the surface which you need to know the height
 * return the height */
Ogre::Real PSphere::getSurfaceHeight(Ogre::Vector3 Position)
{
	Ogre::Real height;
	Ogre::Vector3 direction, surfacePos;
	Ogre::Real distance;

	// Hardcode these values for now, waiting for parameter class.
	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	// normal vector that points from the origo to the observer
	direction = Position.normalisedCopy();
	/* Get position of the surface along the line that goes from
	 * the planet origo to the observer */
	height = heightNoise(amplitude, frequency, direction + randomTranslate);
	surfacePos = direction*(height*radius + radius);

	//distance = fabsf(observer.length()) - fabsf(surfacePos.length());

	return surfacePos.length();
}
/*
*return radius
*/
Ogre::Real PSphere::getRadius()
{
	return radius;
}

/* Generates surface-texturemap using noise-generated height differences.
 * Expects pointer to be already correctly allocated. */
void PSphere::generateImage(unsigned short textureWidth, unsigned short textureHeight, unsigned char *image)
{
	Ogre::Vector3 spherePoint;
	Ogre::Real latitude, longitude;
	Ogre::Real height;
	Ogre::uint32 x, y;
	Ogre::uint8 red, green, blue;
	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();
	float const multiplyer = 0.6;

	unsigned char waterFirstColorblue = 0;
	unsigned char waterFirstColorgreen = 0;
	unsigned char waterFirstColorred = 0;
	unsigned char waterSecondColorblue = 0;
	unsigned char waterSecondColorgreen = 0;
	unsigned char waterSecondColorred = 0;
	RParameter.getWaterFirstColor(waterFirstColorred,waterFirstColorgreen,waterFirstColorblue);
	RParameter.getWaterSecondColor(waterSecondColorred,waterSecondColorgreen,waterSecondColorblue);

	unsigned char terrainFirstColorblue = 0;
	unsigned char terrainFirstColorgreen = 0;
	unsigned char terrainFirstColorred = 0;
	unsigned char terrainSecondColorblue = 0;
	unsigned char terrainSecondColorgreen = 0;
	unsigned char terrainSecondColorred = 0;
	RParameter.getTerrainFirstColor(terrainFirstColorred,terrainFirstColorgreen,terrainFirstColorblue);
	RParameter.getTerrainSecondColor(terrainSecondColorred,terrainSecondColorgreen,terrainSecondColorblue);

	unsigned char mountainFirstColorblue = 0;
	unsigned char mountainFirstColorgreen = 0;
	unsigned char mountainFirstColorred = 0;
	unsigned char mountainSecondColorblue = 0;
	unsigned char mountainSecondColorgreen = 0;
	unsigned char mountainSecondColorred = 0;
	RParameter.getMountainFirstColor(mountainFirstColorred,mountainFirstColorgreen,mountainFirstColorblue);
	RParameter.getMountainSecondColor(mountainSecondColorred,mountainSecondColorgreen,mountainSecondColorblue);

	for(y=0; y < textureHeight; y++)
	{
		for(x=0; x < textureWidth; x++)
		{
			longitude = (Ogre::Real(x)+0.5f)/textureWidth*360.0f;
			latitude = (90.0f-0.5f/textureHeight) - (Ogre::Real(y)+0.5f)/textureHeight*180.0f;

			// Get a point that corresponds to a given pixel
			spherePoint = convertSphericalToCartesian(latitude, longitude);

			// Get height of a point
			height = heightNoise(amplitude, frequency, spherePoint + randomTranslate);

			// Set sea-colors, deeper part is slighly deeper blue.
			if(height < seaHeight)
			{
				red =  waterFirstColorred + (waterSecondColorred-waterFirstColorred)*(height-minimumHeight)/(seaHeight-minimumHeight);
				green =  waterFirstColorgreen + (waterSecondColorgreen-waterFirstColorgreen)*(height-minimumHeight)/(seaHeight-minimumHeight);
				blue =  waterFirstColorblue + (waterSecondColorblue-waterFirstColorblue)*(height-minimumHeight)/(seaHeight-minimumHeight);
			}
			else
			{
				// Set low elevations green and higher brown
				red =  terrainFirstColorred + (terrainSecondColorred-terrainFirstColorred)*(height-seaHeight)/(maximumHeight*multiplyer-seaHeight);
				green =  terrainFirstColorgreen + (terrainSecondColorgreen-terrainFirstColorgreen)*(height-seaHeight)/(maximumHeight*multiplyer-seaHeight);
				blue =  terrainFirstColorblue + (terrainSecondColorblue-terrainFirstColorblue)*(height-seaHeight)/(maximumHeight*multiplyer-seaHeight);
				// Highest elevations are bright grey and go toward white
				if(height > maximumHeight * multiplyer)
				{
					// to avoid unsigned char overflow
					float substractred = (float)mountainSecondColorred - (mountainSecondColorred-mountainFirstColorred)*(maximumHeight-height)/(maximumHeight);
					float substractgreen = (float)mountainSecondColorgreen - (mountainSecondColorgreen-mountainFirstColorgreen)*(maximumHeight-height)/(maximumHeight);
					float substractblue = (float)mountainSecondColorblue - (mountainSecondColorblue-mountainFirstColorblue)*(maximumHeight-height)/(maximumHeight);
					substractred < 0.0f ? substractred = 0.0f : (substractred >= 256.0f ? substractred = 255.0f : substractred);
					substractgreen < 0.0f ? substractgreen = 0.0f : (substractgreen >= 256.0f ? substractgreen = 255.0f : substractgreen);
					substractblue < 0.0f ? substractblue = 0.0f : (substractblue >= 256.0f ? substractblue = 255.0f : substractblue);
					red =  substractred;
					green =  substractgreen;
					blue =  substractblue;
				}
			}

			// Write pixel to image
			image[((textureHeight-1-y)*textureWidth+x)*3] = red;
			image[((textureHeight-1-y)*textureWidth+x)*3+1] = green;
			image[((textureHeight-1-y)*textureWidth+x)*3+2] = blue;
		}
	}
}

void PSphere::deform(HeightMap *map)
{
	unsigned int x, y;
	Ogre::Vector3 spherePos;
	Ogre::Real height;

	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	for(x=0; x < map->getSize(); x++)
	{
		for(y=0; y < map->getSize(); y++)
		{
			spherePos = map->projectToSphere(x, y);
			height = heightNoise(amplitude, frequency, spherePos + randomTranslate);
			map->setHeight(x, y, height);
		}
	}
}

void PSphere::setGridLandInfo(Grid *grid)
{
	unsigned int x, y;
	Ogre::Vector3 spherePos;
	Ogre::Real height;

	vector <float> frequency = RParameter.getFrequency();
	vector <float> amplitude = RParameter.getAmplitude();

	for(x=0; x < grid->getSize(); x++)
	{
		for(y=0; y < grid->getSize(); y++)
		{
			spherePos = grid->projectToSphere(x, y);
			height = heightNoise(amplitude, frequency, spherePos + randomTranslate);

			if (height > seaHeight)
				grid->setValue(x, y, 1);
			else
				grid->setValue(x, y, 0);
		}
	}
}

void PSphere::calculateSeaLevel(float &minElev, float &maxElev, float seaFraction)
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
	seaHeight = Ogre::Real(i) / 99.0f * (maxElev-minElev) + minElev;

}

void PSphere::smoothSeaArea()
{
	faceYP->setToMinimumHeight(seaHeight);
	faceXM->setToMinimumHeight(seaHeight);
	faceYM->setToMinimumHeight(seaHeight);
	faceXP->setToMinimumHeight(seaHeight);
	faceZP->setToMinimumHeight(seaHeight);
	faceZM->setToMinimumHeight(seaHeight);
}


// Makes a sphere out of a cube that is made of 6 squares
void PSphere::create(Ogre::uint32 iters, Ogre::uint32 gridSize, ResourceParameter resourceParameter)
{
	RParameter = resourceParameter;
	float waterFraction = resourceParameter.getWaterFraction();
	// Iters less than 3 are pointless
	if(iters < 3)
	{
		iters = 3;
		std::cout << "Sphere needs atleast 3 iters" << std::endl;
	}

	radius = resourceParameter.getRadius();

	vertexCount = 0;
	indexCount = 0;

	/* Calling Sphere::create more than once would cause memory leak if we
	 * wouldn't delete old allocations first */
	if(vertexes != NULL)
		delete[] vertexes;
	if(vNorms != NULL)
		delete[] vNorms;
	if(texCoords != NULL)
		delete[] texCoords;
	if(indexes != NULL)
		delete[] indexes;

	/* +iter*8 is for texture seam fix, duplicating some vertexes.
	 * Approximate, but should be on a safe side */
	vertexes =	new Ogre::Vector3[iters*iters*6 + iters*8];
	vNorms =	new Ogre::Vector3[iters*iters*6 + iters*8];
	texCoords =	new Ogre::Vector2[iters*iters*6 + iters*8];
	indexes =	new Ogre::uint32[(iters-1)*(iters-1)*6*6];

	Ogre::Matrix3 noRot, rotZ_90, rotZ_180, rotZ_270, rotX_90, rotX_270;

	noRot = Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	rotZ_90 = Ogre::Matrix3(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	rotZ_180 = Ogre::Matrix3(-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	rotZ_270 = Ogre::Matrix3(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	rotX_90 = Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	rotX_270 = Ogre::Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);

	// No rotation
	faceYP = new HeightMap(iters, noRot);
	gridYP = new Grid(gridSize, noRot);
	// 90 degrees through z-axis
	faceXM = new HeightMap(iters, rotZ_90);
	gridXM = new Grid(gridSize, rotZ_90);
	// 180 degrees through z-axis
	faceYM = new HeightMap(iters, rotZ_180);
	gridYM = new Grid(gridSize, rotZ_180);
	// 270 degrees through z-axis
	faceXP = new HeightMap(iters, rotZ_270);
	gridXP = new Grid(gridSize, rotZ_270);
	// 90 degrees through x-axis
	faceZP = new HeightMap(iters, rotX_90);
	gridZP = new Grid(gridSize, rotX_90);
	// 270 degrees through x-axis
	faceZM = new HeightMap(iters, rotX_270);
	gridZM = new Grid(gridSize, rotX_270);

	faceYP->setNeighbours(faceXM, faceXP, faceZP, faceZM);
	faceXM->setNeighbours(faceYM, faceYP, faceZP, faceZM);
	faceYM->setNeighbours(faceXP, faceXM, faceZP, faceZM);
	faceXP->setNeighbours(faceYP, faceYM, faceZP, faceZM);
	faceZP->setNeighbours(faceXM, faceXP, faceYM, faceYP);
	faceZM->setNeighbours(faceXM, faceXP, faceYP, faceYM);

	gridYP->setNeighbours(gridXM, gridXP, gridZP, gridZM);
	gridXM->setNeighbours(gridYM, gridYP, gridZP, gridZM);
	gridYM->setNeighbours(gridXP, gridXM, gridZP, gridZM);
	gridXP->setNeighbours(gridYP, gridYM, gridZP, gridZM);
	gridZP->setNeighbours(gridXM, gridXP, gridYM, gridYP);
	gridZM->setNeighbours(gridXM, gridXP, gridYP, gridYM);

	srand(RParameter.getSeed());
	randomTranslate.x = (float)((rand() % 1000)-500)/100.0f;
	randomTranslate.y = (float)((rand() % 1000)-500)/100.0f;
	randomTranslate.z = (float)((rand() % 1000)-500)/100.0f;

	deform(faceYP);
	deform(faceXM);
	deform(faceYM);
	deform(faceXP);
	deform(faceZP);
	deform(faceZM);

	calculateSeaLevel(minimumHeight, maximumHeight, waterFraction);

	surfaceTexture = new unsigned char[TEX_WIDTH*TEX_HEIGHT*3];
	generateImage(TEX_WIDTH, TEX_HEIGHT, surfaceTexture);//take longtime

	// Requires variable seaHeight that is set by calculateSeaLevel
	setGridLandInfo(gridYP);
	setGridLandInfo(gridXM);
	setGridLandInfo(gridYM);
	setGridLandInfo(gridXP);
	setGridLandInfo(gridZP);
	setGridLandInfo(gridZM);

	smoothSeaArea();
}

void PSphere::generateMeshData()
{
	unsigned int iters;

	faceYP->generateMeshData(radius);
	faceXM->generateMeshData(radius);
	faceYM->generateMeshData(radius);
	faceXP->generateMeshData(radius);
	faceZP->generateMeshData(radius);
	faceZM->generateMeshData(radius);

	// Meshes must be done before trying to blend normals
	faceYP->blendNormalsWithNeighbours();
	faceXM->blendNormalsWithNeighbours();
	faceYM->blendNormalsWithNeighbours();
	faceXP->blendNormalsWithNeighbours();
	faceZP->blendNormalsWithNeighbours();
	faceZM->blendNormalsWithNeighbours();

	faceYP->outputMeshData(vertexes, vNorms, texCoords, indexes);
	// Assuming all faces have same size
	iters = faceYP->getSize();
	indexCount = (iters-1)*(iters-1)*6;
	vertexCount = iters*iters;
	faceXM->outputMeshData(&vertexes[vertexCount], &vNorms[vertexCount], &texCoords[vertexCount], &indexes[indexCount]);

	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceYM->outputMeshData(&vertexes[vertexCount], &vNorms[vertexCount], &texCoords[vertexCount], &indexes[indexCount]);

	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceXP->outputMeshData(&vertexes[vertexCount], &vNorms[vertexCount], &texCoords[vertexCount], &indexes[indexCount]);

	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceZP->outputMeshData(&vertexes[vertexCount], &vNorms[vertexCount], &texCoords[vertexCount], &indexes[indexCount]);

	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;
	faceZM->outputMeshData(&vertexes[vertexCount], &vNorms[vertexCount], &texCoords[vertexCount], &indexes[indexCount]);

	indexCount += (iters-1)*(iters-1)*6;
	vertexCount += iters*iters;

	// Modify index-values, because same vertex-array is shared between faces.
	unsigned int faceNum, i;
	for(i=0; i < indexCount; i++)
	{
		faceNum = i / ( (iters-1)*(iters-1)*6 );
		indexes[i] += faceNum*iters*iters;
	}

	fixTextureSeam();
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
	Ogre::uint8 *exteriorTexture = static_cast<Ogre::uint8*>(pixelBox.data);

	for(i=0; i < TEX_HEIGHT; i++)
	{
		for(j=0; j < TEX_WIDTH; j++)
		{
			/* FIXME: Might be unnecessary memory copy, but was convenient. */
			/* TextureManager did not honor Ogre::PF_R8G8B8, so need to swap red and blue,
			 * plus hardware wants alfa channel values too */
			exteriorTexture[(i*TEX_WIDTH+j)*4]   = surfaceTexture[(i*TEX_WIDTH+j)*3+2];   // blue
			exteriorTexture[(i*TEX_WIDTH+j)*4+1] = surfaceTexture[(i*TEX_WIDTH+j)*3+1];   // green
			exteriorTexture[(i*TEX_WIDTH+j)*4+2] = surfaceTexture[(i*TEX_WIDTH+j)*3];     // red
			exteriorTexture[(i*TEX_WIDTH+j)*4+3] = 255;                          // Alfa
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

bool PSphere::checkIfObjectIsIn (std::string &objectName) {
	// Check if the object if already attached to the planet. Or at least is in the list of objects (the vector objects)
	for (vector<ObjectInfo>::iterator it = objects.begin() ; it != objects.end(); ++it) {
		ObjectInfo objTemp = *it;
		if (objTemp.getObjectName().compare(objectName) == 0) {
			return true;
		}
	}
	return false;
}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real x, Ogre::Real y, Ogre::Real z) {
	int temp_int = 0;
	string newName = meshName;
	string result;
	string delimiter = ".";
	string nameWithoutFormat = newName.substr(0, newName.find(delimiter)); // Remove the format from the name (the part of the name after the ".")
	string finalName = nameWithoutFormat;
	while (checkIfObjectIsIn(finalName)) { 
		// If the name has already been used it change it adding an auto-increased number in the end
		temp_int++;
		ostringstream convert;
		convert << temp_int;
		string result = convert.str();
		finalName = nameWithoutFormat+result;
	}

	Ogre::Vector3 position = Ogre::Vector3(x, y, z);
	Ogre::Entity *entity = scene->createEntity(finalName, meshName);
	Ogre::SceneNode *cube = node->createChildSceneNode(finalName, position);
	ObjectInfo object = ObjectInfo(position, finalName, node);
	objects.push_back(object);
	cube->attachObject(entity);

}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real x, Ogre::Real y, Ogre::Real z) {
	int temp_int = 0;
	string newName = objectName;
	string result;
	string delimiter = ".";
	string nameWithoutFormat = newName.substr(0, newName.find(delimiter)); // Remove the format from the name (the part of the name after the ".")
	string finalName = nameWithoutFormat;
	while (checkIfObjectIsIn(finalName)) { 
		// If the name has already been used it change it adding an auto-increased number in the end
		temp_int++;
		ostringstream convert;
		convert << temp_int;
		string result = convert.str();
		finalName = nameWithoutFormat+result;
	}

	Ogre::Vector3 position = Ogre::Vector3(x, y, z);
	Ogre::Entity *entity = scene->createEntity(finalName, meshName);
	Ogre::SceneNode *cube = node->createChildSceneNode(finalName, position);
	ObjectInfo object = ObjectInfo(position, finalName, node);
	objects.push_back(object);
	cube->attachObject(entity);

	
}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude) {
	Ogre::Vector3 cart_coord = convertSphericalToCartesian(latitude, longitude);
	Ogre::Real x = radius*2*cart_coord.x;
	Ogre::Real y = radius*2*cart_coord.y;
	Ogre::Real z = radius*2*cart_coord.z;

	this->attachMesh(node, scene, meshName, objectName, x, y, z);
}

void PSphere::attachMesh(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, Ogre::Real latitude, Ogre::Real longitude) {
	Ogre::Vector3 cart_coord = convertSphericalToCartesian(latitude, longitude);
	Ogre::Real x = radius*1.2*cart_coord.x;
	Ogre::Real y = radius*1.2*cart_coord.y;
	Ogre::Real z = radius*1.2*cart_coord.z;
	this->attachMesh(node, scene, meshName, x, y, z);

}
/*
*To skip collision detection while moving object, I try to attach it on the ground and set the initial orientation
*Maybe later we need object in air or outer space, so I leave attachmesh() and create this function which could put the object on ground.
*/
void PSphere::attachMeshOnGround(Ogre::SceneNode *node, Ogre::SceneManager *scene, const std::string &meshName, const std::string &objectName, Ogre::Real latitude, Ogre::Real longitude) {
	Ogre::Vector3 cart_coord = convertSphericalToCartesian(latitude, longitude);
	Ogre::Real x = radius*cart_coord.x;
	Ogre::Real y = radius*cart_coord.y;
	Ogre::Real z = radius*cart_coord.z;

	int temp_int = 0;
	string newName = objectName;
	string result;
	string delimiter = ".";
	string nameWithoutFormat = newName.substr(0, newName.find(delimiter)); // Remove the format from the name (the part of the name after the ".")
	string finalName = nameWithoutFormat;
	while (checkIfObjectIsIn(finalName)) { 
		// If the name has already been used it change it adding an auto-increased number in the end
		temp_int++;
		ostringstream convert;
		convert << temp_int;
		string result = convert.str();
		finalName = nameWithoutFormat+result;
	}

	Ogre::Vector3 position = Ogre::Vector3(x, y, z);
	Ogre::Real surfaceHeight = getSurfaceHeight(position);
	Ogre::Entity *entity = scene->createEntity(finalName, meshName);
	Ogre::SceneNode *cube = node->createChildSceneNode(finalName);
	cube->attachObject(entity);

	//cube->_updateBounds();
	//float objectSize=cube->_getWorldAABB().getSize().length();
	//float ratio = (surfaceHeight+objectSize/2 )/position.length();
	float ratio = (surfaceHeight )/position.length();
	position = position*ratio;
	cube->setPosition( position );

	//change orientation
	Ogre::Quaternion q = Ogre::Vector3::UNIT_Y.getRotationTo(position);
	cube->setOrientation( q );

	ObjectInfo object = ObjectInfo(position, finalName, node);
	objects.push_back(object);
}


Ogre::MeshPtr PSphere::getMesh(){
	return mesh;
}

/* Figures which one of the cubefaces 3D-location lands, and what 2D-coordinates
 * that face has.
 * Returns:
 *	On success, pointer to a pointer of HeightMap location lands,
 *	HeightMap-coordinates x and y, and function return value true.
 *	On failure, return function value is false. */
bool PSphere::getGridLocation(Ogre::Vector3 location, Grid **face,
							  unsigned int &ix, unsigned int &iy)
{
	Grid *grid;
	Ogre::Real x, y, z, x_f, y_f;

edgeCase:
	x = Ogre::Math::Abs(location.x);
	y = Ogre::Math::Abs(location.y);
	z = Ogre::Math::Abs(location.z);

	if (x > y && x > z)
	{
		// Scale longest component to unit length
		location *= (1.0f/x);
		// Set grid y-component
		y_f = location.z;
		// Check if this is positive or negative face
		if (location.x < 0.0f)
		{
			// grid x-component
			x_f = -location.y;
			grid = gridXM;
		}
		else
		{
			x_f = location.y;
			grid = gridXP;
		}
	}
	else if (y > x && y > z)
	{
		location *= (1.0f/y);
		y_f = location.z;
		if (location.y < 0.0f)
		{
			x_f = location.x;
			grid = gridYM;
		}
		else
		{
			x_f = -location.x;
			grid = gridYP;
		}
	}
	else if (z > x && z > y)
	{
		location *= (1.0f/z);
		x_f = -location.x;
		if (location.z < 0.0f)
		{
			y_f = location.y;
			grid = gridZM;
		}
		else
		{
			y_f = -location.y;
			grid = gridZP;
		}
	}
	else
	{
		// If vector length is zero, location can't reside on a cube.
		if (location.length() == 0)
			return false;

		/* If two or three vector elements equal to each other, they are on
		 * cube edges. This slightly shortens y compared to x and z compared to y.
		 *  This results in ix and iy to fall within a correct range. */
		if (x == y)
		{
			location.y *= 0.9999;

		}
		if (x == z)
		{
			location.z *= 0.9999;
		}
		if (y == z)
		{
			location.z *= 0.9999;
		}
		// Use goto to try again
		goto edgeCase;
	}

	iy = (unsigned short)((1.0f+y_f)/2.0f*grid->getSize());
	ix = (unsigned short)((1.0f+x_f)/2.0f*grid->getSize());

	(*face) = grid;

	return true;
}

/* Checks if position is water or solid ground.
 * Returns:
 *  On ground, return true.
 *  On water or has an object, return false. */
bool PSphere::checkAccessibility(Ogre::Vector3 location)
{
	Grid *grid, *gridObj;
	unsigned int i, ix, iy, Obj_x, Obj_y;
	Ogre::Vector3 ObjPos;

	getGridLocation(location, &grid, ix, iy);

	// Check if location to check has already an object
	for(i=0; i < objects.size(); i++)
	{
		ObjPos = objects[i].getPosition();
		getGridLocation(ObjPos, &gridObj, Obj_x, Obj_y);

		// Checks if location and object is on a same grid
		if (grid == gridObj)
		{
			if ( (ix == Obj_x) && (iy == Obj_y) )
				return false;
		}
	}

	// If location height is more than sea-height, it is accessible.
	if (grid->getValue(ix, iy) != 0.0)
		return true;
	else
		return false;
}

Ogre::Vector3 PSphere::nextPosition(Ogre::Vector3 location, PSphere::Direction dir)
{
	Ogre::Vector3 newPos;
	Grid *grid;
	unsigned int int_x, int_y;

	/* Using 3D cartesian position figures out which face of the 6 cubefaces it
	 * resides, and gives integer grid-coordinates x and y for it. */
	getGridLocation(location, &grid, int_x, int_y);

	// Going y+
	if (dir == PSPHERE_GRID_YPLUS)
	{
		// Handles migrating from one grid to the next
		if (int_y == grid->getSize()-1)
		{
			// Outputs adjacent x and y on neighboring grid by using current grid x and y
			grid->getNeighbourEntryCoordinates(Grid::neighbour_YP, int_x, int_y);
			// Set neighbour as a grid
			grid = grid->getNeighbourPtr(Grid::neighbour_YP);
		}
		else
			int_y++;
	}
	else if (dir == PSPHERE_GRID_YMINUS)
	{
		if (int_y == 0)
		{
			grid->getNeighbourEntryCoordinates(Grid::neighbour_YM, int_x, int_y);
			grid = grid->getNeighbourPtr(Grid::neighbour_YM);
		}
		else
			int_y--;
	}
	else if (dir == PSPHERE_GRID_XPLUS)
	{
		if (int_x == grid->getSize()-1)
		{
			grid->getNeighbourEntryCoordinates(Grid::neighbour_XP, int_x, int_y);
			grid = grid->getNeighbourPtr(Grid::neighbour_XP);
		}
		else
			int_x++;
	}
	else if (dir == PSPHERE_GRID_XMINUS)
	{
		if (int_x == 0)
		{
			grid->getNeighbourEntryCoordinates(Grid::neighbour_XM, int_x, int_y);
			grid = grid->getNeighbourPtr(Grid::neighbour_XM);
		}
		else
			int_x--;
	}

	// Project 2D grid-location back to 3D cartesian coordinate
	newPos = grid->projectToSphere(int_x, int_y);

	return newPos;
}


vector<ObjectInfo> *PSphere::getObjects()
{
	return &objects;
}

void PSphere::setCollisionManager(CollisionManager	*CDM)
{
	CollisionDetectionManager = CDM;
}

unsigned char *PSphere::exportEquirectangularMap(unsigned short width, unsigned short height) {

	// Check if exportImage is already allocated
	if (exportImage != NULL)
		delete[] exportImage;
	exportImage = new unsigned char[width*height*3];

	// Generates the map. exportImage points to it.
	generateImage(width, height, exportImage);

	return exportImage;
}

void PSphere::exportEquirectangularMap(unsigned short width, unsigned short height, std::string fileName) {
	RGBQUAD color;
//	unsigned char *exportImage;

	/* Create map to memory location pointed by exportImage.
	 * It is class private variable. */
	exportEquirectangularMap(width, height);

	// Use freeimage to save the map as a file
	FreeImage_Initialise();
	FIBITMAP *bitmap = FreeImage_Allocate(width, height, 24);
	for(int i=0; i < width; i++) {
		for (int j=0; j < height; j++) {
			color.rgbRed = exportImage[((width*j)+i)*3];
			color.rgbGreen = exportImage[((width*j)+i)*3+1];
			color.rgbBlue = exportImage[((width*j)+i)*3+2];
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}
	FreeImage_Save(FIF_PNG, bitmap, fileName.c_str(), 0);

	FreeImage_DeInitialise();
}

void PSphere::moveObject(const std::string &objectName, int direction, float pace) {
	for (vector<ObjectInfo>::iterator it = objects.begin() ; it != objects.end(); ++it) {
		ObjectInfo objTemp = *it;
		if (objTemp.getObjectName().compare(objectName) == 0) {
			Ogre::Node *node = objTemp.getNode();
			Ogre::Vector3 oldPosition = node->getPosition();
			Ogre::Vector3 newPosition(oldPosition.x, oldPosition.y, oldPosition.z);

			Ogre::Vector2 cartesianCoord;
			Ogre::Vector3 cart_coord;
			switch (direction) {
				case (UP):
					oldPosition.normalise();
					cartesianCoord=Ogre::Vector2(asin(oldPosition.z ), atan2(oldPosition.y, oldPosition.x));
					cartesianCoord = Ogre::Vector2(cartesianCoord.x*(180/Ogre::Math::PI)-pace, 360+cartesianCoord.y*(180/Ogre::Math::PI)); // Convertion from radians to degrees
					
					if(cartesianCoord.x < -90.0f) //prevent shaking
						break;
					cart_coord = convertSphericalToCartesian(cartesianCoord.x, cartesianCoord.y);
					
					//set on the ground
					newPosition = cart_coord * ( getSurfaceHeight(cart_coord) / cart_coord.length()) ;
					
					node->setPosition(newPosition);
					objTemp.setPosition(newPosition);

					//Collision Detection
					if(CollisionDetectionManager->checkCollisionAABB(objTemp).collided)//collided,move back
					{	
						node->setPosition(oldPosition);
						objTemp.setPosition(oldPosition);
					}else{//not collided, change orientataion and position
						
						//change orientation
						Ogre::Quaternion q;
						q = Ogre::Vector3::UNIT_Y.getRotationTo(newPosition);
						Ogre::Quaternion a;
						node->setOrientation( q );
						node->yaw ( ( (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw() ) );
						
					}
					break;
				case (DOWN):
					oldPosition.normalise();
					cartesianCoord=Ogre::Vector2(asin(oldPosition.z ), atan2(oldPosition.y, oldPosition.x));
					cartesianCoord = Ogre::Vector2(cartesianCoord.x*(180/Ogre::Math::PI)+pace, 360+cartesianCoord.y*(180/Ogre::Math::PI)); // Convertion from radians to degrees
					if(cartesianCoord.x > 90.0f)//prevent shaking
						break;
					cart_coord = convertSphericalToCartesian(cartesianCoord.x, cartesianCoord.y);

					//set on the ground
					newPosition = cart_coord * ( getSurfaceHeight(cart_coord) / cart_coord.length()) ;

					node->setPosition(newPosition);
					objTemp.setPosition(newPosition);

					//Collision Detection
					if(CollisionDetectionManager->checkCollisionAABB(objTemp).collided)//collided,move back
					{	
						node->setPosition(oldPosition);
						objTemp.setPosition(oldPosition);
					}else{//not collided, change orientataion and position
						
						//change orientation
						Ogre::Quaternion q;
						q = Ogre::Vector3::UNIT_Y.getRotationTo(newPosition);
						Ogre::Quaternion a;
						node->setOrientation( q );
						//node->yaw ( Ogre::Math::Abs( (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw() )*-1 );
						//qDebug() << (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw().valueDegrees();
						node->yaw ( ( (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw() ) );
						
					}
					break;
				case (LEFT):

					cartesianCoord = Ogre::Vector2(asin(oldPosition.z ), atan2(oldPosition.y, oldPosition.x));
		/*			if(cartesianCoord.isNaN())
					{
						break;
					}*/
					cartesianCoord = Ogre::Vector2(cartesianCoord.x*(180/Ogre::Math::PI), 360+cartesianCoord.y*(180/Ogre::Math::PI)-pace); // Convertion from radians to degrees
			/*		if(cartesianCoord.isNaN())
					{
						break;
					}*/
					cart_coord = convertSphericalToCartesian(cartesianCoord.x, cartesianCoord.y);

					//set on the ground
					newPosition = cart_coord * ( getSurfaceHeight(cart_coord) / cart_coord.length()) ;


					node->setPosition(newPosition);
					objTemp.setPosition(newPosition);

					//Collision Detection
					if(CollisionDetectionManager->checkCollisionAABB(objTemp).collided)//collided,move back
					{	
						node->setPosition(oldPosition);
						objTemp.setPosition(oldPosition);
					}else{//not collided, change orientataion and position	
						//change orientation
						Ogre::Quaternion q;
						q = Ogre::Vector3::UNIT_Y.getRotationTo(newPosition);
						Ogre::Quaternion a;
						node->setOrientation( q );
						node->yaw ( Ogre::Math::Abs( (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw() ) );
					}
					
					break;
				case (RIGHT):
					cartesianCoord = Ogre::Vector2(asin(oldPosition.z ), atan2(oldPosition.y, oldPosition.x));
					cartesianCoord = Ogre::Vector2(cartesianCoord.x*(180/Ogre::Math::PI), 360+cartesianCoord.y*(180/Ogre::Math::PI)+pace); // Convertion from radians to degrees
					cart_coord = convertSphericalToCartesian(cartesianCoord.x, cartesianCoord.y);
					
					//set on the ground
					newPosition = cart_coord * ( getSurfaceHeight(cart_coord) / cart_coord.length()) ;

					node->setPosition(newPosition);
					objTemp.setPosition(newPosition);

					//Collision Detection
					if(CollisionDetectionManager->checkCollisionAABB(objTemp).collided)//collided,move back
					{	
						node->setPosition(oldPosition);
						objTemp.setPosition(oldPosition);
					}else{//not collided, change orientataion and position	
						//change orientation
						Ogre::Quaternion q;
						q = Ogre::Vector3::UNIT_Y.getRotationTo(newPosition);
						Ogre::Quaternion a;
						node->setOrientation( q );
						node->yaw ( Ogre::Math::Abs( (newPosition-oldPosition).getRotationTo(q*Ogre::Vector3::UNIT_Z).getYaw() ) );
					}
					
					break;
			}
		}
	}
}
