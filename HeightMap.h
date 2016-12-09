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

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreMatrix3.h>
#include "Grid.h"
#include "ResourceParameter.h"

class HeightMap: public Grid
{
public:
    HeightMap(unsigned int size,
              const Ogre::Matrix3 face,
              Ogre::Vector2 UpperLeft,
              Ogre::Vector2 LowerRight,
              ResourceParameter *param,
              Ogre::Real Height_sea);
	~HeightMap();
	void setHeight(unsigned int x, unsigned int y, float elevation);
	Ogre::Vector3 projectToSphere(unsigned int x, unsigned int y);

    /* Fills hardware-buffers with vertice- and texture-data. Creates entity
     * called Name, mesh called Name+"_mesh", material called Name+"_material",
     * textureUnitState called Name+"_texture".
     * Attachs entity to a given node. */
    void load(Ogre::SceneNode *node, Ogre::SceneManager *scene,
              const std::string &Name, float scalingFactor);

    /* Detach and destroy entity */
    void unload(Ogre::SceneNode *node, Ogre::SceneManager *scene);

    /* Creates 4 children, unless at least one child pointer is already
     * non-null, which results in not creating any children. */
    bool createChildren();

    /* Deletes 4 children, unless at least one child pointer is already null,
     * which results in no children to be deleted. */
    bool deleteChildren();

    /* Returns pointers to children */
    void getChildren(HeightMap *&upperLeft, HeightMap *&upperRight,
                     HeightMap *&lowerLeft, HeightMap *&lowerRight);

    HeightMap *getChild(unsigned char child);

    Ogre::Vector3 getCenterPosition();

    bool isLoaded();
private:
    float           **height;
    float           minHeight;
    float           maxHeight;
    float           seaHeight;
    Ogre::uint16    textureResolution;
    Ogre::uint8     *squareTexture;
    Ogre::Vector3   randomTranslate;

    HeightMap       *child[4];

	Ogre::Vector3	*vertexes;
	Ogre::Vector3	*verNorms;
	Ogre::Vector2	*txCoords;
	Ogre::uint32	*indexes;

    Ogre::Entity    *entity;
    ResourceParameter *RParam;

	void calculateNormals();

    /* Creates vertex-data and indexes */
    void generateMeshData(float scalingFactor);

    /* Creates geometry for heightmap */
    void createGeometry();

    /* Creates square bitmap to be used as a texture */
    void createTexture();

    /* Calculate AABox for HeightMap mesh */
    Ogre::AxisAlignedBox tileAABox(void);

    /* Creates and fills hardware-buffer with vertex-data */
    void bufferMesh(const std::string &meshName, float scalingFactor);

    /* Creates and fills hardware-buffer with texture-data */
    void bufferTexture(const std::string &textureName);
};

#endif // HEIGHTMAP_H
