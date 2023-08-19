#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreLight.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreBitesConfigDialog.h"
#include "OgreConfigDialog.h"

class TutorialApplication
	: public OgreBites::ApplicationContext,
	public OgreBites::InputListener

{
public:
	TutorialApplication();
	virtual ~TutorialApplication();

	void setup();
	bool keyPressed(const OgreBites::KeyboardEvent& evt);

};

TutorialApplication::TutorialApplication()
	: OgreBites::ApplicationContext("OgreTutorialApp")

{

}

TutorialApplication::~TutorialApplication() 

{

}

void TutorialApplication::setup()
{
	// important to call the base first
	OgreBites::ApplicationContext::setup();
	addInputListener(this);

	// set the root and spawn our scnMgr from it

	Ogre::Root* root = getRoot();
	Ogre::SceneManager* scnMgr = root->createSceneManager();

	//root->showConfigDialog();

	//config
	/*
	if (mroot->showConfigDialog()) {
		return false;
	}
	*/

	//m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

	/*
	Ogre::RenderSystem* renderSystem =
		root->getRenderSystemByName("Direct3D11 Rendering Subsystem");
	if (!(renderSystem))
	{
		printf("Render system not found!\n");
		return;
	}
	root->setRenderSystem(renderSystem);
	*/



	// register our scene with the RTSS
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scnMgr);

	// camera creation

	Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Camera* cam = scnMgr->createCamera("myCam");

	camNode->setPosition(0, 200, 400);
	camNode->lookAt(Ogre::Vector3(50, 50, 0), Ogre::Node::TransformSpace::TS_WORLD);

	cam->setNearClipDistance(5);
	camNode->attachObject(cam);


	// create a viewport

	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// set aspectratio

	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// set ambiance and shadow technique

	scnMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	// making a plane
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane
	(
		"plane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z
	);

	// creating entities

	//1 - Plane
	Ogre::Entity* ent = scnMgr->createEntity("LightPlaneEntity", "plane");
	scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setCastShadows(false);
	ent->setMaterialName("Examples/BeachStones");

	/*
	Besides the plane definition, we need to give the plane a name. When loading meshes from the 
	disk, the file's name is used as the resource name, resource name. 
	It also needs an resource group it belongs to, resource groups are like namespaces in C++. 
	The third parameter is the plane definition and the fourth and fifth parameters are the size of 
	the plane. The sixth and seventh parameters are used to say how many segments the 
	plane should have.
	*/

	/*
	After we have defined how many segments we want, we pass a Boolean parameter 
	which tells Ogre 3D that we want the normal of the plane to be calculated. 
	As said before, a normal is a vector which stands vertically on the surface of the plane. 
	The next three parameters are used for texturing. To texture something all points need 
	texture coordinates. Texture coordinates tell the render engine how to map the texture 
	onto the triangle. Because a picture is a 2D surface, the texture coordinates also consist 
	of two values, namely, x and y. They are presented as a tuple (x,y). 
	The value range of texture coordinates is normalized from zero to one. (0,0) means the 
	upper-left corner of the texture and (1,1) the bottom-right corner. 
	Sometimes the values can be greater than 1. 
	This means that the texture could be repeated depending on the set mode. 
	This topic will be explained in a later chapter extensively. (2,2) could repeat the texture 
	twice across both axis. The tenth and eleventh parameters tell Ogre 3D how often we 
	want the texture to be tiled across the plane. 
	The ninth parameter defines how many textures' coordinates we want. 
	This can be useful when working with more than one texture for one surface. 
	The last parameter defines the "up" direction for our textures. 
	This also affects how the texture coordinates are generated. 
	We simply say that the z-axis should be "up" for our plane.
	*/

	//2 - Nodes

	Ogre::SceneNode* lightnode = scnMgr->createSceneNode("Node1");
	scnMgr->getRootSceneNode()->addChild(lightnode);
	lightnode->setPosition(-25, 100, 0);
	lightnode->setDirection(Ogre::Vector3(1, -1, 0));

	Ogre::SceneNode* spherenode = scnMgr->createSceneNode("Node11");
	scnMgr->getRootSceneNode()->addChild(spherenode);
	spherenode->setPosition(-110, 100, -55);

	Ogre::SceneNode* lightnode2 = scnMgr->createSceneNode("Node2");
	scnMgr->getRootSceneNode()->addChild(lightnode2);
	lightnode2->setPosition(125, 100, 0);
	lightnode2->setDirection(Ogre::Vector3(-1, -1, 0));

	Ogre::SceneNode* spherenode2 = scnMgr->createSceneNode("Node21");
	scnMgr->getRootSceneNode()->addChild(spherenode2);
	spherenode2->setPosition(185, 100, -55);

	Ogre::SceneNode* lightnode3 = scnMgr->createSceneNode("Node3");
	scnMgr->getRootSceneNode()->addChild(lightnode3);
	lightnode3->setPosition(45, 100, -100);
	lightnode3->setDirection(0, -1, 1);

	Ogre::SceneNode* spherenode3 = scnMgr->createSceneNode("Node31");
	scnMgr->getRootSceneNode()->addChild(spherenode3);
	spherenode3->setPosition(40, 100, -75);

	Ogre::SceneNode* penguinnode = scnMgr->getRootSceneNode()->createChildSceneNode();
	penguinnode->setScale(1, 1.33, 0.5);
	penguinnode->setPosition(0, 20, 0);



	// Lights

	Ogre::Light* light1 = scnMgr->createLight("Light1");
	light1->setType(Ogre::Light::LT_SPOTLIGHT);
	light1->setSpotlightInnerAngle(Ogre::Degree(15.0f));
	light1->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	light1->setSpotlightFalloff(0.3f);
	light1->setDiffuseColour(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
	lightnode->attachObject(light1);

	Ogre::Light* light2 = scnMgr->createLight("Light2");
	light2->setType(Ogre::Light::LT_SPOTLIGHT);
	light2->setSpotlightInnerAngle(Ogre::Degree(15.0f));
	light2->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	light2->setSpotlightFalloff(0.3f);
	light2->setDiffuseColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f));
	lightnode2->attachObject(light2);

	Ogre::Light* light3 = scnMgr->createLight("Light3");
	light3->setType(Ogre::Light::LT_SPOTLIGHT);
	light3->setSpotlightInnerAngle(Ogre::Degree(15.0f));
	light3->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	light3->setSpotlightFalloff(0.3f);
	light3->setDiffuseColour(Ogre::ColourValue(0.0f, 0.0f, 1.0f));
	lightnode3->attachObject(light3);

	// Light Entities - No shadows
	
	Ogre::Entity* LightEnt = scnMgr->createEntity("MyEntity", "sphere.mesh");
	spherenode->setScale(0.1f, 0.1f, 0.1f);
	spherenode->attachObject(LightEnt);
	LightEnt->setCastShadows(false);    // important for lightentities

	Ogre::Entity* LightEnt2 = scnMgr->createEntity("MyEntity2", "sphere.mesh");
	spherenode2->setScale(0.1f, 0.1f, 0.1f);
	spherenode2->attachObject(LightEnt2);
	LightEnt2->setCastShadows(false);

	Ogre::Entity* LightEnt3 = scnMgr->createEntity("MyEntity3", "sphere.mesh");
	spherenode3->setScale(0.1f, 0.1f, 0.1f);
	spherenode3->attachObject(LightEnt3);
	LightEnt3->setCastShadows(false);

	/* Learnt the very obvious thing that meshes that build lightentities must have setCastShadows(false) */

	// Pinguino

	Ogre::Entity* penguinentity = scnMgr->createEntity("penguin.mesh");
	penguinentity->setCastShadows(false);
	penguinnode->attachObject(penguinentity);



	//3


	/* 
	*/

	/*
	*/






}

bool TutorialApplication::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
		getRoot()->queueEndRendering();
	}
	return true;
}

int main(int argc, char** argv)
{
	try {
		TutorialApplication app;
		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
	}
	catch (const std::exception& e) {
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		return 1;
	}

	return 0;
}