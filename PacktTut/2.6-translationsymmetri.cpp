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
	node2->yaw(Ogre::Degree(-45));
	node2->translate(0, 0, 20, Ogre::Node::TS_LOCAL);
	node2->attachObject(ent2);

	//3
	Ogre::Entity* ent3 = scnMgr->createEntity("MyEntity3", "Sinbad.mesh");
	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	node3->yaw(Ogre::Degree(45));
	node3->translate(0, 0, 20, Ogre::Node::TS_LOCAL);
	node3->attachObject(ent3);

	/* Translate takes rotations into consideration since translate itself offsets from the
	     the parent node, (unless told otherwise) if the parent node is rotated it will offset accordingly
	*/

	/*
		What just happened?
		We created our reference model and then added two models which were rotated 45 degrees 
		around the y-axis. Then we translated both with (0,0,20), one model in parent space,
		the default setting, and the other model in local space. The model we translated in parent 
		pace moved in a straight line on the z-axis. But because we rotated the models around 
		the y-axis, the model we translated in local space moved with this rotation and ended up 
		moving up and left in the image. Let's repeat this. When we translate, 
		the default setting is parent space, meaning that all rotations, except the rotation of the 
		scene node we translate, are used while translating. When using world space, no rotation 
		is taken into consideration. When translating, the world coordination system is used. 
		When translating in local space, every rotation, even the rotation from the node we translate, 
		is used for the translation.
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