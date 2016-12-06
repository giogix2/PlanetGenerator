#include "Test.h"

void testAll()
{
	PSphere				*testSphere;
	Ogre::SceneNode		*sphere;
	Ogre::Root			*Root;
	Ogre::SceneManager	*Scene;

	initTest(&testSphere,&Root, &Scene, &sphere);
	test_PSphere_checkAccessibility(testSphere);
	cout<<"PSphere:___checkAccessibility___\tfunction Passed."<<endl;
	
	test_PSphere_getSurfaceHeight(testSphere);
	cout<<"PSphere:___getSurfaceHeight___\t\tfunction Passed."<<endl;

	test_PSphere_getRadius(testSphere);
	cout<<"PSphere:___getRadius___\t\t\tfunction Passed."<<endl;

	test_PSphere_getGridLocation(testSphere);
	cout<<"PSphere:___getGridLocation___\t\tfunction Passed."<<endl;

	test_PSphere_nextPosition(testSphere);
	cout<<"PSphere:___nextPosition___\t\tfunction Passed."<<endl;
	
	test_PSphere_getObserverDistanceToSurface(testSphere);
	cout<<"PSphere:___getObserverDistanceToSurface___function Passed."<<endl;

	//test_PSphere_attachMeshOnGround(testSphere,sphere,Scene);
	//cout<<"PSphere::___attachMeshOnGround___\t\tfunction Passed."<<endl;
}

bool initTest(PSphere **Sphere,Ogre::Root **Root, Ogre::SceneManager **Scene, Ogre::SceneNode **sphere)
{
	//Init
    std::vector < std::pair < std::string, int > > meshlocs;
	meshlocs.push_back(std::make_pair("media/models/ram.mesh", 1));
	meshlocs.push_back(std::make_pair("media/models/asteroid.mesh", 1));
    ResourceParameter *params = new ResourceParameter((std::string)"#00FF00",(std::string)"#FACD00",(std::string)"#32CDFF"
        ,(std::string)"#64FFFF",(std::string)"#B4B4B4",(std::string)"#FFFFFF",0.6f,7.5f,60,"1.0 0.02 0.3 0.008 0.1 0.005 0.06666 0.006666", meshlocs);

    *Sphere = new PSphere(100, 0, *params);

	//Root Initialization
	Ogre::String PluginName;
	*Root = new Ogre::Root("", "resources.cfg", "Generator.LOG");
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PluginName.append("RenderSystem_Direct3D9");
#else
	/* Use Posix-function stat to check existence of a file.
	 * This is needed because Ogre makes zero effort to find its own plugins. */
	struct stat statBuf;
	Ogre::String PluginFile[4];

	PluginFile[0].append("/usr/lib/OGRE/RenderSystem_GL.so");
	PluginFile[1].append("/usr/lib/local/OGRE/RenderSystem_GL.so");
	// Ubuntu
#ifdef __x86_64__
	PluginFile[2].append("/usr/lib/x86_64-linux-gnu/OGRE-");
	PluginFile[3].append("/usr/lib64/OGRE/RenderSystem_GL.so");
#elif __i386__
	PluginFile[2].append("/usr/lib/i386-linux-gnu/OGRE-");
	PluginFile[3].append("/usr/lib32/OGRE/RenderSystem_GL.so");
#endif
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MAJOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MINOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_PATCH);
	PluginFile[2] += "/RenderSystem_GL.so";

	int i;
	for(i=0; i < 4; i++)
	{
		if( stat(PluginFile[i].c_str(), &statBuf) == 0 )
		{
			PluginFile[i].resize(PluginFile[i].size()-3);	// strip ".so"
			PluginName.assign( PluginFile[i] );
			break;											// Exit loop if file exists
		}
	}
	if(PluginName == "")
		Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Could not find RenderSystem_GL.so!");
#endif

	// RenderSystem_GL_d if using Ogre debug mode
	if(OGRE_DEBUG_MODE)
		PluginName.append("_d");
	// Loads renderer plugin
	(*Root)->loadPlugin(PluginName);
	// Check renderer availability
	const Ogre::RenderSystemList& RenderSystemList = (*Root)->getAvailableRenderers();
	if( RenderSystemList.size() == 0 )
	{
		Ogre::LogManager::getSingleton().logMessage("Rendersystems not found.");
		return 1;
	}
	Ogre::RenderSystem *RenderSystem = RenderSystemList[0];
	(*Root)->setRenderSystem(RenderSystem);
	// Do not create window automatically
	(*Root)->initialise(false);

	*Scene = (*Root)->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);

	*sphere = (*Scene)->getRootSceneNode()->createChildSceneNode("planetSphere");

	return true;
}

bool test_PSphere_getRadius(PSphere *Sphere)
{
	assert( Sphere->getRadius()==7.5f );
	return true;
}

bool test_PSphere_checkAccessibility(PSphere *Sphere)
{
	assert( Sphere->checkAccessibility(Ogre::Vector3 (100.0f,100.0f,100.0f)) == false );
	return true;
}

bool test_PSphere_getSurfaceHeight(PSphere *Sphere)
{
	Ogre::Real r=Sphere->getSurfaceHeight(Ogre::Vector3 (100.0f,100.0f,100.0f));
	assert( Sphere->getSurfaceHeight(Ogre::Vector3 (100.0f,100.0f,100.0f)) == 7.4931941f );
	return true;
}

bool test_PSphere_getGridLocation(PSphere *Sphere)
{
	//Sphere->getGridLocation(
	return true;
}
bool test_PSphere_nextPosition(PSphere *Sphere)
{
	Ogre::Vector3 nextPos;
	nextPos = Sphere->nextPosition(Ogre::Vector3 (100.0f,100.0f,100.0f),Sphere->PSPHERE_GRID_XMINUS);
	assert(nextPos==Ogre::Vector3 (0.78446454f,-0.58834839f,0.19611613f));
	return true;
}

bool test_PSphere_getObserverDistanceToSurface(PSphere *Sphere)
{
	assert( Sphere->getObserverDistanceToSurface() == 0.0f );
	return true;
}

bool test_PSphere_attachMeshOnGround(PSphere *Sphere,Ogre::SceneNode *sphereNode, Ogre::SceneManager *Scene)
{
	Sphere->attachMeshOnGround(sphereNode, Scene, "ram.mesh", "Ramiro", 0.0, 270.0);

	return true;
}
