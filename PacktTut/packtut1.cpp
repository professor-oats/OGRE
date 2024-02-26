#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreLight.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreBitesConfigDialog.h"
#include "OgreConfigDialog.h"
#include "OgreFrameListener.h"

/* Solution:
    Implement key inputs into the frameStarted() function. Make sure to change the constructor to
	handle inputs.
*/

class Example25FrameListener : public Ogre::FrameListener, OgreBites::InputListener

{


public:



	Example25FrameListener(Ogre::SceneNode* entityNode, Ogre::Entity* anient, Ogre::Entity* anient2)
		: mEntityNode(entityNode), _anient(anient), _anient2(anient2)
	{
		_aniState = _anient->getAnimationState("RunBase");
		_aniState->setEnabled(true);
		_aniState->setLoop(true);

		_aniState2 = _anient2->getAnimationState("Dance");
		_aniState2->setEnabled(true);
		_aniState2->setLoop(true);
	}

	Example25FrameListener() {
		_frameListener = NULL;
	}


	~Example25FrameListener() {
		if (_frameListener) {
			delete _frameListener;
		}

	}

	bool frameStarted(const Ogre::FrameEvent& evt) override {
	
		//_node->translate(Ogre::Vector3(0.1, 0, 0));

		//_node->translate(Ogre::Vector3(10, 0, 0) * evt.timeSinceLastFrame);

		// Apply translation to the entity node
		mEntityNode->translate(mEntityTranslation * evt.timeSinceLastFrame);
		_aniState->addTime(evt.timeSinceLastFrame);
		_aniState2->addTime(evt.timeSinceLastFrame);

		// Apply translation to the camera node
		//mCameraNode->translate(mCameraTranslation * evt.timeSinceLastFrame);

		return true;
	}

	void setEntityTranslation(const Ogre::Vector3& translation)
	{
		mEntityTranslation = translation;
	}

	/*
	void setCameraTranslation(const Ogre::Vector3& translation)
	{
		mCameraTranslation = translation;
	}
	*/

	Ogre::FrameListener* _frameListener;
	

private:
	Ogre::SceneNode* mEntityNode;
	Ogre::Vector3 mEntityTranslation = Ogre::Vector3::ZERO;
	Ogre::Entity* _anient;
	Ogre::Entity* _anient2;
	Ogre::AnimationState* _aniState;
	Ogre::AnimationState* _aniState2;

	//Ogre::SceneNode* mCameraNode;
	//Ogre::Vector3 mCameraTranslation = Ogre::Vector3::ZERO;


};

class TutorialApplication
	: public OgreBites::ApplicationContext,
	public OgreBites::InputListener,
	public Ogre::FrameListener,
	public Example25FrameListener

{
public:
	TutorialApplication();
	virtual ~TutorialApplication();

	void setup();
	void createFramelistener();
	bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
	bool mouseWheel(const OgreBites::MouseWheelEvent& evt);

private:
	Ogre::SceneNode* _SinbadNode;
	Ogre::SceneNode* _SinbadNode2;
	Example25FrameListener* _frameListener;
	Ogre::SceneNode* camNode;
	Ogre::Entity* _SinbadEnt;
	Ogre::Entity* _SinbadEnt2;
	bool keyPressed(const OgreBites::KeyboardEvent& evt);
};

TutorialApplication::TutorialApplication()
	: OgreBites::ApplicationContext("OgreTutorialApp")

{
	_SinbadNode = NULL;
	_SinbadNode2 = NULL;
	_frameListener = NULL;
	camNode = NULL;
	_SinbadEnt = NULL;
	_SinbadEnt2 = NULL;
}

TutorialApplication::~TutorialApplication() 

{
	delete _SinbadNode;
	delete _SinbadNode2;
	delete camNode;
	delete _SinbadEnt;
	delete _SinbadEnt2;

	delete _frameListener;
}

void TutorialApplication::createFramelistener() {
	_frameListener = new Example25FrameListener(_SinbadNode, _SinbadEnt, _SinbadEnt2);
	getRoot()->addFrameListener(_frameListener);
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

	camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Camera* cam = scnMgr->createCamera("myCam");

	camNode->setPosition(0, 100, 200);
	camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);

	cam->setNearClipDistance(5);
	camNode->attachObject(cam);

	//cam->setPolygonMode(Ogre::PM_POINTS);


	// create a viewport

	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// set aspectratio

	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// set ambiance and shadow technique

	scnMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

	// nada movements
	//mytranslate = Ogre::Vector3(0, 0, 0);

	// making a plane
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane
	(
		"plane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1500, 1500, 200, 200, true, 1, 5, 5, Ogre::Vector3::UNIT_Z
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

	// Nodes

	Ogre::SceneNode* directionallightnode = scnMgr->createSceneNode("Node1");
	scnMgr->getRootSceneNode()->addChild(directionallightnode);
	directionallightnode->setDirection(Ogre::Vector3(1, -1, 0));

	/* Get the _SinbadNode that we defined in the class */

	Ogre::SceneNode* node = scnMgr->createSceneNode("Node2");
	scnMgr->getRootSceneNode()->addChild(node);

	_SinbadNode = node->createChildSceneNode("SinbadNode");
	_SinbadNode->setScale(3.0f, 3.0f, 3.0f);
	_SinbadNode->setPosition(Ogre::Vector3(0.0f, 4.0f, 0.0f));
	_SinbadNode2 = _SinbadNode->createChildSceneNode("SinbadNode2");
	_SinbadNode2->setPosition(Ogre::Vector3(-8.0f, 0.0f, 0.0f));



	// Lights

	Ogre::Light* light = scnMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	directionallightnode->attachObject(light);
	



	// Entities

	_SinbadEnt = scnMgr->createEntity("Sinbad", "Sinbad.mesh");
	_SinbadNode->attachObject(_SinbadEnt);
	_SinbadEnt2 = scnMgr->createEntity("Sinbad2", "Sinbad.mesh");
	_SinbadNode2->attachObject(_SinbadEnt2);


	/* 
	*/

	/*
	*/

	// FrameListener

	createFramelistener();
	//Example25FrameListener::frameStarted(OgreBites::KeyboardEvent& evtf, evtk);

	// Movement
	//_SinbadNode->translate(mytranslate);


}


bool TutorialApplication::keyPressed(const OgreBites::KeyboardEvent& evt) {

	OgreBites::Keycode key = evt.keysym.sym;

	if (key == OgreBites::SDLK_ESCAPE) {
		getRoot()->queueEndRendering();
	}

	// Update translation based on key presses
	if (key == OgreBites::SDLK_UP)
	{
		_frameListener->setEntityTranslation(Ogre::Vector3(0, 0, -10)); // Move up
	}
	else if (key == OgreBites::SDLK_DOWN)
	{
		_frameListener->setEntityTranslation(Ogre::Vector3(0, 0, 10)); // Move down
	}
	else if (key == OgreBites::SDLK_LEFT)
	{
		_frameListener->setEntityTranslation(Ogre::Vector3(-10, 0, 0)); // Move left
	}
	else if (key == OgreBites::SDLK_RIGHT)
	{
		_frameListener->setEntityTranslation(Ogre::Vector3(10, 0, 0)); // Move right
	}

	return true;
}

bool TutorialApplication::mouseMoved(const OgreBites::MouseMotionEvent& evt) {

	// Calculate rotation based on mouse movement
	float rotX = evt.xrel * -0.5; // Adjust the sensitivity as needed
	float rotY = evt.yrel * -0.5;

	// Adjust camera orientation
	camNode->yaw(Ogre::Degree(rotX));
	camNode->pitch(Ogre::Degree(rotY));

	// Update camera position based on forward/backward movement (optional)
	// Adjust the forward/backward movement speed as needed
	/*
	Ogre::Vector3 cameraMovement = mCameraNode->getOrientation() * Ogre::Vector3(0, 0, -1);
	mCameraNode->setPosition(mCameraNode->getPosition() + cameraMovement * 10 * evt.timeSinceLastFrame);
	*/

	return true;
}

bool TutorialApplication::mouseWheel(const OgreBites::MouseWheelEvent& evt) {
	float zoomin = evt.y * 0.1;
	float zoomout = evt.y * -0.1;

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