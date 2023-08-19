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

	camNode->setPosition(-10, 60, 430);
	camNode->lookAt(Ogre::Vector3(0, 0, 400), Ogre::Node::TransformSpace::TS_WORLD);

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
	node->setPosition(0, 0, 400);
	node->yaw(Ogre::Degree(180.0f));
	scnMgr->getRootSceneNode()->addChild(node);
	node->attachObject(ent);

	//2
	Ogre::Entity* ent2 = scnMgr->createEntity("MyEntitysNinja", "Sinbad.mesh");
	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	node2->setPosition(10, 0, 0);
	node2->translate(0, 0, 10);
	node2->attachObject(ent2);

	//3
	Ogre::Entity* ent3 = scnMgr->createEntity("MyEntity3", "Sinbad.mesh");
	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	node3->setPosition(20, 0, 0);
	node3->translate(0, 0, 10, Ogre::Node::TS_WORLD);
	node3->attachObject(ent3);

	/* Translate takes rotations into consideration since translate itself offsets from the
	     the parent node, (unless told otherwise) if the parent node is rotated it will offset accordingly
	*/

	/*
	When we call the translate() function, the cube is moved in parent space if the space to use 
	is not defined, like we did in step 5. When no parent of the cube is rotated, translate() 
	behaves the same way with world space as it would when used with parent or local space. 
	This is true because only the position of the null point changes and not the orientation of the axes.
	When we, say, move the cube (0,0,10) units, it doesn't matter where the null point is as long as 
	the axes of the coordination system are orientated the same, it won't change the outcome of the
	translate process. However, when a parent is rotated, this is no longer true. 
	When the parent is rotated, this also rotates the axis of the null point, which changes the 
	meaning of translate(0,0,10).
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