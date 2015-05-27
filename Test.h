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
#pragma once
#ifndef TEST_H
#define TEST_H

#include <assert.h>
#include "PSphere.h"

/* This is unit test.
 * To run the test, you need to launch the application
 * in consle with a argument "t".*/

void testAll();
bool initTest(PSphere **Sphere,Ogre::Root **Root, Ogre::SceneManager **Scene, Ogre::SceneNode **sphere);
bool test_PSphere_getRadius(PSphere *Sphere);
bool test_PSphere_checkAccessibility(PSphere *Sphere);
bool test_PSphere_getSurfaceHeight(PSphere *Sphere);
bool test_PSphere_getGridLocation(PSphere *Sphere);
bool test_PSphere_nextPosition(PSphere *Sphere);
bool test_PSphere_getObserverDistanceToSurface(PSphere *Sphere);
bool test_PSphere_attachMeshOnGround(PSphere *Sphere,Ogre::SceneNode *sphereNode, Ogre::SceneManager *Scene);



#endif