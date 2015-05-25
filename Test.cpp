#include "Test.h"

void testAll()
{
	PSphere *testSphere;

	initTest(&testSphere);
	test_PSphere_checkAccessibility(testSphere);
	cout<<"PSphere:___checkAccessibility___ function Passed."<<endl;
	test_PSphere_getRadius(testSphere);
	cout<<"PSphere:___getRadius___ function Passed."<<endl;
	test_PSphere_getSurfaceHeight(testSphere);
	cout<<"PSphere:___getSurfaceHeight___ function Passed."<<endl;
	
}

bool initTest(PSphere **Sphere)
{
	//Init
    std::vector < std::pair < std::string, int > > meshlocs;
	meshlocs.push_back(std::make_pair("media/models/ram.mesh", 1));
	meshlocs.push_back(std::make_pair("media/models/asteroid.mesh", 1));
	std::ResourceParameter *params = new std::ResourceParameter((std::string)"#00FF00",(std::string)"#FACD00",(std::string)"#32CDFF"
        ,(std::string)"#64FFFF",(std::string)"#B4B4B4",(std::string)"#FFFFFF",0.6f,7.5f,60,"1.0 0.02 0.3 0.008 0.1 0.005 0.06666 0.006666", meshlocs);

	*Sphere = new PSphere(100, 0, 0, 0, *params);

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