/* Disclaimer: For demo use only! It is one main()-blob by design,
 * or rather because of lack of design.
 *
 * No animals were harmed while making of this program...
 * ok, i consumed some tuna while coding, but those were dead already.
 */

#include <iostream>
#include <exception>

#include "OGRE/Ogre.h"
#include "PSphere.h"
#include "OgreConfigFile.h"
#include "GeneratorFrameListener.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

 class initOgre
 {
 public:
	Ogre::Root              *Root;
	Ogre::SceneManager      *Scene;
	Ogre::SceneNode         *RootSceneNode;
	Ogre::RenderWindow      *Window;
	Ogre::Camera            *Camera;
	GeneratorFrameListener  *FrameListener;
	Ogre::OverlaySystem     *OverlaySystem;

	initOgre()
	{
		FrameListener = 0;
		Root = 0;
		OverlaySystem = 0;

	}
	int start();
	void CreateFrameListener();

};

int initOgre::start()
{

	Root = new Ogre::Root("", "", "Generator.LOG");
	OverlaySystem = new Ogre::OverlaySystem();


	Ogre::String PluginName;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PluginName.append("RenderSystem_Direct3D9");
#else
	// Note to self: can the absolute path be avoided?
	PluginName.append("/usr/lib/OGRE/RenderSystem_GL");
#endif
	if(OGRE_DEBUG_MODE)
		PluginName.append("_d");    // RenderSystem_GL_d if using Ogre debug mode

	// Loads renderer plugin
	Root->loadPlugin(PluginName);


	// Check renderer availability
	const Ogre::RenderSystemList& RenderSystemList = Root->getAvailableRenderers();
	if( RenderSystemList.size() == 0 )
	{
		Ogre::LogManager::getSingleton().logMessage("Rendersystems not found.");
		return 1;
	}
	Ogre::RenderSystem *RenderSystem = RenderSystemList[0];
	Root->setRenderSystem(RenderSystem);


	// Do not create window automatically
	Root->initialise(false);

	Ogre::NameValuePairList renderParams;

	renderParams["FSAA"] = "0";
	renderParams["vsync"] = "true"; // Waits for vertical sync

	// Actual call to create window, bool value is fullscreen flag
	Window = Root->createRenderWindow("My little planet", 800, 600, false, &renderParams);


	// Start creating scene
	Scene = Root->createSceneManager(Ogre::ST_GENERIC);
	RootSceneNode = Scene->getRootSceneNode();

	if(OverlaySystem)
		Scene->addRenderQueueListener(OverlaySystem);

	return 0;
}
void initOgre::CreateFrameListener()
{

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	FrameListener= new GeneratorFrameListener(Window, Camera, true, true, true);
#else
	FrameListener= new GeneratorFrameListener(Window, Camera,RootSceneNode);
#endif
	FrameListener->showDebugOverlay(true);
	Root->addFrameListener(FrameListener);
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{

	try
	{
		initOgre myOgre;

		myOgre.start();

		// Create camera
		myOgre.Camera = myOgre.Scene->createCamera("VertCamera");

		// Attach camera to sceneNode
		Ogre::SceneNode *CameraNode = myOgre.RootSceneNode->createChildSceneNode("DefaultCameraNode");
		CameraNode->attachObject(myOgre.Camera);

		// Camera position
		myOgre.Camera->setPosition(Ogre::Vector3(0,0,20));
		// Camera looks toward origo
		myOgre.Camera->lookAt(Ogre::Vector3(0,0,0));

		// Create viewport
		Ogre::Viewport *vp = myOgre.Window->addViewport(myOgre.Camera);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

		// Draw distances
		myOgre.Camera->setNearClipDistance(1.5f);
		myOgre.Camera->setFarClipDistance(3000.0f);     // note to self: do not set if using stencil shadows

		// Let there be light
		myOgre.Scene->setAmbientLight(Ogre::ColourValue(0.02f, 0.02f, 0.02f));
		Ogre::Light *light = myOgre.Scene->createLight( "PointLight" );
		light->setType(Ogre::Light::LT_POINT);
		light->setPosition(200, 40, 150);


		// Draw a sphere
		PSphere mySphere;
		mySphere.create(15.0f, 0.6f, 100);
		mySphere.loadToBuffers("CustomMesh", "sphereTex");
		mySphere.destroy();

		Ogre::Entity *entity1 = myOgre.Scene->createEntity("CustomEntity", "CustomMesh");
		Ogre::SceneNode *sphere1 = myOgre.Scene->getRootSceneNode()->createChildSceneNode("planetSphere");
		sphere1->attachObject(entity1);

		// No need for this anymore
		Ogre::MeshManager::getSingleton().remove("CustomMesh");

		Ogre::MaterialPtr textureMap = Ogre::MaterialManager::getSingleton()
				.create("TextureObject",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		textureMap->getTechnique(0)->getPass(0)->createTextureUnitState("sphereTex");
		textureMap->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

		// Set texture for the sphere
		entity1->setMaterial(textureMap);

		sphere1->setOrientation(1.3003361e-01f, -1.5604560e-01f, -7.5052901e-01f, 6.2884596e-01f);

		//createFrameListener
		myOgre.CreateFrameListener();

		//start Rendering
		myOgre.Root->startRendering();

		// Clean up our mess before exiting
		Ogre::MaterialManager::getSingleton().remove("TextureObject");
		Ogre::TextureManager::getSingleton().remove("sphereTex");
		myOgre.Scene->destroyEntity("CustomEntity");
		myOgre.Scene->destroySceneNode(sphere1);
		myOgre.Scene->destroySceneNode(CameraNode);
		myOgre.Scene->destroyCamera(myOgre.Camera);
		myOgre.Scene->destroyLight(light);
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderWindow("My little planet");
		myOgre.Scene->clearScene();
		myOgre.Root->shutdown();
		myOgre.Root->destroySceneManager(myOgre.Scene);

		Ogre::LogManager::getSingleton().logMessage("The end is near");
	}
	// Let's make informative messages in case there is an error
	catch(Ogre::Exception &error)
	{
		std::cout << "Ogre makes an exception! " << error.what() << std::endl;
	}
	catch(std::exception &error)
	{
		std::cout << "Shit hit the fan! For details, see the fan " << error.what() << std::endl;
	}

	return 0;
}
