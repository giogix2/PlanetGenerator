#include "GeneratorFrameListener.h"
#include <iostream>

GeneratorFrameListener::GeneratorFrameListener()
{
}
GeneratorFrameListener::GeneratorFrameListener(	Ogre::RenderWindow* win, Ogre::Camera* cam, PSphere *ps, Ogre::SceneManager *Sc,Ogre::SceneNode    *RSN,
												CollisionManager	*CDM ,bool bufferedKeys, bool bufferedMouse,
												bool bufferedJoy) :
	mCamera(cam), mTranslateVector(Ogre::Vector3::ZERO), mCurrentSpeed(0), mWindow(win), mStatsOn(true), mNumScreenShots(0),
	mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(Ogre::TFO_BILINEAR),
	mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(18), mDebugOverlay(0),
	mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
{

	CollisionDetectionManager = CDM;

	RootSceneNode = RSN;

	Scene = Sc;

	pSphere = ps;

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
	Ogre::OverlayManager &overlayManager = Ogre::OverlayManager::getSingleton();
	mDebugOverlay = overlayManager.getByName("Core/DebugOverlay");
	//mDebugOverlay->setZOrder(500);
	//mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");

	//Ogre::OverlayContainer* border = static_cast<Ogre::OverlayContainer*>  (overlayManager.createOverlayElement( "Panel", "NormalPanel" ) );
	//Ogre::OverlayContainer *border = static_cast<Ogre::OverlayContainer *> (overlayManager.getOverlayElement("Core/StatPanel"));
	//border->setMetricsMode(Ogre::GMM_PIXELS);
	//border->setVerticalAlignment(Ogre::GVA_BOTTOM);
	//border->setPosition(0, 0);
	//border->setDimensions(0.1, 0.1);
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
	border->addChild(textArea);*/
	//mDebugOverlay->add2D(border);
	showDebugOverlay(true);

		
	//mWindow->resize(800, 700);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);	

}

void GeneratorFrameListener::updateStats(void)
{

    static Ogre::String currFps = "Current FPS: ";
    static Ogre::String avgFps = "Average FPS: ";
    static Ogre::String bestFps = "Best FPS: ";
    static Ogre::String worstFps = "Worst FPS: ";
    static Ogre::String tris = "Triangle Count: ";

    // update stats when necessary
    try {
        Ogre::OverlayElement* guiAvg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
        Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
        Ogre::OverlayElement* guiBest = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
        Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

        const Ogre::RenderTarget::FrameStats& stats = mWindow->getStatistics();

        guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
        guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
        guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS)
            +" "+Ogre::StringConverter::toString(stats.bestFrameTime)+" ms");
        guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(stats.worstFPS)
            +" "+Ogre::StringConverter::toString(stats.worstFrameTime)+" ms");

        Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
        guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

    }
    catch(...)
    {
        // ignore
    }

}

#ifdef INCLUDE_RTSHADER_SYSTEM
void GeneratorFrameListener::processShaderGeneratorInput()
{
	// Switch to default scheme.
	if (mKeyboard->isKeyDown(OIS::KC_F2))
	{	
		mCamera->getViewport()->setMaterialScheme(MaterialManager::DEFAULT_SCHEME_NAME);			
		mDebugText = "Active Viewport Scheme: ";
		mDebugText += MaterialManager::DEFAULT_SCHEME_NAME;						
	}

	// Switch to shader generator scheme.
	if (mKeyboard->isKeyDown(OIS::KC_F3))
	{
		mCamera->getViewport()->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		mDebugText = "Active Viewport Scheme: ";
		mDebugText += RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME;
	}	

	// Toggles per pixel per light model.
	if (mKeyboard->isKeyDown(OIS::KC_F4) && mTimeUntilNextToggle <= 0)
	{	
		mTimeUntilNextToggle = 1.0;

		static bool userPerPixelLightModel = true;
		RTShader::ShaderGenerator* shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();			
		RTShader::RenderState* renderState = shaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

		// Remove all global sub render states.
		renderState->reset();

		// Add per pixel lighting sub render state to the global scheme render state.
		// It will override the default FFP lighting sub render state.
		if (userPerPixelLightModel)
		{
			RTShader::SubRenderState* perPixelLightModel = shaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
			renderState->addTemplateSubRenderState(perPixelLightModel);

			mDebugText = "Per pixel lighting model applied to shader generator default scheme";
		}
		else
		{
			mDebugText = "Per vertex lighting model applied to shader generator default scheme";
		}

		// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
		shaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

		userPerPixelLightModel = !userPerPixelLightModel;
	}	
		
}
#endif

void GeneratorFrameListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void GeneratorFrameListener::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );
			mInputManager->destroyInputObject( mJoy );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

bool GeneratorFrameListener::processUnbufferedKeyInput(const Ogre::FrameEvent& evt)
{
	Ogre::Real moveScale = mMoveScale;
	if(mKeyboard->isKeyDown(OIS::KC_X))
		mCamera->lookAt(0.0f,0.0f,0.0f);
	if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
		moveScale *= 10;
	if(mKeyboard->isKeyDown(OIS::KC_RSHIFT))
		moveScale /= 10;

	if(mKeyboard->isKeyDown(OIS::KC_A))
		mTranslateVector.x = -moveScale;	// Move camera left

	if(mKeyboard->isKeyDown(OIS::KC_D))
		mTranslateVector.x = moveScale;	// Move camera RIGHT

	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
	{
		Ogre::Entity *tmpE = NULL;
		Ogre::Vector3 result = Ogre::Vector3::ZERO;
		float distToColl;
		Ogre::Vector2 *Vec=new Ogre::Vector2(mMouse->getMouseState().X.abs,mMouse->getMouseState().Y.abs);

		mTranslateVector.z = -pSphere->getRadius()*0.1;

	}
			

	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
	{
		Ogre::Entity *tmpE = NULL;
		Ogre::Vector3 result = Ogre::Vector3::ZERO;
		float distToColl;
		Ogre::Vector2 *Vec=new Ogre::Vector2(mMouse->getMouseState().X.abs,mMouse->getMouseState().Y.abs);

		mTranslateVector.z = pSphere->getRadius()*0.1;
		
	}
			

	if(mKeyboard->isKeyDown(OIS::KC_PGUP))
		mTranslateVector.y = moveScale;	// Move camera up

	if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
		mTranslateVector.y = -moveScale;	// Move camera down

	if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mCamera->yaw(-mRotScale);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT))
		mCamera->yaw(mRotScale);

	if( mKeyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 )
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 )
	{
			
		switch(mFiltering)
		{
		case Ogre::TFO_BILINEAR:
			mFiltering = Ogre::TFO_TRILINEAR;
			mAniso = 1;
			break;
		case Ogre::TFO_TRILINEAR:
			mFiltering = Ogre::TFO_ANISOTROPIC;
			mAniso = 8;
			break;
		case Ogre::TFO_ANISOTROPIC:
			mFiltering = Ogre::TFO_BILINEAR;
			mAniso = 1;
			break;
		default: break;
		}
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mTimeUntilNextToggle = 0.5;
		mDebugText = "Saved: " + ss.str();
	}

	if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(Ogre::PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(Ogre::PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(Ogre::PM_POINTS); break;
		}
		mTimeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
	{
		displayCameraDetails = !displayCameraDetails;
		mTimeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			mDebugText = "";
	}

	// Print camera details
	if(displayCameraDetails)
		mDebugText = "P: " + Ogre::StringConverter::toString(mCamera->getDerivedPosition()) +
						" " + "O: " + Ogre::StringConverter::toString(mCamera->getDerivedOrientation());

	// Return true to continue rendering
	return true;
}

bool GeneratorFrameListener::processUnbufferedMouseInput(const Ogre::FrameEvent& evt)
{

	// Rotation factors, may not be used if the second mouse button is pressed
	// 2nd mouse button - slide, otherwise rotate
	const OIS::MouseState &ms = mMouse->getMouseState();
	if( ms.buttonDown( OIS::MB_Right ) )
	{
		mTranslateVector.x += ms.X.rel * 0.13;
		mTranslateVector.y -= ms.Y.rel * 0.13;
	}
	else
	{
		mRotX = Ogre::Degree(-ms.X.rel * 0.13);
		mRotY = Ogre::Degree(-ms.Y.rel * 0.13);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
		// Adjust the input depending upon viewport orientation
		Radian origRotY, origRotX;
		switch(mCamera->getViewport()->getOrientation())
		{
			case Viewport::OR_LANDSCAPELEFT:
				origRotY = mRotY;
				origRotX = mRotX;
				mRotX = origRotY;
				mRotY = -origRotX;
				break;
			case Viewport::OR_LANDSCAPERIGHT:
				origRotY = mRotY;
				origRotX = mRotX;
				mRotX = -origRotY;
				mRotY = origRotX;
				break;
					
			// Portrait doesn't need any change
			case Viewport::OR_PORTRAIT:
			default:
				break;
		}
#endif
	}
	return true;
}

void GeneratorFrameListener::moveCamera()
{
	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
	//(e.g. airplane)
	mCamera->yaw(mRotX);
	mCamera->pitch(mRotY);
	Ogre::Vector3 oldPosition=mCamera->getPosition();
	mCamera->moveRelative(mTranslateVector);
	pSphere->setObserverPosition(mCamera->getPosition());

	if( pSphere->getObserverDistanceToSurface()<=2.0f)// inside the sphere
	{
		mCamera->setPosition(oldPosition);
		return;
	}
	if(CollisionDetectionManager->checkCameraCollision().collided)
	{
		mCamera->setPosition(oldPosition);
		return;
	}
}

void GeneratorFrameListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}

bool GeneratorFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	//debug code

	//RootSceneNode->getChild("planetSphere")->roll(Ogre::Radian(0.04));
	pSphere->moveObject("Ramiro", 3, 0.5);
	//pSphere->moveObject("CK7", 2, 0.5);
	//pSphere->getObserverDistanceToSurface();

	return true;
}

bool GeneratorFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	//RootSceneNode->getChild("planetSphere")->roll(Ogre::Radian(0.004));

	if(mWindow->isClosed())	return false;

	mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	if( mJoy ) mJoy->capture();

	bool buffJ = (mJoy) ? mJoy->buffered() : true;

	Ogre::Vector3 lastMotion = mTranslateVector;

	//Check if one of the devices is not buffered
	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
	{
		// one of the input modes is immediate, so setup what is needed for immediate movement
		if (mTimeUntilNextToggle >= 0)
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// Move about 100 units per second
		//mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
		mMoveScale = 0.5*mCamera->getPosition().distance( Ogre::Vector3::ZERO ) * evt.timeSinceLastFrame;
		// Take about 10 seconds for full rotation
		mRotScale = mRotateSpeed * evt.timeSinceLastFrame;

		mRotX = 0;
		mRotY = 0;
		mTranslateVector = Ogre::Vector3::ZERO;

	}

	//Check to see which device is not buffered, and handle it
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
	if( !mKeyboard->buffered() )
		if( processUnbufferedKeyInput(evt) == false )
			return false;

#ifdef INCLUDE_RTSHADER_SYSTEM
	processShaderGeneratorInput();
#endif

#endif
	if( !mMouse->buffered() )
		if( processUnbufferedMouseInput(evt) == false )
			return false;

	// ramp up / ramp down speed
	if (mTranslateVector == Ogre::Vector3::ZERO)
	{
		// decay (one third speed)
		mCurrentSpeed -= evt.timeSinceLastFrame * 0.3;
		mTranslateVector = lastMotion;
	}
	else
	{
		// ramp up
		mCurrentSpeed += evt.timeSinceLastFrame;

	}
	// Limit motion speed
	if (mCurrentSpeed > 1.0)
		mCurrentSpeed = 1.0;
	if (mCurrentSpeed < 0.0)
		mCurrentSpeed = 0.0;

	mTranslateVector *= mCurrentSpeed;


	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
		moveCamera();

	return true;
}

bool GeneratorFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	updateStats();
	return true;
}

GeneratorFrameListener::~GeneratorFrameListener()
{		
	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}