#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"

class TutorialApplication
	: public OgreBites::ApplicationContext
	, public OgreBites::InputListener

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

	// get a pointer to the already created root
	Ogre::Root* root = getRoot();
	Ogre::SceneManager* scnMgr = root->createSceneManager();

	// register our scene with the RTSS
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scnMgr);

	// here follows the setup specifics

	// camera creation

	Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Camera* cam = scnMgr->createCamera("myCam");

	camNode->setPosition(-50, 400, 800);
	camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);

	cam->setNearClipDistance(5);
	camNode->attachObject(cam);
	
	// create a viewport

	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// set aspectratio

	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// set ambiance and shadow technique

	scnMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);


	// create ninja node and entity
	Ogre::Entity* ninjaEntity = scnMgr->createEntity("penguin.mesh");
	ninjaEntity->setCastShadows(false);

	Ogre::SceneNode* ninjaNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	ninjaNode->setScale(6, 8, 3);
	ninjaNode->attachObject(ninjaEntity);

	// create free Plane with and use with Ogre::MeshManager
	// use UNIT_Y to define the normal for the plane and 0 offset from the origin
	// we can also feed a vector as the offset thanks to overloads

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	// create a mesh named "ground" put on to the plane we made

	Ogre::MeshManager::getSingleton().createPlane(
		"ground", Ogre::RGN_DEFAULT,
		plane,
		2500, 2500, 30, 30,
		true,
		1, 5, 5,
		Ogre::Vector3::UNIT_Z);

	Ogre::Entity* groundEntity = scnMgr->createEntity("ground");
	Ogre::SceneNode* groundNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	groundNode->attachObject(groundEntity);

	groundEntity->setCastShadows(false);

	// put material to the mesh

	groundEntity->setMaterialName("Examples/TransparentTest");

	Ogre::Light* spotLight = scnMgr->createLight("SpotLight");
	Ogre::Light* spotLight2 = scnMgr->createLight("SpotLight2");

	// set diffuse and specular colour for an Ogre::Light*

	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);

	spotLight2->setDiffuseColour(1.0, 0, 0.2);
	spotLight2->setSpecularColour(1.0, 0, 0.2);

	// and to the type of light found in Ogre::Light definition
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight2->setType(Ogre::Light::LT_SPOTLIGHT);

	Ogre::SceneNode* spotLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::SceneNode* spotLightNode2 = scnMgr->getRootSceneNode()->createChildSceneNode();

	spotLightNode->attachObject(spotLight);
	spotLightNode->setDirection(-1, -1, 0);
	spotLightNode->setPosition(Ogre::Vector3(200, 200, 0));

	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));

	spotLightNode2->attachObject(spotLight2);
	spotLightNode2->setDirection(0, -1, 0);
	spotLightNode2->setPosition(Ogre::Vector3(50, 400, 0));

	spotLight2->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));

	/*
	// add directional light
	Ogre::Light* directionalLight = scnMgr->createLight("DirectionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);

	directionalLight->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
	directionalLight->setSpecularColour(Ogre::ColourValue(0, 4, 0, 0));

	Ogre::SceneNode* directionalLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	directionalLightNode->attachObject(directionalLight);
	directionalLightNode->setDirection(Ogre::Vector3(0, -1, 1));

	*/

	// add point light
	Ogre::Light* pointLight = scnMgr->createLight("PointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	
	pointLight->setDiffuseColour(0.8, 0.8, 0.8);
	pointLight->setSpecularColour(1.0, 0.5, 0);

	Ogre::SceneNode* pointLightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	pointLightNode->attachObject(pointLight);
	pointLightNode->setPosition(Ogre::Vector3(0, 150, 250));

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