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

	camNode->setPosition(10, 10, 100);
	camNode->lookAt(Ogre::Vector3(10, 10, 0), Ogre::Node::TransformSpace::TS_WORLD);

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
	node->setPosition(10, 10, 0);

	// make new node to child of root
	scnMgr->getRootSceneNode()->addChild(node);
	node->attachObject(ent);

	//2
	Ogre::Entity* ent2 = scnMgr->createEntity("MyEntity2", "Sinbad.mesh");
	Ogre::SceneNode* node2 = scnMgr->createSceneNode("Node2");

	//make node2 to child of node
	node->addChild(node2);

	node2->setPosition(10, 0, 0);
	node2->pitch(Ogre::Radian(Ogre::Math::HALF_PI));  // rotate thumb to x-axis
	node2->attachObject(ent2);

	//create a node3 child to node
	Ogre::Entity* ent3 = scnMgr->createEntity("MyEntity3", "Sinbad.mesh");
	Ogre::SceneNode* node3 = scnMgr->createSceneNode("Node3");
	node->addChild(node3);
	node3->translate(20, 0, 0);
	node3->yaw(Ogre::Degree(90.0f));  // rotate thumb to y-axis
	node3->attachObject(ent3);

	//node 4 child to node
	Ogre::Entity* ent4 = scnMgr->createEntity("MyEntity4", "Sinbad.mesh");
	Ogre::SceneNode* node4 = scnMgr->createSceneNode("Node4");
	node->addChild(node4);
	node4->setPosition(30, 0, 0);
	node4->roll(Ogre::Radian(Ogre::Math::HALF_PI));  // rotate thumb to z-axis
	node4->attachObject(ent4);






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