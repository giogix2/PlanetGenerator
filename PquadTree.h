/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Taneli Mikkonen
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

#ifndef PQUADTREE_H
#define PQUADTREE_H

#include "HeightMap.h"
#include "ResourceParameter.h"

class PquadTree
{
public:
    PquadTree(const std::string name, Ogre::uint16 levelSize,
              Ogre::Matrix3 orientation, Ogre::Real seaHeight,
              ResourceParameter *parameters);
    ~PquadTree();

    /* Unload and delete the whole tree up to this node. Depth-first */
    void merge(HeightMap *node);

    /* Set viewer position and render face accordingly. */
    void update(Ogre::Vector3 viewer);

    /* Set scene and node once to avoid passing them as function parameters. */
    void setScene(Ogre::SceneManager *scene, Ogre::SceneNode *node);
private:
    HeightMap               *root;
    std::string             name;
    Ogre::SceneManager      *scene;
    Ogre::SceneNode         *scNode;
    ResourceParameter       *params;
    Ogre::uint32            runningNumber;
    Ogre::Real              dotCutoff;
    Ogre::Real              cornerScaling;

    /* Recursively subdivide face. Three states: match, subdivide, leaf reached. */
    void recursiveTest(HeightMap *node, Ogre::Vector3 viewer,
                       float distanceTest, Ogre::uint16 level);
};

#endif // PQUADTREE_H
