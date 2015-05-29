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

#include "initOgre.h"

#include "Common.h"

#include "OgreConfigFile.h"
#include <OgreMeshSerializer.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <sys/stat.h>
#endif

initOgre::initOgre(){
		FrameListener = NULL;
		Root = NULL;
		OverlaySystem = NULL;

	}
initOgre::~initOgre()
{
	//delete Root;
}

int initOgre::start(){

	Ogre::String PluginName;
	Ogre::String mResourcesCfg;
	mResourcesCfg = "resources.cfg";

	Root = new Ogre::Root("", mResourcesCfg, "Generator.LOG");
	OverlaySystem = new Ogre::OverlaySystem();


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PluginName.append("RenderSystem_Direct3D9");
#else
	PluginName = findPlugin();
#endif

	// RenderSystem_GL_d if using Ogre debug mode
	if(OGRE_DEBUG_MODE)
		PluginName.append("_d");
	// Loads renderer plugin
	Root->loadPlugin(PluginName);

	//Load information from resource.cfg file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);
	Ogre::String name, locType;
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
	while (secIt.hasMoreElements()) {
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;
		for (it = settings->begin(); it != settings->end(); ++it) {
			locType = it->first;
			name = it->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		}

	}

	//Ogre::ResourceGroupManager::getSingleton().declareResource("ram.mesh", "Mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("char_ram_col.jpg", "Font", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	////Ogre::ResourceGroupManager::getSingleton().declareResource("char_ram_nor.png", "Font", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("ram_skin.material", "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("ram_skin_eyelids.material", "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("ram_skin_eyes.material", "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("ram_skin_horns.material", "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());

	//Ogre::ResourceGroupManager::getSingleton().declareResource("asteroid.mesh", "Mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("Material.001.material", "Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());
	//Ogre::ResourceGroupManager::getSingleton().declareResource("space rock tex 3.jpg", "Font", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::NameValuePairList());

	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
	//Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
	Scene = Root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
	RootSceneNode = Scene->getRootSceneNode();

	if(OverlaySystem)
		Scene->addRenderQueueListener(OverlaySystem);

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	return 0;
}

void initOgre::CreateFrameListener(PSphere *pSphere){

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	FrameListener= new GeneratorFrameListener(Window, Camera, true, true, true);
#else
	FrameListener= new GeneratorFrameListener(Window, Camera,pSphere ,Scene , RootSceneNode ,CollisionDetectionManager);
#endif
	//FrameListener->showDebugOverlay(true);
	Root->addFrameListener(FrameListener);
}

void initOgre::setSceneAndRun(PSphere *planet){

	//background
	Scene->setSkyBox(true, "SpaceSkyBox",1000.0f,false);

	// Create camera
	Camera = Scene->createCamera("VertCamera");

	// Camera position
	Camera->setPosition(Ogre::Vector3(0,0,planet->getRadius()*2.65f));
	planet->setObserverPosition(Camera->getPosition());
	// Camera looks toward origo
	Camera->lookAt(Ogre::Vector3(0,0,0));

	// Create viewport
	Ogre::Viewport *vp = Window->addViewport(Camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Draw distances
	Camera->setNearClipDistance(1.5f);
	Camera->setFarClipDistance(3000.0f);     // note to self: do not set if using stencil shadows

	// Let there be light
	Scene->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	Ogre::Light *light = Scene->createLight( "PointLight" );
	light->setDiffuseColour(1.0, 1.0, 1.0);
	light->setSpecularColour(1.0, 1.0, 1.0);
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(200, 40, 150);


	// Draw a sphere
	planet->loadToBuffers("CustomMesh", "sphereTex");

	// Attach entitys to sceneNodes
	Ogre::SceneNode *CameraNode = RootSceneNode->createChildSceneNode("DefaultCameraNode");
	CameraNode->attachObject(Camera);

	Ogre::Entity *entity1 = Scene->createEntity("CustomEntity", "CustomMesh");
	Ogre::SceneNode *sphere1 = Scene->getRootSceneNode()->createChildSceneNode("planetSphere");
	sphere1->attachObject(entity1);

	srand(0);
	//planet->loadMeshFile("ram1.mesh", "LocalMesh");
	for (vector<pair <string, int> >::const_iterator iter = planet->getParameters()->getMeshLocObjAmount().begin(); iter != planet->getParameters()->getMeshLocObjAmount().end(); ++iter)
    {
        //qDebug() << QString::fromStdString(iter->first) <<", " << iter->second;
		for(int i=0; i<iter->second; i++) {
			
			double latitude = 90 - rand() % 180;
			double longitude = rand() % 360;
			// Check if location is on land
			if (planet->checkAccessibility(convertSphericalToCartesian(latitude, longitude)) == true)
			{
				planet->attachMeshOnGround(sphere1, Scene, iter->first, iter->first, latitude, longitude);
			}
			// Try same iteration again
			else
			{
				i--;
			}
		}
		

    }

	/*planet->attachMeshOnGround(sphere1, Scene, "ram.mesh", "Ramiro", 0.0, 270.0);*/
	planet->attachMesh(sphere1, Scene, "asteroid.mesh", "CK7", 0.0, 180.0);

	// No need for this anymore
	//Ogre::MeshManager::getSingleton().remove("CustomMesh");

	Ogre::MaterialPtr textureMap = Ogre::MaterialManager::getSingleton()
			.create("TextureObject",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	textureMap->getTechnique(0)->getPass(0)->createTextureUnitState("sphereTex");
	textureMap->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	// Set texture for the sphere
	entity1->setMaterial(textureMap);

	sphere1->setOrientation(0.163149834f, -0.19578641f, -0.314332321f, -0.9144643269f);

	//Collision Manager
	CollisionDetectionManager = new CollisionManager(planet->getObjects(),Camera);
	planet->setCollisionManager( CollisionDetectionManager );

	//createFrameListener
	CreateFrameListener(planet);

	

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
	Root->destroySceneManager(Scene);

	Root->shutdown();
	
	delete FrameListener;
	FrameListener = NULL;
	delete OverlaySystem;
	OverlaySystem = NULL;
	delete Root;
	Root = NULL;

	//Ogre::LogManager::getSingleton().logMessage("Ogre is cleaned up");
}

void initOgre::cleanupSavePlanetAsMesh(){

	// Clean up our mess before exiting
	Ogre::MaterialManager::getSingleton().remove("TextureObject");
	Ogre::TextureManager::getSingleton().remove("sphereTex");
	Scene->destroyEntity("CustomEntity");
	Ogre::Root::getSingleton().getRenderSystem()->destroyRenderWindow("My little planet");
	Scene->clearScene();
	Root->destroySceneManager(Scene);

	Root->shutdown();

	delete FrameListener;
	FrameListener = NULL;
	delete OverlaySystem;
	OverlaySystem = NULL;
	delete Root;
	Root = NULL;

}

void initOgre::savePlanetAsMesh(PSphere *planet, const std::string &exportFile)
{
	Ogre::String PluginName;

	Root = new Ogre::Root("", "", "Exporter.LOG");

	// Renderer is required to create window
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PluginName.append("RenderSystem_Direct3D9");
#else
	PluginName = findPlugin();
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
		return;
	}
	Ogre::RenderSystem *RenderSystem = RenderSystemList[0];
	Root->setRenderSystem(RenderSystem);


	// Do not create window automatically
	Root->initialise(false);

	// Actual call to create window, bool value is fullscreen flag
	Window = Root->createRenderWindow("My little planet", 800, 600, false);

	// Try to hide. Flashes anyway.
	Window->setHidden(true);
	Ogre::WindowEventUtilities::messagePump();

	// Required to create entity
	Scene = Root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);

//	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// manipulate filename
	const size_t last_slash = exportFile.find_last_of("\\/");
	std::string name = exportFile;

	name.erase(0, last_slash + 1);
	name.erase(name.length()-5, name.length()); // Name of files without extension

	// Materials name
	std::string materialName = name + "_Material";


	// Create mesh
	planet->loadToBuffers("CustomMesh", "sphereTex");

	Ogre::Entity *sphereEntity = Scene->createEntity("CustomEntity", "CustomMesh");

	/* Material parameters are probably useless to set up as we are only
	 * interested on setting material name for a mesh */
	Ogre::MaterialPtr matSphere;
	matSphere = Ogre::MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Pass *pass = matSphere->getTechnique(0)->getPass(0);
	pass->setLightingEnabled(true);
	pass->setDepthCheckEnabled(true);
	pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	Ogre::TextureUnitState *tex = pass->createTextureUnitState("sphereTex", 0);
	tex->setColourOperation(Ogre::LBO_MODULATE);
	tex->setTextureFiltering(Ogre::TFO_TRILINEAR);

	matSphere->load();
	/* Set texture for the sphere, does nothing if you want to export mesh with
	 * material. */
	sphereEntity->setMaterial(matSphere);
	// Mesh has now information for material name
	sphereEntity->getMesh()->getSubMesh(0)->setMaterialName(materialName);

	//Export the shape in a mesh file before destroying it
	Ogre::MeshPtr mesh;
	mesh = sphereEntity->getMesh();
	Ogre::MeshSerializer ser;
	ser.exportMesh(mesh.getPointer(), exportFile,  Ogre::MeshSerializer::ENDIAN_NATIVE);

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	std::string materialPath = "media\\materials\\scripts\\";
	std::string texturePath = "media\\materials\\textures\\";
#else
	std::string materialPath = "media/materials/scripts/";
	std::string texturePath = "media/materials/textures/";
#endif

	std::string textureFile = name + ".png";
	if (!planet->exportMap(1024, 512, texturePath+textureFile, PSphere::MAP_EQUIRECTANGULAR))
	{
		Ogre::LogManager::getSingleton().logMessage("Saving texture failed!");
	}
	else
	{
		Ogre::LogManager::getSingleton().logMessage("Saving texture succeeded!");
	}

	std::string materialFile = name + ".material";
	saveMaterialFile(materialPath+materialFile, sphereEntity->getMesh()->getSubMesh(0)->getMaterialName(), textureFile);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
std::string initOgre::findPlugin()
{
	/* Use Posix-function stat to check existence of a file.
	 * This is needed because Ogre makes zero effort to find its own plugins. */
	struct stat statBuf;
	Ogre::String PluginName, PluginFile[4];

	PluginFile[0].append("/usr/lib/OGRE/RenderSystem_GL.so");
	PluginFile[1].append("/usr/lib/local/OGRE/RenderSystem_GL.so");

	// Ubuntu and Fedora
#ifdef __x86_64__
	PluginFile[2].append("/usr/lib/x86_64-linux-gnu/OGRE-");
	PluginFile[3].append("/usr/lib64/OGRE/RenderSystem_GL.so");
#elif __i386__
	PluginFile[2].append("/usr/lib/i386-linux-gnu/OGRE-");
	PluginFile[3].append("/usr/lib32/OGRE/RenderSystem_GL.so");
#endif
	// Ubuntu
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MAJOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_MINOR) + ".";
	PluginFile[2] += Ogre::StringConverter::toString(OGRE_VERSION_PATCH);
	PluginFile[2] += "/RenderSystem_GL.so";

	int i;
	for(i=0; i < 4; i++)
	{
		if( stat(PluginFile[i].c_str(), &statBuf) == 0 )
		{
			// strip ".so"
			PluginFile[i].resize(PluginFile[i].size()-3);
			PluginName.assign( PluginFile[i] );
			// Exit loop if file exists
			break;
		}
	}
	if(PluginName == "")
		Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
													"Could not find RenderSystem_GL.so!");

	return PluginName;
}
#endif

void initOgre::saveMaterialFile(const std::string &fileName, const std::string &material, const std::string &textureName)
{
	std::ofstream matFile;

	matFile.open(fileName.c_str());
	matFile << "material " << material << "\n{\n";
	matFile << "    receive_shadows on\n\n    technique\n    {\n        pass\n";
	matFile << "        {\n            texture_unit\n            {\n";
	matFile << "                texture " << textureName << "\n";
	matFile << "                tex_address_mode wrap\n";
	matFile << "                scale 1.0 -1.0\n";
	matFile << "                colour_op modulate\n";
	matFile << "            }\n        }\n    }\n}\n";
	matFile.close();
}
