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

/*
-----------------------------------------------------------------------------
Filename:    GeneratorFrameListener.h
Description: Defines an Generator frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Esc:		Quit.
Mouse:		Freelook
W or Up:	Forward
S or Down:	Backward
A:			Step left
D:			Step right
R:			Render mode
X:			Look at the planet
F:			Toggle frame rate stats on/off
U:			Choose previous object
O:			Choose next object
I:			Move selected object up
J:			Move selected object down
K:			Move selected object to left
L:			Move selected object to right
******************************************************************************
			Not used yet
			 PgUp:     Move upwards
			 PgDown:   Move downwards
			 
			
			 T:        Cycle texture filtering
					   Bilinear, Trilinear, Anisotropic(8)
			 P:        Toggle on/off display of camera position / orientation
			 //if INCLUDE_RTSHADER_SYSTEM
			 F2:	   Set the main viewport material scheme to default material manager scheme.
			 F3:	   Set the main viewport material scheme to shader generator default scheme.
			 F4:	   Toggle default shader generator lighting model from per vertex to per pixel.
-----------------------------------------------------------------------------
*/

#ifndef __GeneratorFrameListener_H__
#define __GeneratorFrameListener_H__


#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreOverlaySystem.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreFontManager.h"
#include "PSphere.h"
#include "CollisionManager.h"



//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>


#ifdef INCLUDE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#endif
/*
------------------------------------------------------------------------------------------
The Lifcycle of the frame listener:
After call Root->startRendering() function in initOgre's setSceneAndRun() function,
The Root begin to call renderOneFrame() function over and over again.
In renderOneFrame(void):
bool Root::renderOneFrame(void)
{
    if(!_fireFrameStarted()) //call the FrameStarted() function from all listeners
        return false;

    if (!_updateAllRenderTargets())
        return false;

    return _fireFrameEnded();//call the FrameEnded() function from all listeners
}

In _updateAllRenderTargets() function:
bool Root::_updateAllRenderTargets(void)
{
    // update all targets but don't swap buffers
    mActiveRenderer->_updateAllRenderTargets(false);

	// give client app opportunity to use queued GPU time
    bool ret = _fireFrameRenderingQueued();	//call the frameRenderingQueued() function from all listeners

    // block for final swap
    mActiveRenderer->_swapAllRenderTargetBuffers(mActiveRenderer->getWaitForVerticalBlank());

	return ret;
}

In a nutshell, The three principle function work like that:
					  frameStarted()
						    |
				updateAllRenderTargets()
					|				|
			render(Using GPU)	frameRenderingQueued(Using CPU)
						|		|
					   frameEnded()

Although frameStarted() and frameRenderingQueued() start before frameEnded,
render by GPU need some time, at the mean while, frameRenderingQueued() can work,
so it is better to put pre-processing in this function rather than frameStarted()
------------------------------------------------------------------------------------------
*/
class GeneratorFrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener
{
protected:
	virtual void updateStats(void); //overlay update
public:
	GeneratorFrameListener();

	// Constructor takes a RenderWindow because it uses that to determine input context
	GeneratorFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, PSphere *ps, Ogre::SceneManager *Sc,Ogre::SceneNode    *RSN,CollisionManager	*CDM ,bool bufferedKeys = false, bool bufferedMouse = false,
				 bool bufferedJoy = false );

#ifdef INCLUDE_RTSHADER_SYSTEM
	virtual void processShaderGeneratorInput();
#endif

	//Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw);

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	virtual ~GeneratorFrameListener();

	virtual bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);

	virtual bool processUnbufferedMouseInput(const Ogre::FrameEvent& evt);

	virtual void moveCamera();

	virtual void showDebugOverlay(bool show);
	
	// Start before each frame war generated	
	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	// Start after frameStarted() and before frameEnded(),Override frameRenderingQueued event to process that (don't care about frameEnded)
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// Recall after each frame was ended
	bool frameEnded(const Ogre::FrameEvent& evt);

protected:

	Ogre::SceneManager	*Scene;
	Ogre::SceneNode     *RootSceneNode;//use to find the entity
	PSphere				*pSphere;
	CollisionManager	*CollisionDetectionManager;
	Ogre::Camera		*mCamera;
	Ogre::Vector3		mTranslateVector;
	Ogre::Real			mCurrentSpeed;
	Ogre::RenderWindow	*mWindow;
	bool				mStatsOn;
	Ogre::String		mDebugText;
	unsigned int		mNumScreenShots;
	float				mMoveScale;
	float				mSpeedLimit;
	Ogre::Degree		mRotScale;
	// just to stop toggles flipping too fast
	Ogre::Real			mTimeUntilNextToggle ;
	Ogre::Radian		mRotX, mRotY;
	Ogre::TextureFilterOptions mFiltering;
	int					mAniso;

	int					mSceneDetailIndex ;
	Ogre::Real			mMoveSpeed;
	Ogre::Degree		mRotateSpeed;
	Ogre::Overlay*		mDebugOverlay;
	Ogre::Overlay*		mInformationOverlay;//show information

	ObjectInfo*			selectedObject;

	//OIS Input devices
	OIS::InputManager*	mInputManager;
	OIS::Mouse*			mMouse;
	OIS::Keyboard*		mKeyboard;
	OIS::JoyStick*		mJoy;
	
};

#endif
