#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"

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

	// register our scene with the RTSS
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(scnMgr);

	// camera creation

	Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Camera* cam = scnMgr->createCamera("myCam");

	camNode->setPosition(50, 10, 60);
	camNode->lookAt(Ogre::Vector3(40, 10, 0), Ogre::Node::TransformSpace::TS_WORLD);

	cam->setNearClipDistance(5);
	camNode->attachObject(cam);


	// create a viewport

	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// set aspectratio

	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// set ambiance and shadow technique

	scnMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	// creating entities

	//1
	Ogre::Entity* ent = scnMgr->createEntity("MyEntity", "Sinbad.mesh");
	Ogre::SceneNode* node = scnMgr->createSceneNode("Node1");
	node->setPosition(40, 10, 0);
	node->yaw(Ogre::Degree(180.0f));
	scnMgr->getRootSceneNode()->addChild(node);
	node->attachObject(ent);

	//2
	Ogre::Entity* ent2 = scnMgr->createEntity("MyEntitysNinja", "ninja.mesh");
	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	node2->setPosition(10, 0, 0);
	node2->setScale(0.02f, 0.02f, 0.02f);
	node2->attachObject(ent2);

	//3
	Ogre::Entity* ent3 = scnMgr->createEntity("MyEntitysNinja2", "ninja.mesh");
	Ogre::SceneNode* node3 = scnMgr->createSceneNode("Node3");
	node2->addChild(node3);
	node3->setScale(3.0f, 3.0f, 3.0f);
	node3->translate(Ogre::Vector3(800, 0, 0));
	node3->attachObject(ent3);






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