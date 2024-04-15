#include <iostream>
#include <exception>

// OGRE

#include "Ogre.h"
#include "SdkSample.h"
#include "Sample.h"
#include "SampleContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "SinbadCharacterController.h"
#include "OgreTrays.h"
#include "OgreRenderTarget.h"
#include "OgreWindowEventUtilities.h"

// Collision and physics
#include "OgreBullet.h"

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

class MyFrameListener : public FrameListener, public InputListener, public WindowEventListener {

public:

    MyFrameListener(SinbadCharacterController* inCharacter, RenderWindow* inWindow) : _Character(inCharacter), _Window(inWindow) {
        WindowEventUtilities::addWindowEventListener(_Window, this);
    }

    virtual ~MyFrameListener() {
        WindowEventUtilities::removeWindowEventListener(_Window, this);
        windowClosed(_Window);
    }

    bool frameStarted(const FrameEvent& evt) override {
        _Character->addTime(evt.timeSinceLastFrame);
        return true;
    }

    bool frameEnded(const FrameEvent& evt) override {
        return true;
    }

private:
    SinbadCharacterController* _Character;
    RenderWindow* _Window;

};


class BaseApplication : public ApplicationContext, public FrameListener, public InputListener

{

public:

    BaseApplication();

    virtual ~BaseApplication() {
        _TrayManager = nullptr;
        delete _TrayManager;
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
    TrayManager* _TrayManager;
    ParamsPanel* _ParamsPanel;
    RenderWindow* _Window;
};


BaseApplication::BaseApplication() : ApplicationContext("OGREsamples"), _Character(nullptr) {
    _ScnMgr = nullptr;
    _Root = nullptr;
    _Cam = nullptr;
    _Character = nullptr;
    _FrameListener = nullptr;
    _TrayManager = nullptr;
    _ParamsPanel = nullptr;
}

void BaseApplication::setup() {

    // do not forget to call the base first

    ApplicationContext::setup();
    addInputListener(this);

    _Root = getRoot();
    _Window = getRenderWindow();
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

    Viewport* _Vp = _Window->addViewport(_Cam);
    _Vp->setMaterialScheme(MSN_SHADERGEN);
    _Vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));


    // Set aspect ratio and attach camera to node

    _Cam->setAspectRatio(Real(_Vp->getActualWidth()) / Real(_Vp->getActualHeight()));
    _CamNode->attachObject(_Cam);

    _Character = new SinbadCharacterController(_Cam);

    // Make a framelistener for the character

    createFramelistener();

    //_Character->addTime(0.5);

    // TrayManager

    _TrayManager = new OgreBites::TrayManager("InterfaceName", _Window);
    addInputListener(_TrayManager);
    _TrayManager->showFrameStats(TL_TOPLEFT);

    /* Button* b = _TrayManager->createButton(TL_TOPLEFT, "MyButton", "Click Me!"); */

    /*
    StringVector panelitems;
    panelitems.push_back("FPS");
    panelitems.push_back("Quit");
    _ParamsPanel = _TrayManager->createParamsPanel(TL_BOTTOMLEFT, "MyStats", 200, panelitems);
    _ParamsPanel->setParamValue("Quit", "ESC");
    */


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


    Light* _LightDir2 = _ScnMgr->createLight();
    _LightDir2->setType(Light::LT_DIRECTIONAL);
    _LightDir2->setDiffuseColour(ColourValue::White);
    _LightDir2->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

    SceneNode* _LightDirNode2 = _ScnMgr->getRootSceneNode()->createChildSceneNode();
    _LightDirNode2->setDirection(Vector3(-0.55, -0.3, 0.55).normalisedCopy());
    _LightDirNode2->attachObject(_LightDir2);

    Light* _LightDir3 = _ScnMgr->createLight();
    _LightDir3->setType(Light::LT_DIRECTIONAL);
    _LightDir3->setDiffuseColour(ColourValue::White);
    _LightDir3->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

    SceneNode* _LightDirNode3 = _ScnMgr->getRootSceneNode()->createChildSceneNode();
    _LightDirNode3->setDirection(Vector3(0, -0.3, -0.55).normalisedCopy());
    _LightDirNode3->attachObject(_LightDir3);

    // Make plane

    Plane plane(Vector3::UNIT_Y, 0);
    MeshManager::getSingleton().createPlane
    (
        "plane",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 200, 200, true, 20, 20, 20, Vector3::UNIT_Z
    );

    Entity* _PlaneEnt = _ScnMgr->createEntity("LightPlaneEntity", "plane");
    _ScnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(_PlaneEnt);
    _PlaneEnt->setCastShadows(false);
    _PlaneEnt->setMaterialName("Examples/Rocky");


    // Make sky

    _ScnMgr->setSkyBox(true, "Examples/EveningSkyBox");

    // Add a penguin entity

    Entity* _PengEnt = _ScnMgr->createEntity("penguin.mesh");
    _PengEnt->setCastShadows(false);

    SceneNode* _PengEntNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();
    _PengEntNode->setScale(6, 8, 3);
    _PengEntNode->attachObject(_PengEnt);
}

/*
bool BaseApplication::frameStarted(const FrameEvent& evt) {
    // Update Sinbad's body
    //_Character->addTime(evt.timeSinceLastFrame);

    return true; // Return true to continue rendering
}
*/

void BaseApplication::createFramelistener() {
    _FrameListener = new MyFrameListener(_Character, _Window);
    _Root->addFrameListener(_FrameListener);
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