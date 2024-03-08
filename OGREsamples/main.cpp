#include <iostream>
#include <exception>

#include "Ogre.h"
#include "SdkSample.h"
#include "Sample.h"
#include "SampleContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "SinbadCharacterController.h"

// Terrain

#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

// Define Terrain Paging

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

// Define Terrain size

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513

#define SHADOWS_IN_LOW_LOD_MATERIAL false

using namespace Ogre;
using namespace OgreBites;



// Currently we have interference with the keyinputs when using BaseApplication::frameStarted() override
// so I will try to do as I made work before by creating a framelistener class
// Works very well now

class MyFrameListener : public FrameListener, public InputListener {

public:

    MyFrameListener(SinbadCharacterController* inCharacter) : _Character(inCharacter) {
       
    }

    virtual ~MyFrameListener() {
    
    }

    bool frameStarted(const FrameEvent& evt) override {
        _Character->addTime(evt.timeSinceLastFrame);
        return true;
    }

private:
    SinbadCharacterController* _Character;

};


class BaseApplication : public ApplicationContext, public FrameListener, public InputListener

{

public:

    BaseApplication();

    virtual ~BaseApplication() {
    
    }

    void createFramelistener();
    void setup() override;
    bool keyPressed(const KeyboardEvent& evt) override;
    bool keyReleased(const KeyboardEvent& evt) override;
    bool mouseMoved(const MouseMotionEvent& evt) override;
    bool mouseWheelRolled(const MouseWheelEvent& evt) override;
    bool mousePressed(const MouseButtonEvent& evt) override;

private:

    SceneManager* _ScnMgr;
    Root* _Root;
    Camera* _Cam;
    SinbadCharacterController* _Character;
    MyFrameListener* _FrameListener;

};


BaseApplication::BaseApplication() : ApplicationContext ("OGREsamples"), _Character(nullptr) {
    _ScnMgr = nullptr;
    _Root = nullptr;
    _Cam = nullptr;
    _Character = nullptr;
    _FrameListener = nullptr;
}

void BaseApplication::setup() {

    // do not forget to call the base first

    ApplicationContext::setup();
    addInputListener(this);

    _Root = getRoot();
    _ScnMgr = _Root->createSceneManager();

    // register our scene with the RTSS

    RTShader::ShaderGenerator* _Shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    _Shadergen->addSceneManager(_ScnMgr);

    OverlaySystem* _OverlaySystem = ApplicationContext::getOverlaySystem();
    _ScnMgr->addRenderQueueListener(_OverlaySystem);

    // [camera]

    SceneNode* _CamNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();

    _Cam = _ScnMgr->createCamera("myCam");
    //_CamNode->lookAt(Vector3(1963, 50, 1660), Node::TS_PARENT);

    // This will be setup by the SinbadCharacterController setup
    //_Cam->setNearClipDistance(40); // tight near plane important for shadows
    //_Cam->setFarClipDistance(50000);

    //_Cam->setFarClipDistance(0); // enable infinite far clip distance

    // Set viewport

    Viewport* _Vp = getRenderWindow()->addViewport(_Cam);
    _Vp->setMaterialScheme(MSN_SHADERGEN);
    _Vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));


    // Set aspect ratio and attach camera to node

    _Cam->setAspectRatio(Real(_Vp->getActualWidth()) / Real(_Vp->getActualHeight()));
    _CamNode->attachObject(_Cam);

    _Character = new SinbadCharacterController(_Cam);

    // Make a framelistener for the character

    createFramelistener();

    //_Character->addTime(0.5);

    // Set shadows

    _ScnMgr->setAmbientLight(ColourValue(0, 0, 0));
    _ScnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_NONE);

    // Lights

    Light* _LightDir = _ScnMgr->createLight();
    _LightDir->setType(Light::LT_DIRECTIONAL);
    _LightDir->setDiffuseColour(ColourValue::White);
    _LightDir->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

    SceneNode* _LightDirNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();
    _LightDirNode->setDirection(Vector3(0.55, -0.3, 0.75).normalisedCopy());
    _LightDirNode->attachObject(_LightDir);

    // Make sky

    //_ScnMgr->setSkyBox(true, "Examples/EveningSkyBox");
}

/*
bool BaseApplication::frameStarted(const FrameEvent& evt) {
    // Update Sinbad's body
    //_Character->addTime(evt.timeSinceLastFrame);

    return true; // Return true to continue rendering
}
*/

void BaseApplication::createFramelistener() {
    _FrameListener = new MyFrameListener(_Character);
    getRoot()->addFrameListener(_FrameListener);
}

bool BaseApplication::keyPressed(const KeyboardEvent& evt) {

    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        delete _Character;
        _Character = nullptr;
        getRoot()->queueEndRendering();
    }

    if (_Character) {
        _Character->injectKeyDown(evt);
    }

    return true;
}

bool BaseApplication::keyReleased(const KeyboardEvent& evt) {

    if (_Character) {
        _Character->injectKeyUp(evt);
    }

    return true;
}

bool BaseApplication::mouseMoved(const MouseMotionEvent& evt) {

    if (_Character) {
        _Character->injectMouseMove(evt);
    }


    return true;
}

bool BaseApplication::mouseWheelRolled(const MouseWheelEvent& evt) {

    if (_Character) {
        _Character->injectMouseWheel(evt);
    }

    return true;

}

bool BaseApplication::mousePressed(const MouseButtonEvent& evt) {

    if (_Character) {
        _Character->injectMouseDown(evt);
    }

    return true;
}


int main(int argc, char** argv)
{
    try
    {
        BaseApplication app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}