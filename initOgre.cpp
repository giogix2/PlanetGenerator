#include "OGRE/Ogre.h"
#include "initOgre.h"
#include "GeneratorFrameListener.h"

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
		PluginName.append("RenderSystem_GL");					// When all else fails...
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

void initOgre::CreateFrameListener(){

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	FrameListener= new GeneratorFrameListener(Window, Camera, true, true, true);
#else
	FrameListener= new GeneratorFrameListener(Window, Camera,RootSceneNode);
#endif
	FrameListener->showDebugOverlay(true);
	Root->addFrameListener(FrameListener);
}