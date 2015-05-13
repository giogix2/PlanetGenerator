/*
-----------------------------------------------------------------------------
Filename:    GeneratorFrameListener.h
Description: Defines an Generator frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
			 PgUp:     Move upwards
			 PgDown:   Move downwards
			 F:        Toggle frame rate stats on/off
			 R:        Render mode
			 L:        Look at the planet
			 T:        Cycle texture filtering
					   Bilinear, Trilinear, Anisotropic(8)
			 P:        Toggle on/off display of camera position / orientation
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

class GeneratorFrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener
{
protected:
	virtual void updateStats(void); //overlay update
public:
	Ogre::SceneNode     *RootSceneNode;//use to operate the entity

	//MOC::CollisionTools *CollisionManager;
	Ogre::SceneManager	*Scene;
	PSphere				*pSphere;
	CollisionManager	*CollisionDetectionManager;
	

	GeneratorFrameListener()
	{
	}

	// Constructor takes a RenderWindow because it uses that to determine input context
	GeneratorFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, PSphere *ps, Ogre::SceneManager *Sc,Ogre::SceneNode    *RSN,CollisionManager	*CDM ,bool bufferedKeys = false, bool bufferedMouse = false,
				 bool bufferedJoy = false ) :
		mCamera(cam), mTranslateVector(Ogre::Vector3::ZERO), mCurrentSpeed(0), mWindow(win), mStatsOn(true), mNumScreenShots(0),
		mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(Ogre::TFO_BILINEAR),
		mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(18), mDebugOverlay(0),
		mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
	{

		CollisionDetectionManager = CDM;

		RootSceneNode = RSN;//NEW

		Scene = Sc;

		pSphere = ps;
		
		// init the collision handler
		//CollisionManager = new MOC::CollisionTools(Scene);
		// set how far we want the camera to be above ground
		//CollisionManager->setHeightAdjust(5);
		

		Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
		mDebugOverlay = om.create("Core/DebugOverlay");
		mDebugOverlay->setZOrder(500);
		mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");

		Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = OIS::InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
		try {
			mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
		}
		catch(...) {
			mJoy = 0;
		}

		//Set initial mouse clipping size
		windowResized(mWindow);

		//overlay ststem
		Ogre::OverlayContainer *border = static_cast<Ogre::OverlayContainer *> (om.createOverlayElement("BorderPanel", "Core/StatPanel"));
		border->setMetricsMode(Ogre::GMM_PIXELS);
		border->setVerticalAlignment(Ogre::GVA_BOTTOM);
		border->setPosition(10, 10);
		border->setDimensions(100, 100);
		//border->setMaterialName( "BaseWhite" );
		//border->setMaterialName("Examples/Rockwall");

		// Create a text area
		/*Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(
			om.createOverlayElement("TextArea", "TextAreaName"));
		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(0, 0);
		textArea->setDimensions(100, 100);
		textArea->setCaption("Hello, World!");
		textArea->setCharHeight(16);
		//textArea->setFontName("TrebuchetMSBold");
		textArea->setColourBottom(Ogre::ColourValue(0.3, 0.5, 0.3));
		textArea->setColourTop(Ogre::ColourValue(0.5, 0.7, 0.5));

		// Add the text area to the panel
		border->addChild(textArea);
		mDebugOverlay->add2D(border);
		showDebugOverlay(true);*/

		
		//mWindow->resize(800, 700);

		//Register as a Window listener
		Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);	

	}


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

	Ogre::Quaternion RotationBetweenVectors(Ogre::Vector3 start, Ogre::Vector3 dest);

	virtual void moveCamera();

	virtual void showDebugOverlay(bool show);

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	// Override frameRenderingQueued event to process that (don't care about frameEnded)
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	bool frameEnded(const Ogre::FrameEvent& evt);

protected:
	Ogre::Camera* mCamera;

	Ogre::Vector3 mTranslateVector;
	Ogre::Real mCurrentSpeed;
	Ogre::RenderWindow* mWindow;
	bool mStatsOn;

	Ogre::String mDebugText;

	unsigned int mNumScreenShots;
	float mMoveScale;
	float mSpeedLimit;
	Ogre::Degree mRotScale;
	// just to stop toggles flipping too fast
	Ogre::Real mTimeUntilNextToggle ;
	Ogre::Radian mRotX, mRotY;
	Ogre::TextureFilterOptions mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	Ogre::Real mMoveSpeed;
	Ogre::Degree mRotateSpeed;
	Ogre::Overlay* mDebugOverlay;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	OIS::JoyStick* mJoy;
	
};

#endif
