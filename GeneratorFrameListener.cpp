#include "GeneratorFrameListener.h"
#include <iostream>

void GeneratorFrameListener::updateStats(void)
	{

 
		// Create a panel
		//OverlayContainer* panel = static_cast<OverlayContainer*>(
			//overlayManager.createOverlayElement("Panel", "DebugPanel"));
		/*Ogre::OverlayContainer *panel = static_cast<Ogre::OverlayContainer *> (om.createOverlayElement("BorderPanel", "Core/StatPanel"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(100, 100);
		//panel->setMaterialName("MaterialName"); // Optional background material
 
		// Create a text area
		TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(
			overlayManager.createOverlayElement("TextArea", "TextAreaName"));
		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(0, 0);
		textArea->setDimensions(100, 100);
		textArea->setCaption("Hello, World!");
		textArea->setCharHeight(16);
		//textArea->setFontName("TrebuchetMSBold");
		textArea->setColourBottom(ColourValue(0.3, 0.5, 0.3));
		textArea->setColourTop(ColourValue(0.5, 0.7, 0.5));
 
		// Create an overlay, and add the panel
		Overlay* overlay = overlayManager.create("OverlayName");
		overlay->add2D(panel);*/

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
GeneratorFrameListener::~GeneratorFrameListener()
	{		
		//Remove ourself as a Window listener
		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
		windowClosed(mWindow);
	}
bool GeneratorFrameListener::processUnbufferedKeyInput(const Ogre::FrameEvent& evt)
	{
		Ogre::Real moveScale = mMoveScale;
		if(mKeyboard->isKeyDown(OIS::KC_L))
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

			/*if( !CollisionManager->raycastFromCamera(mWindow,mCamera,*Vec,result,tmpE,distToColl) )
			{
					mTranslateVector.z = -moveScale;
			}	*/
			//cout<<pSphere->getObserverDistanceToSurface();
			//if( pSphere->getObserverDistanceToSurface()>=2.0f)
			{
					//mTranslateVector.z = -moveScale;
					mTranslateVector.z = -pSphere->getRadius()*0.1;
			}

		}
			

		if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
		{
			Ogre::Entity *tmpE = NULL;
			Ogre::Vector3 result = Ogre::Vector3::ZERO;
			float distToColl;
			Ogre::Vector2 *Vec=new Ogre::Vector2(mMouse->getMouseState().X.abs,mMouse->getMouseState().Y.abs);
			//mTranslateVector.z = moveScale;// Move camera backward
			//if( CollisionManager->raycastFromCamera(mWindow,mCamera,*Vec,result,tmpE,distToColl) )
				//mTranslateVector.z = -moveScale;	// Move camera back
			//if( !CollisionManager->raycastFromCamera(mWindow,mCamera,*Vec,result,tmpE,distToColl) )
			//if( pSphere->getObserverDistanceToSurface()>=2.0f)
			{
				//mTranslateVector.z = moveScale;
				mTranslateVector.z = pSphere->getRadius()*0.1;
			}
					
		}
			

		if(mKeyboard->isKeyDown(OIS::KC_PGUP))
			mTranslateVector.y = moveScale;	// Move camera up

		if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
			mTranslateVector.y = -moveScale;	// Move camera down

		if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
			mCamera->yaw(-mRotScale);

		if(mKeyboard->isKeyDown(OIS::KC_LEFT))
			mCamera->yaw(mRotScale);

		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
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

Ogre::Quaternion GeneratorFrameListener::RotationBetweenVectors(Ogre::Vector3 start, Ogre::Vector3 dest){
		start = start.normalisedCopy();
		dest = dest.normalisedCopy();

		float cosTheta = start.dotProduct(dest);
		Ogre::Vector3 rotationAxis;

		if (cosTheta < -1 + 0.001f){
			// special case when vectors in opposite directions:
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			rotationAxis = Ogre::Vector3(0.0f, 0.0f, 1.0f).crossProduct(start);
			
			if (rotationAxis.length()*rotationAxis.length() < 0.01 ) // bad luck, they were parallel, try again!
				rotationAxis = Ogre::Vector3(1.0f, 0.0f, 0.0f).crossProduct(start);

			rotationAxis = rotationAxis.normalisedCopy();
			return Ogre::Quaternion( Ogre::Radian(180.0f), rotationAxis);
		}

		rotationAxis = start.crossProduct(dest);

		float s = sqrt( (1+cosTheta)*2 );
		float invs = 1 / s;

		return Ogre::Quaternion(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);

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
		//later need to detect all the collision between camera and objects
		if( pSphere->getObserverDistanceToSurface()<=2.0f)
		{
			mCamera->setPosition(oldPosition);
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
		RootSceneNode->getChild("planetSphere")->roll(Ogre::Radian(0.004));
		//pSphere->getObserverDistanceToSurface();

		//Physical movement according to the gravity
		/*Ogre::SceneNode::ChildNodeIterator it = RootSceneNode->getChild("planetSphere")->getChildIterator();
		while(it.hasMoreElements())
		{
			
			
			Ogre::Vector3 oldPosition = it.current()->second->getPosition();

			//it.current()->second->setPosition( *new Ogre::Vector3(oldPosition.x-0.01,oldPosition.y-0.01,oldPosition.z-0.01));
			it.current()->second->setPosition( oldPosition*0.98 );
			
			if (CollisionManager->collidesWithEntity(oldPosition, it.current()->second->getPosition(),0.4F))
			{
				it.current()->second->setPosition(oldPosition);
			}
			it.getNext();
		}
		//change orientation
		it = RootSceneNode->getChild("planetSphere")->getChildIterator();
		while(it.hasMoreElements())
		{
			
			
			//Vector3 oldPosition = it.current()->second->getPosition();
			//Vector3 gravity = Vector3::ZERO-oldPosition;
			//Vector3 direction = (it.current()->second->getOrientation() * gravity).UNIT_SCALE*0.01;
			//Vector3 yAxis=Vector3(Math::Abs(it.current()->second->_getDerivedPosition().x),Math::Abs(it.current()->second->_getDerivedPosition().y),Math::Abs(it.current()->second->_getDerivedPosition().z));
			//Ogre::Vector3 yAxis=Ogre::Vector3(it.current()->second->_getDerivedPosition().x,it.current()->second->_getDerivedPosition().y,it.current()->second->_getDerivedPosition().z);
			Ogre::Vector3 yAxis=Ogre::Vector3(it.current()->second->getPosition().x,it.current()->second->getPosition().y,it.current()->second->getPosition().z);
			//move
			//it.current()->second->setPosition( oldPosition + direction );
			//Quaternion q( Degree( 10 ), gravity );
			//it.current()->second->setOrientation( q );
			//Quaternion *q = new Quaternion( Ogre::Degree(10.0f), Vector3::UNIT_X );
			Ogre::Quaternion q = RotationBetweenVectors(Ogre::Vector3::UNIT_Y,yAxis);
			
			//if( q.getPitch() != Ogre::Radian(0.0f) && q.getRoll() != Ogre::Radian(0.0f) && q.getYaw() != Ogre::Radian(0.0f) )
			//it.current()->second->pitch(q.getPitch());
			//it.current()->second->roll(q.getRoll());
			//it.current()->second->yaw(q.getYaw());
			it.current()->second->setOrientation( q );
			//it.current()->second->pitch( Ogre::Degree(10.0f) ); //Y axis
			//it.current()->second->rotate(gravity ,Ogre::Degree(1.0f));
			//it.current()->second->_getDerivedOrientation()
			
			
			//it.current()->second->setOrientation(RotationBetweenVectors(-it.current()->second->_getDerivedPosition(),Vector3::UNIT_Y));
			it.getNext();
		}
		//move forword
		
		it = RootSceneNode->getChild("planetSphere")->getChildIterator();
		while(it.hasMoreElements())
		{
			Ogre::Vector3 oldPosition = it.current()->second->getPosition();
			//Vector3 yAxis=Vector3(Math::Abs(it.current()->second->_getDerivedPosition().x),Math::Abs(it.current()->second->_getDerivedPosition().y),Math::Abs(it.current()->second->_getDerivedPosition().z));
			//Ogre::Vector3 yAxis=Ogre::Vector3(it.current()->second->_getDerivedPosition().x,it.current()->second->_getDerivedPosition().y,it.current()->second->_getDerivedPosition().z);
			Ogre::Vector3 yAxis=Ogre::Vector3(it.current()->second->getPosition().x,it.current()->second->getPosition().y,it.current()->second->getPosition().z);
			Ogre::Quaternion q = RotationBetweenVectors(Ogre::Vector3::UNIT_Y,yAxis);
			Ogre::Vector3 direction = (q*Ogre::Vector3::UNIT_Z).normalisedCopy();
			
			it.current()->second->setPosition( oldPosition + direction*0.15 );
			
			it.getNext();
		}
		*/
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