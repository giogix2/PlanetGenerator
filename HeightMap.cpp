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

#include "HeightMap.h"
#include "Common.h"

HeightMap::HeightMap(unsigned int size,
                     const Ogre::Matrix3 face,
                     Ogre::Vector2 UpperLeft,
                     Ogre::Vector2 LowerRight,
                     std::ResourceParameter *param,
                     Ogre::Real Height_sea)
    : Grid(size, face, UpperLeft, LowerRight)
{
    Ogre::Real maxAmplitude=0;

    child[0] = NULL;
    child[1] = NULL;
    child[2] = NULL;
    child[3] = NULL;

    RParam = param;
    seaHeight = Height_sea;
    textureResolution = 256;

	height = allocate2DArray<float>(size, size);
	memset(height[0], 0, sizeof(float)*size*size);

    /* Calculate minimum and maximum possible height assuming noise
     * range between -1 - +1 */
    for(unsigned int i=0; i < RParam->getAmplitude().size(); i++)
        maxAmplitude += Ogre::Math::Abs(RParam->getAmplitude()[i]);

    /* Simplexnoise represents white noise poorly, so actual min and max can be
     * quite a bit less. */
    minHeight = -maxAmplitude;
    maxHeight = +maxAmplitude;

	vertexes = new Ogre::Vector3[gridSize*gridSize];
	verNorms = new Ogre::Vector3[gridSize*gridSize];
	txCoords = new Ogre::Vector2[gridSize*gridSize];
	indexes = new Ogre::uint32[(gridSize-1)*(gridSize-1)*6];

    /* Re-create randomTranslate using seed. Avoids passing it as a costructor
     * parameter. */
    srand(RParam->getSeed());
    randomTranslate.x = (float)((rand() % 1000)-500)/100.0f;
    randomTranslate.y = (float)((rand() % 1000)-500)/100.0f;
    randomTranslate.z = (float)((rand() % 1000)-500)/100.0f;

    createGeometry();
    createTexture();
}

HeightMap::~HeightMap()
{
	free2DArray(height);

	delete[] vertexes;
	delete[] verNorms;
	delete[] txCoords;
	delete[] indexes;
    delete[] squareTexture;
}

void HeightMap::setHeight(unsigned int x, unsigned int y, float elevation)
{
	height[y][x] = elevation;
}

/* Return vector for a normalized (radius 1.0) sphere */
Ogre::Vector3 HeightMap::projectToSphere(unsigned int x, unsigned int y)
{
	Ogre::Vector3 pos;

    // project heightMap to unit sphere by calling parent-class function
    pos = Grid::projectToSphere(x, y);
	/* add height */
	pos = pos + pos*height[y][x];
	return pos;
}

// scalingFactor scales size of the mesh
void HeightMap::generateMeshData(float scalingFactor)
{
	unsigned int x, y, idx;

	idx = 0;
	for(x=0; x < gridSize; x++)
	{
		for(y=0; y < gridSize; y++)
		{
			// Project height-map location to a sphere
			vertexes[idx] = projectToSphere(x, y) * scalingFactor;

            // Flatten vertices that are under sea-level
            if (vertexes[idx].length() < (1.0f+seaHeight)*scalingFactor)
            {
                vertexes[idx].normalise();
                vertexes[idx] = (vertexes[idx] + seaHeight)*scalingFactor;
            }

			// Calculate texture-coordinate for the vertex
            txCoords[idx].x = static_cast<float>(x)/static_cast<float>(gridSize-1);
            txCoords[idx].y = static_cast<float>(y)/static_cast<float>(gridSize-1);
			idx++;
		}
	}

	// Create indexes to build triangles.
	idx = 0;
	for(x=0; x < gridSize-1; x++)
	{
		for(y=0; y < gridSize-1; y++)
		{
			// Triangle 1
			indexes[idx] = x*gridSize+y+gridSize+1;
			idx++;
			indexes[idx] = x*gridSize+y;
			idx++;
			indexes[idx] = x*gridSize+y+gridSize;
			idx++;

			// Triangle 2, in other words, a quad.
			indexes[idx] = x*gridSize+y;
			idx++;
			indexes[idx] = x*gridSize+y+gridSize+1;
			idx++;
			indexes[idx] = x*gridSize+y+1;
			idx++;
		}
	}

	// Generate normals
	calculateNormals();
}

void HeightMap::calculateNormals()
{
	Ogre::uint32 i, indexCount;
	Ogre::Vector3 p1, p2, normal;

	// Blended normals for a vertex
	// first, zero our normals, so that we can use it as a accumulator
	for(i=0; i < gridSize*gridSize; i++)
	{
		verNorms[i].x = 0.0f;
		verNorms[i].y = 0.0f;
		verNorms[i].z = 0.0f;
	}
	/* calculate normals for every triangle. Multiple normals are added together
	 * for each vertex, so result is average of all the normals. */
	indexCount = (gridSize-1)*(gridSize-1)*6;
	for(i=0; i < indexCount; i = i + 3)
	{
		p1 = vertexes[indexes[i]] - vertexes[indexes[i+1]];
		p2 = vertexes[indexes[i+1]] - vertexes[indexes[i+2]];
		normal = p1.crossProduct(p2);

		verNorms[indexes[i]] += normal;
		verNorms[indexes[i+1]] += normal;
		verNorms[indexes[i+2]] += normal;
	}
	// Normalization pass
	for(i=0; i < gridSize*gridSize; i++)
	{
		verNorms[i].normalise();
	}
}

void HeightMap::createGeometry()
{
    Ogre::uint16 x, y, gSize;
    Ogre::Real elev;
    Ogre::Vector3 spherePos;

    gSize = this->getSize();
    for(y=0; y < gSize; y++)
    {
        for(x=0; x < gSize; x++)
        {
            spherePos = this->projectToSphere(x, y);
            elev = heightNoise(RParam->getAmplitude(), RParam->getFrequency(),
                               spherePos+this->randomTranslate);
            this->setHeight(x, y, elev);
        }
    }
}

void HeightMap::createTexture()
{
    Grid *tGrid;
    Ogre::uint16 gSize, x, y;
    unsigned char red, green, blue;
    Ogre::Real elev;
    Ogre::ColourValue Output;

    gSize = this->textureResolution;
    squareTexture = new Ogre::uint8[gSize*gSize*3];

    Ogre::Vector2 upperL, lowerR, edges, sub;

    // Move sample point half a pixel away from edges
    edges = Ogre::Vector2(0.5f/(gSize-1));
    // Calculate tile range
    sub = this->LowerRight - this->UpperLeft;
    // Scale edges
    edges = sub*edges;
    // Add edges to limit the tile by half a pixel
    upperL = this->UpperLeft + edges;
    lowerR = this->LowerRight - edges;
    tGrid = new Grid(gSize, this->getOrientation(), upperL, lowerR);

    Ogre::ColourValue water1st, water2nd;

    RParam->getWaterFirstColor(red, green, blue);
    water1st.r = red;
    water1st.g = green;
    water1st.b = blue;
    RParam->getWaterSecondColor(red, green, blue);
    water2nd.r = red;
    water2nd.g = green;
    water2nd.b = blue;

    Ogre::ColourValue terrain1st, terrain2nd;

    RParam->getTerrainFirstColor(red, green, blue);
    terrain1st.r = red;
    terrain1st.g = green;
    terrain1st.b = blue;
    RParam->getTerrainSecondColor(red, green, blue);
    terrain2nd.r = red;
    terrain2nd.g = green;
    terrain2nd.b = blue;

    Ogre::ColourValue mountain1st, mountain2nd;

    RParam->getMountainFirstColor(red, green, blue);
    mountain1st.r = red;
    mountain1st.g = green;
    mountain1st.b = blue;
    RParam->getMountainSecondColor(red, green, blue);
    mountain2nd.r = red;
    mountain2nd.g = green;
    mountain2nd.b = blue;

    for(y=0; y < gSize; y++)
    {
        for(x=0; x < gSize; x++)
        {
            elev = heightNoise(RParam->getAmplitude(), RParam->getFrequency(),
                               tGrid->projectToSphere(x,y)+randomTranslate);
            Output = generatePixel(elev,
                                   seaHeight,
                                   minHeight,
                                   maxHeight,
                                   water1st,
                                   water2nd,
                                   terrain1st,
                                   terrain2nd,
                                   mountain1st,
                                   mountain2nd);

            squareTexture[(y*gSize+x)*3] = Output.r;
            squareTexture[(y*gSize+x)*3+1] = Output.g;
            squareTexture[(y*gSize+x)*3+2] = Output.b;
        }
    }

    delete tGrid;
}

void HeightMap::load(Ogre::SceneNode *node, Ogre::SceneManager *scene,
                     const std::string &Name, float scalingFactor)
{
    const std::string meshName = Name + "_mesh";
    const std::string textureName = Name + "_texture";
    const std::string matName = Name + "_material";
    std::string defGrpName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    generateMeshData(scalingFactor);
    bufferMesh(meshName, scalingFactor);
    bufferTexture(textureName);

    this->entity = scene->createEntity(Name, meshName);
    node->attachObject(this->entity);

    Ogre::MaterialPtr texMap;

    /* FIXME: Should texMap and subMesh have a different (material) name?
     * Same name works, but different name works as well. */
    texMap = Ogre::MaterialManager::getSingleton().create(matName ,defGrpName);

    texMap->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
    texMap->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

    this->entity->getMesh()->getSubMesh(0)->setMaterialName(matName);

    // Set texture for the entity
    this->entity->setMaterial(texMap);
}

void HeightMap::unload(Ogre::SceneNode *node, Ogre::SceneManager *scene)
{
    node->detachObject(this->entity->getName());
    scene->destroyEntity(this->entity->getName());
}

void HeightMap::bufferMesh(const std::string &meshName, float scalingFactor)
{
    Ogre::uint32 i, gSize = this->gridSize;
    Ogre::MeshPtr mesh;
    Ogre::SubMesh *subMesh;
    std::string defGrpName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    // Create mesh and subMesh
    mesh = Ogre::MeshManager::getSingleton().createManual(meshName, defGrpName);
    subMesh = mesh->createSubMesh();

    mesh->sharedVertexData = new Ogre::VertexData;
    mesh->sharedVertexData->vertexCount = gSize*gSize;

    // Pointer to declaration of vertexData
    Ogre::VertexDeclaration* vertexDecl = mesh->sharedVertexData->vertexDeclaration;

    // define elements position, normal and tex coordinate
    vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    vertexDecl->addElement(0, 4*3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    vertexDecl->addElement(0, 4*6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

    // Vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vBuf;

    vBuf = Ogre::HardwareBufferManager::getSingleton()
           .createVertexBuffer(8*sizeof(float), gSize*gSize,
                               Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

    mesh->sharedVertexData->vertexBufferBinding->setBinding(0, vBuf);

    // Lock the buffer and write vertex data to it
    float *pVertex;
    pVertex = static_cast<float *>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    for(i=0; i < gSize*gSize; i++)
    {
        pVertex[i*8+0] = vertexes[i].x;
        pVertex[i*8+1] = vertexes[i].y;
        pVertex[i*8+2] = vertexes[i].z;

        pVertex[i*8+3] = verNorms[i].x;
        pVertex[i*8+4] = verNorms[i].y;
        pVertex[i*8+5] = verNorms[i].z;

        pVertex[i*8+6] = txCoords[i].x;
        pVertex[i*8+7] = txCoords[i].y;
    }
    vBuf->unlock();

    // Index buffer
    Ogre::HardwareIndexBufferSharedPtr iBuf;
    Ogre::HardwareIndexBuffer::IndexType itype = Ogre::HardwareIndexBuffer::IT_32BIT;

    iBuf = Ogre::HardwareBufferManager::getSingleton()
           .createIndexBuffer(itype, (gSize-1)*(gSize-1)*6,
           Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

    // Lock index buffer
    unsigned int *pIdx;
    pIdx = static_cast<unsigned int *>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    for(i=0; i < (gSize-1)*(gSize-1)*6; i++)
    {
        pIdx[i] = indexes[i];
    }
    iBuf->unlock();

    subMesh->useSharedVertices = true;
    subMesh->indexData->indexBuffer = iBuf;
    subMesh->indexData->indexCount = (gSize-1)*(gSize-1)*6;
    subMesh->indexData->indexStart = 0;

    /* FIXME: Review boxsize, mesh fits to a lot smaller box */
    mesh->_setBounds(Ogre::AxisAlignedBox(-scalingFactor, -scalingFactor,
                                          -scalingFactor,  scalingFactor,
                                           scalingFactor,  scalingFactor));
    mesh->_setBoundingSphereRadius(scalingFactor);

    mesh->load();
}

void HeightMap::bufferTexture(const std::string &textureName)
{
    Ogre::uint32 y, x;
    Ogre::TexturePtr texture;
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer;
    std::string defGrpName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
    Ogre::uint16 tRes = this->textureResolution;

    texture = Ogre::TextureManager::getSingleton()
              .createManual(textureName, defGrpName, Ogre::TEX_TYPE_2D,
                            tRes, tRes, 0, Ogre::PF_R8G8B8, Ogre:: TU_DYNAMIC);

    pixelBuffer = texture->getBuffer();
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

    const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
    Ogre::uint8 *Texture = static_cast<Ogre::uint8*>(pixelBox.data);

    for(y=0; y < tRes; y++)
    {
        for(x=0; x < tRes; x++)
        {
            /* FIXME: Might be unnecessary memory copy, but was convenient. */
            /* TextureManager did not honor Ogre::PF_R8G8B8, so need to swap
             * red and blue, plus hardware wants alfa channel values too */
            Texture[(y*tRes+x)*4]   = squareTexture[(y*tRes+x)*3+2];   // blue
            Texture[(y*tRes+x)*4+1] = squareTexture[(y*tRes+x)*3+1];   // green
            Texture[(y*tRes+x)*4+2] = squareTexture[(y*tRes+x)*3];     // red
            Texture[(y*tRes+x)*4+3] = 255;                             // Alfa
        }
    }

    pixelBuffer->unlock();
}

bool HeightMap::createChildren()
{
    bool notNull = false;

    /* Check all 4 are null pointers */
    for(int i=0; i < 4; i++)
    {
        if (child[i] != NULL)
            notNull = true;
    }

    if (notNull)
    {
        std::cerr << "Can't create children heightmaps, at least one non-Null"
                     "pointer!" << std::endl;
        return false;
    }
    else
    {
        Ogre::Vector2 upperL, lowerR;

        upperL = this->UpperLeft;
        lowerR = upperL + (this->LowerRight-this->UpperLeft)/2.0f;
        this->child[0] = new HeightMap(this->gridSize, this->orientation, upperL,
                                       lowerR, this->RParam, this->seaHeight);

        upperL.x += (this->LowerRight.x-this->UpperLeft.x)/2.0f;
        lowerR.x += (this->LowerRight.x-this->UpperLeft.x)/2.0f;
        this->child[1] = new HeightMap(this->gridSize, this->orientation, upperL,
                                       lowerR, this->RParam, this->seaHeight);

        upperL = this->UpperLeft;
        upperL.y += (this->LowerRight.y-this->UpperLeft.y)/2.0f;
        lowerR = upperL + (this->LowerRight-this->UpperLeft)/2.0f;
        this->child[2] = new HeightMap(this->gridSize, this->orientation, upperL,
                                       lowerR, this->RParam, this->seaHeight);

        upperL.x += (this->LowerRight.x-this->UpperLeft.x)/2.0f;
        lowerR.x += (this->LowerRight.x-this->UpperLeft.x)/2.0f;
        this->child[3] = new HeightMap(this->gridSize, this->orientation, upperL,
                                       lowerR, this->RParam, this->seaHeight);
    }

    return true;
}

bool HeightMap::deleteChildren()
{
    bool isNull = false;

    /* Check all 4 are non-Null */
    for(int i=0; i < 4; i++)
    {
        if (child[i] == NULL)
            isNull = true;
    }

    if (isNull)
    {
        std::cerr << "Can't delete children, at least one NULL pointer!"
                  << std::endl;
        return false;
    }
    else
    {
        delete this->child[0];
        delete this->child[1];
        delete this->child[2];
        delete this->child[3];
    }

    return true;
}

void HeightMap::getChildren(HeightMap *&upperLeft, HeightMap *&upperRight,
                            HeightMap *&lowerLeft, HeightMap *&lowerRight)
{
    upperLeft  = this->child[0];
    upperRight = this->child[1];
    lowerLeft  = this->child[2];
    lowerRight = this->child[3];
}
