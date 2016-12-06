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

PquadTree::PquadTree(const std::string name, Ogre::uint16 levelSize,
                     Ogre::Matrix3 orientation, Ogre::Real seaHeight,
                     ResourceParameter *parameters)
{
    Ogre::Vector2 upperLeft, lowerRight;
    
    this->name = name;
    this->params = parameters;
    this->runningNumber = 0;

    upperLeft = Ogre::Vector2(-1.0f, 1.0f);
    lowerRight = Ogre::Vector2(1.0f, -1.0f);

    this->root = new HeightMap(levelSize, orientation, upperLeft, lowerRight,
                               parameters, seaHeight);

    std::vector <HeightMap *> childQueue;
    HeightMap *child[4];
    Ogre::uint8 level = 0;

    /* Create full static quadtree up to level 4 for leaf-nodes. Breadth-first. */
    childQueue.push_back(this->root);
    while(level < 4)
    {
        int i = childQueue.size() - static_cast<int>(Ogre::Math::Pow(4, level));
        int iters = childQueue.size();
        for(; i < iters; i++)
        {
            childQueue[i]->createChildren();
            childQueue[i]->getChildren(child[0], child[1], child[2], child[3]);
            childQueue.push_back(child[0]);
            childQueue.push_back(child[1]);
            childQueue.push_back(child[2]);
            childQueue.push_back(child[3]);

        }
        level++;
    }
}

PquadTree::~PquadTree()
{
    std::vector <HeightMap *> childQueue;
    HeightMap *child[4];

    /* Get all nodes in the tree. Breadth-first. */
    childQueue.push_back(this->root);
    unsigned int i=0, previousSize;
    while (1) {
        previousSize = childQueue.size();
        /* i should be correct from previous iteration */
        for(; i < previousSize; i++)
        {
            childQueue[i]->getChildren(child[0], child[1], child[2], child[3]);
            /* Correct quadtree has either all 4 children or none at all. */
            if (child[0] != NULL)
            {
                childQueue.push_back(child[0]);
                childQueue.push_back(child[1]);
                childQueue.push_back(child[2]);
                childQueue.push_back(child[3]);
            }
        }
        if (previousSize == childQueue.size())
            break;
    }

    std::cout << "PquadTree \"" << this->name << "\": deleting "
              << childQueue.size() << " nodes." << std::endl;

    while (childQueue.size() > 0)
    {
        delete childQueue[childQueue.size()-1];
        childQueue.pop_back();
    }
}

void PquadTree::recursiveTest(HeightMap *node, Ogre::Vector3 viewer,
                              float distanceTest, Ogre::uint16 level)
{
    Ogre::Vector3 distTocenter;
    float distance;
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

    distTocenter = node->getCenterPosition() - viewer;
    distance = distTocenter.length();

    /* If distance is bigger than test, render tile. */
    if (distance > distanceTest )
    {
        if (node->isLoaded() == false)
        {
            node->load(this->scNode, this->scene, hName, params->getRadius());

            /* FIXME: Assumes checking one level up is enough, but might not be
             * in every case, e.g. camera backing off quicly. */
            if (node->getChild(0) != NULL)
            {
                for(int i=0; i < 4; i++)
                {
                    /* Unload child-tiles, if previously loaded. */
                    if (node->getChild(i)->isLoaded() == true)
                        node->getChild(i)->unload(scNode, scene);
                }
            }
        }
    }
    /* Sub-divide. */
    else if (node->getChild(0) != NULL)
    {
        if (node->isLoaded() == true)
            node->unload(scNode, scene);

        distanceTest /= 2.0f;

        level++;
        for(int i=0; i < 4; i++)
        {
            recursiveTest(node->getChild(i), viewer, distanceTest, level);

        }
    }
    /* Leaf reached. */
    else
    {
        if (node->isLoaded() == false)
            node->load(this->scNode, this->scene, hName, params->getRadius());
    }
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
