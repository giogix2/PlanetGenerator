#include "initOgre.h"

#include "OgreConfigFile.h"
#include <OgreMeshSerializer.h>

#ifdef OGRE_PLATFORM_LINUX
#include <sys/stat.h>
#endif

initOgre::initOgre(){
		FrameListener = 0;
		Root = 0;
		OverlaySystem = 0;

	}

int initOgre::start(){

	Root = new Ogre::Root("", "", "Generator.LOG");
	OverlaySystem = new Ogre::OverlaySystem();


	Ogre::String PluginName;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PluginName.append("RenderSystem_Direct3D9");
#else
	/* Use Posix-function stat to check existence of a file.
	 * This is needed because Ogre makes zero effort to find its own plugins. */
	struct stat statBuf;
	Ogre::String PluginFile[3];

	PluginFile[0].append("/usr/lib/OGRE/RenderSystem_GL.so");
	PluginFile[1].append("/usr/lib/local/OGRE/RenderSystem_GL.so");
	// Ubuntu
#ifdef __x86_64__
	PluginFile[2].append("/usr/lib/x86_64-linux-gnu/OGRE-");
#elif __i386__
	PluginFile[2].append("/usr/lib/i386-linux-gnu/OGRE-");
#endif
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MAJOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MINOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_PATCH);
	PluginFile[2] += "/RenderSystem_GL.so";

	int i;
	for(i=0; i < 3; i++)
	{
		if( stat(PluginFile[i].c_str(), &statBuf) == 0 )
		{
			PluginFile[i].resize(PluginFile[i].size()-3);	// strip ".so"
			PluginName.assign( PluginFile[i] );
			break;											// Exit loop if file exists
		}
	}
	if(PluginName == "")
		PluginName.append("RenderSystem_GL");				// When all else fails...
#endif

	// RenderSystem_GL_d if using Ogre debug mode
	if(OGRE_DEBUG_MODE)
		PluginName.append("_d");
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

void initOgre::CreateFrameListener(){

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	FrameListener= new GeneratorFrameListener(Window, Camera, true, true, true);
#else
	FrameListener= new GeneratorFrameListener(Window, Camera,RootSceneNode,Scene);
#endif
	//FrameListener->showDebugOverlay(true);
	Root->addFrameListener(FrameListener);
}

void initOgre::setSceneAndRun(PSphere *planet){

	// Create camera
	Camera = Scene->createCamera("VertCamera");

	// Camera position
	Camera->setPosition(Ogre::Vector3(0,0,20));
	// Camera looks toward origo
	Camera->lookAt(Ogre::Vector3(0,0,0));

	// Create viewport
	Ogre::Viewport *vp = Window->addViewport(Camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Draw distances
	Camera->setNearClipDistance(1.5f);
	Camera->setFarClipDistance(3000.0f);     // note to self: do not set if using stencil shadows

	// Let there be light
	Scene->setAmbientLight(Ogre::ColourValue(0.02f, 0.02f, 0.02f));
	Ogre::Light *light = Scene->createLight( "PointLight" );
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(200, 40, 150);


	// Draw a sphere
	planet->loadToBuffers("CustomMesh", "sphereTex");

	//Export the shape in a mesh file before destroying it
	//Ogre::MeshPtr mesh;
	//mesh = planet->getMesh();
	//Ogre::MeshSerializer ser;
	//ser.exportMesh(mesh.getPointer(), "C:\\Users\\giova\\Documents\\PlanetGenerator\\planet.mesh",  Ogre::MeshSerializer::ENDIAN_NATIVE);


	// Attach entitys to sceneNodes
	Ogre::SceneNode *CameraNode = RootSceneNode->createChildSceneNode("DefaultCameraNode");
	CameraNode->attachObject(Camera);

	Ogre::Entity *entity1 = Scene->createEntity("CustomEntity", "CustomMesh");
	Ogre::SceneNode *sphere1 = Scene->getRootSceneNode()->createChildSceneNode("planetSphere");
	sphere1->attachObject(entity1);


	planet->loadMeshFile("Cube2.mesh", "LocalMesh");

	planet->attachMesh(sphere1, Scene, "LocalMesh", 0.0, 0.0);

	//cat head just for testing the movement
	
	//Ogre::Entity *entity2 = Scene->createEntity("Head", "ogrehead.mesh");
	//Ogre::SceneNode *cat = sphere1->createChildSceneNode("cat",Ogre::Vector3(6.4, 6.4, 6.4));
	//cat->setScale(0.01,0.01,0.01);
	//cat->attachObject(entity2);



	// No need for this anymore
	//Ogre::MeshManager::getSingleton().remove("CustomMesh");

	Ogre::MaterialPtr textureMap = Ogre::MaterialManager::getSingleton()
			.create("TextureObject",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	textureMap->getTechnique(0)->getPass(0)->createTextureUnitState("sphereTex");
	textureMap->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	// Set texture for the sphere
	entity1->setMaterial(textureMap);

	sphere1->setOrientation(1.3003361e-01f, -1.5604560e-01f, -7.5052901e-01f, 6.2884596e-01f);

	//createFrameListener
	CreateFrameListener();

	//start Rendering
	Root->startRendering();
}

void initOgre::cleanup(){

	// Clean up our mess before exiting
	Ogre::MaterialManager::getSingleton().remove("TextureObject");
	Ogre::TextureManager::getSingleton().remove("sphereTex");
	Scene->destroyEntity("CustomEntity");
	Scene->destroySceneNode("planetSphere");
	Scene->destroySceneNode("DefaultCameraNode");
	Scene->destroyCamera(Camera);
	Scene->destroyLight("PointLight");
	Ogre::Root::getSingleton().getRenderSystem()->destroyRenderWindow("My little planet");
	Scene->clearScene();
	Root->shutdown();
	Root->destroySceneManager(Scene);

	Ogre::LogManager::getSingleton().logMessage("Ogre is cleaned up");
}
