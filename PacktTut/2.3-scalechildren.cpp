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
	scnMgr->getRootSceneNode()->addChild(node);
	node->attachObject(ent);

	//2
	Ogre::Entity* ent2 = scnMgr->createEntity("MyEntity2", "Sinbad.mesh");

	// create a childscenenode to node named node2
	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	node2->setPosition(10, 0, 0);
	node2->scale(2.0f, 2.0f, 2.0f);
	node2->attachObject(ent2);

	//3
	Ogre::Entity* ent3 = scnMgr->createEntity("MyEntity3", "Sinbad.mesh");

	// create a childscenenode to node named node 3 and position it with Ogre::Vector3
	Ogre::SceneNode* node3 = node->createChildSceneNode("node3", Ogre::Vector3(20, 0, 0));
	node3->scale(0.2f, 0.2f, 0.2f);
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