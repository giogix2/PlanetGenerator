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

#include <sstream>
#include "PquadTree.h"

#define MAX_LEVEL 6

PquadTree::PquadTree(const std::string name, Ogre::uint16 levelSize,
                     Ogre::Matrix3 orientation, Ogre::Real seaHeight,
                     ResourceParameter *parameters)
{
    Ogre::Vector2 upperLeft, lowerRight;
    Ogre::Real angle, diff;
    
    this->name = name;
    this->params = parameters;
    this->runningNumber = 0;

    upperLeft = Ogre::Vector2(-1.0f, 1.0f);
    lowerRight = Ogre::Vector2(1.0f, -1.0f);

    this->root = new HeightMap(levelSize, orientation, upperLeft, lowerRight,
                               parameters, seaHeight);

    // Scaling factor for corners
    this->cornerScaling = (this->params->getRadius() - this->root->getAmplitude())
                          / this->params->getRadius();

    /* Calculates comparison value for determining HeightMap visibility.
     * Assumes horizon as a lowest plain which has highest possible feature as
     * background. First calculates angle from horizon to feature which is enough
     * to hide feature behind horizon. Then adds Pi/2 which is horizons
     * (or rather its normals) angle to the viewer. Finally, converts it to be
     * used as a comparator against dot product calculation. */
    diff = (parameters->getRadius() - this->root->getAmplitude())
            /(parameters->getRadius() + this->root->getAmplitude());
    angle = Ogre::Math::ACos(diff).valueRadians();
    this->dotCutoff = Ogre::Math::Cos(angle+Ogre::Math::HALF_PI);
}

PquadTree::~PquadTree()
{
    merge(this->root);
    delete this->root;
}

void PquadTree::merge(HeightMap *node)
{
    if (node->getChild(0) != NULL)
    {
        for(int i=0; i < 4; i++)
        {
            merge(node->getChild(i));

            if (node->getChild(i)->isLoaded() == true)
                node->getChild(i)->unload(this->scNode, this->scene);
        }
        node->deleteChildren();
    }
}

void PquadTree::recursiveTest(HeightMap *node, Ogre::Vector3 viewer,
                              float distanceTest, Ogre::uint16 level)
{
    Ogre::Vector3 dist, corner[4];
    float distance, dProd, smallestAngle;
    std::stringstream levelSS, runningSS;
    std::string hName, ss_str;

    /* Make individual name for every tile. qtree-name + _l<level> + _<running> */
    /* FIXME: Tiny, but non-zero change, that 2 different entitys have same name. */
    levelSS << level;
    hName = this->name + "_l";
    ss_str = levelSS.str();
    runningSS << this->runningNumber;
    ss_str = ss_str + "_" + runningSS.str();
    hName = hName + ss_str;

    this->runningNumber++;

    dist = node->getCenterPosition() - viewer;
    distance = dist.length();

    node->getCornerPosition(corner[0], corner[1], corner[2], corner[3]);

    // Find corner with least tilt away from the viewer
    smallestAngle = -1.0f;
    for(int i=0; i < 4; i++)
    {
        // Scale corner to minimum height
        corner[i] *= this->cornerScaling;

        dist = viewer - corner[i];
        corner[i].normalise();
        dist.normalise();
        dProd = dist.dotProduct(corner[i]);
        if (smallestAngle < dProd)
            smallestAngle = dProd;
    }

    /* If HeightMap is not visible, just merge and return. */
    if (this->dotCutoff < smallestAngle)
    {
        /* If distance is bigger than test, render tile. */
        if (distance > distanceTest)
        {

            if (node->isLoaded() == false)
            {
                node->load(this->scNode, this->scene, hName, params->getRadius());
            }
            /* Delete tree from here on */
            merge(node);
        }
        /* Sub-divide. */
        else if (level < MAX_LEVEL)
        {
            if (node->isLoaded() == true)
                node->unload(scNode, scene);

            distanceTest /= 2.0f;

            level++;

            /* Create children if not done in previous frame */
            if (node->getChild(0) == NULL)
                node->createChildren();

            for(int i=0; i < 4; i++)
            {
                recursiveTest(node->getChild(i), viewer, distanceTest, level);

            }
        }
        /* MAX_LEVEL reached. */
        else
        {
            if (node->isLoaded() == false)
                node->load(this->scNode, this->scene, hName, params->getRadius());
        }
    }
    else
        merge(node);

    return;
}

void PquadTree::update(Ogre::Vector3 viewer)
{
    Ogre::Real subdivideDistance = 40.5f;

    /* Test for a subdivision in the quadtree. Depth-first. */
    recursiveTest(this->root, viewer, subdivideDistance, 0);
}

void PquadTree::setScene(Ogre::SceneManager *scene, Ogre::SceneNode *node)
{
    this->scene = scene;
    this->scNode = node;
}
