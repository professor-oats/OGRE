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

    MyFrameListener(Entity* inEntity, SceneNode* inNode, RenderWindow* inWindow, std::deque<Vector3> inList) : _RobEntity(inEntity), _RobNode(inNode), _Window(inWindow), _WalkList(inList) {
        WindowEventUtilities::addWindowEventListener(_Window, this);
        _AnimationState = _RobEntity->getAnimationState("Idle");
        _AnimationState->setLoop(true);
        _AnimationState->setEnabled(true);
        _Distance = 0;
        _WalkSpd = 70.0;
        _Direction = Vector3::ZERO;
        _Destination = Vector3::ZERO;
    }

    virtual ~MyFrameListener() {
        _AnimationState = nullptr;
        WindowEventUtilities::removeWindowEventListener(_Window, this);
        windowClosed(_Window);
    }

    bool frameStarted(const FrameEvent& evt) override {

        if (_Direction == Ogre::Vector3::ZERO)
        {
            if (nextLocation())
            {
                _AnimationState = _RobEntity->getAnimationState("Walk");
                _AnimationState->setLoop(true);
                _AnimationState->setEnabled(true);
            }
        }

        _AnimationState->addTime(evt.timeSinceLastFrame);

        return true;
    }

    bool frameEnded(const FrameEvent& evt) override {
        return true;
    }

    bool nextLocation() {
        if (_WalkList.empty()) {
            return false;
        }

        // Get the new location from the Walklist front, then pop
        _Destination = _WalkList.front();
        _WalkList.pop_front();
        _Direction = _Destination - _RobNode->getPosition();
        _Distance = _Direction.normalise();

        return true;
    }

private:
    Entity* _RobEntity;
    SceneNode* _RobNode;
    RenderWindow* _Window;
    AnimationState* _AnimationState;
    Real _Distance;
    Real _WalkSpd;
    Vector3 _Direction;
    Vector3 _Destination;
    std::deque<Vector3> _WalkList;
};


class BaseApplication : public ApplicationContext, public FrameListener, public InputListener

{

public:

    BaseApplication();

    virtual ~BaseApplication() {
        _TrayManager = nullptr;
        delete _TrayManager;
        _RobEntity = nullptr;
        _RobNode = nullptr;
    }

    void createFramelistener();
    void setup() override;
    bool keyPressed(const KeyboardEvent& evt) override;
    /*
    bool keyReleased(const KeyboardEvent& evt) override;
    bool mouseMoved(const MouseMotionEvent& evt) override;
    bool mouseWheelRolled(const MouseWheelEvent& evt) override;
    bool mousePressed(const MouseButtonEvent& evt) override;
    */

private:

    SceneManager* _ScnMgr;
    Root* _Root;
    Camera* _Cam;
    MyFrameListener* _FrameListener;
    SinbadCharacterController* _Character;
    TrayManager* _TrayManager;
    ParamsPanel* _ParamsPanel;
    RenderWindow* _Window;
    Entity* _RobEntity;
    SceneNode* _RobNode;
    std::deque<Vector3> _WalkList;
};


BaseApplication::BaseApplication() : ApplicationContext("OGREsamples") {
    _ScnMgr = nullptr;
    _Root = nullptr;
    _Cam = nullptr;
    _Character = nullptr;
    _FrameListener = nullptr;
    _TrayManager = nullptr;
    _ParamsPanel = nullptr;
    _RobEntity = nullptr;
    _RobNode = nullptr;
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

    // [scene settings]

    _ScnMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    // [camera]

    SceneNode* _CamNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();

    _Cam = _ScnMgr->createCamera("myCam");
 
    // Set viewport

    Viewport* _Vp = _Window->addViewport(_Cam);
    _Vp->setMaterialScheme(MSN_SHADERGEN);
    //_Vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));


    // Set aspect ratio and attach camera to node

    _Cam->setAspectRatio(Real(_Vp->getActualWidth()) / Real(_Vp->getActualHeight()));
    _CamNode->attachObject(_Cam);


    // Fix the camera settings later on

    _CamNode->setPosition(40.0, 280.0, 535.0);
    _CamNode->pitch(Ogre::Degree(-30.0));
    _CamNode->yaw(Ogre::Degree(-15.0));

    // [entities]

    // Robot

    _RobEntity = _ScnMgr->createEntity("robot.mesh");
    _RobNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 0, 25.0));
    _RobNode->attachObject(_RobEntity);

    // Objects

    Entity* _ObjEntity;
    SceneNode* _ObjNode;

    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, -10.0, 25.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);

    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(550.0, -10.0, 50.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);

    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(-100.0, -10.0, -200.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);

    // [walklist vectors]

    _WalkList.push_back(Vector3(550.0, 0, 50.0));
    _WalkList.push_back(Vector3(-100.0, 0, -200.0));
    _WalkList.push_back(Vector3(0, 0, 25.0));

    // Construct the framelistener

    createFramelistener();

    // TrayManager

    _TrayManager = new OgreBites::TrayManager("InterfaceName", _Window);
    addInputListener(_TrayManager);
    //_TrayManager->showFrameStats(TL_TOPLEFT);

}


void BaseApplication::createFramelistener() {
    _FrameListener = new MyFrameListener(_RobEntity, _RobNode, _Window, _WalkList);
    _Root->addFrameListener(_FrameListener);
}

bool BaseApplication::keyPressed(const KeyboardEvent& evt) {

    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        // We may need to set nullptrs here for the constructed pointers
        //_Character = nullptr;
        getRoot()->queueEndRendering();
    }
    /*
    if (_Character) {
        _Character->injectKeyDown(evt);
    }
    */

    return true;
}

/**
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
*/


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