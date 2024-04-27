#include <iostream>
#include <exception>

// OGRE

#include "Ogre.h"
#include "OgreInput.h"
#include "OgreApplicationContext.h"
#include "OgreRenderTarget.h"
#include "OgreMath.h"
#include "OgreFrameListener.h"
#include "OgreInput.h"
#include "OgreWindowEventUtilities.h"

// Collision and physics
#include "OgreBullet.h"

using namespace Ogre;
using namespace OgreBites;



// Will we move the keypress handling into the frameListener perhaps?
// The situation kind of starts calling for it.
// Or jus have the ESCAPE in the BaseApp thanks to the getRoot and rest inside here?
// At this point we could as well make the MyFrameListener inherit from BaseApplication
// instead of passing all the pointer parameters.
// Guess this will be for the future.

class MyFrameListener : public FrameListener, public InputListener, public WindowEventListener {

public:

    MyFrameListener(SceneManager* inScnMgr, Entity* inEntity, SceneNode* inNode, RenderWindow* inWindow, std::deque<Vector3> inList) : _ScnMgr(inScnMgr), _RobEntity(inEntity), _RobNode(inNode), _Window(inWindow), _WalkList(inList) {
        WindowEventUtilities::addWindowEventListener(_Window, this);
        _AnimationState = _RobEntity->getAnimationState("Idle");
        _AnimationState->setLoop(true);
        _AnimationState->setEnabled(true);
        _Distance = 0;
        _Direction = Vector3::ZERO;
        _Destination = Vector3::ZERO;
        _Walking = false;
        _ObjNode = nullptr;
        _ObjEntity = nullptr;
    }

    virtual ~MyFrameListener() {
        _AnimationState = nullptr;
        WindowEventUtilities::removeWindowEventListener(_Window, this);
        windowClosed(_Window);
    }

    bool keyPressed(const KeyboardEvent& evt) override {

        int _key = evt.keysym.sym;

        if (_key == 'r') {
            addToWalkList();
        }

        return true;
    }

    bool frameStarted(const FrameEvent& evt) override {

        if (!_Walking)
        {
            if (nextLocation())
            {
                rotateRobNodeTowardsDirection();
                _AnimationState = _RobEntity->getAnimationState("Walk");
                _AnimationState->setLoop(true);
                _AnimationState->setEnabled(true);
                _Walking = true;
            }
        }
        else
        {
            Real move = _WalkSpd * evt.timeSinceLastFrame;
            _Distance -= move;

            if (_Distance <= 0.0)
            {
                _RobNode->setPosition(_Destination);  // Will store latest destination if walklist is empty?
                //_Direction = Vector3::ZERO;
                _Walking = false;

                if (nextLocation()) {
                    rotateRobNodeTowardsDirection();
                }

                else {
                    _AnimationState = _RobEntity->getAnimationState("Idle");
                    _AnimationState->setLoop(true);
                    _AnimationState->setEnabled(true);
                }
            }
            else
            {
                // Translate the RobNode along the vector with the move constant/REAL
                _RobNode->translate(move * _Direction);
            }
        }

        _AnimationState->addTime(evt.timeSinceLastFrame);

        return true;
    }

    bool frameEnded(const FrameEvent& evt) override {
        return true;
    }

    void rotateRobNodeTowardsDirection() {
        // First we get the Quaternion through getOrientation and multiply with UNIT_X
        // to fix the Quaternion to the RobNode vectoring on X-axis - Making them match.
        src = _RobNode->getOrientation() * Vector3::UNIT_X;
        _Walking = true;

        // Check if we need a 180 turn

        if ((1.0 + src.dotProduct(_Direction)) < 0.0001)
        {
            _RobNode->yaw(Degree(180));
        }
        else
        {
            quat = src.getRotationTo(_Direction);
            _RobNode->rotate(quat);
        }
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

    // Now ... how will we spawn the new nodes/entities?
    // I guess we will pass in the object node and entity to FrameListener
    // Looks like this class becomes more than a mere FrameListener now

    void addToWalkList() {

        // We will generate the ranges for the Vector3. X has the negative max 200
        // and the rest are symmetrics

        Real thisX = Math::Floor(Math::RangeRandom(0, 500));
        Real thisminX = Math::Floor(Math::SymmetricRandom() * Math::RangeRandom(0, 200));
        Real thisY = Math::SymmetricRandom() * Math::RangeRandom(0, 200);
        Real thisZ = Math::SymmetricRandom() * Math::RangeRandom(0, 300);

        // First I'd like a fifty fifty [-1, 1] < 0 but then I realise the distance min distance -200
        // and the max distance 500 makes a tilted relationship over to the left so I will adjust the comparison
        // Easily a 2/5 would be a comparison offset of 0.4, however I think 0.2 does better on this viewport
        // since the 0 on X-axis is not centered.

        if (Math::SymmetricRandom() < -0.8) {
            thisX = thisminX;
        }

        _ObjEntity = _ScnMgr->createEntity("knot.mesh");
        _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(thisX, thisY, thisZ));
        _ObjNode->attachObject(_ObjEntity);
        _ObjNode->setScale(0.1, 0.1, 0.1);


        _WalkList.push_back(Vector3(thisX, thisY, thisZ));

    }

private:
    SceneManager* _ScnMgr;
    Entity* _RobEntity;
    SceneNode* _RobNode;
    Entity* _ObjEntity;
    SceneNode* _ObjNode;
    RenderWindow* _Window;
    AnimationState* _AnimationState;
    Real _Distance;
    Vector3 _Direction;
    Vector3 _Destination;
    Vector3 src;
    Quaternion quat;
    std::deque<Vector3> _WalkList;
    bool _Walking;

    const Real _WalkSpd = 90;

public:


};

class BaseApplication : public ApplicationContext, public FrameListener, public InputListener

{

public:

    BaseApplication();

    virtual ~BaseApplication() {
        _RobEntity = nullptr;
        _RobNode = nullptr;
    }

    void createFramelistener();
    void setup() override;
    bool keyPressed(const KeyboardEvent& evt) override;

private:

    SceneManager* _ScnMgr;
    Root* _Root;
    Camera* _Cam;
    MyFrameListener* _FrameListener;
    RenderWindow* _Window;
    Entity* _RobEntity;
    SceneNode* _RobNode;
    Entity* _ObjEntity;
    SceneNode* _ObjNode;
    SceneNode* _RootNode;
    std::deque<Vector3> _WalkList;
};


BaseApplication::BaseApplication() : ApplicationContext("OGREsamples") {
    _ScnMgr = nullptr;
    _Root = nullptr;
    _Cam = nullptr;
    _FrameListener = nullptr;
    _RobEntity = nullptr;
    _RobNode = nullptr;
    _ObjEntity = nullptr;
    _ObjNode = nullptr;
    _RootNode = nullptr;
    _Window = nullptr;
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

    // [scene settings]

    _ScnMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    // [camera]

    SceneNode* _CamNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();

    _Cam = _ScnMgr->createCamera("myCam");

    // Set viewport

    Viewport* _Vp = _Window->addViewport(_Cam);

    // Set aspect ratio and attach camera to node

    _Cam->setAspectRatio(Real(_Vp->getActualWidth()) / Real(_Vp->getActualHeight()));
    _CamNode->attachObject(_Cam);


    // Fix the camera settings later on

    _CamNode->setPosition(40.0, 280.0, 535.0);
    _CamNode->pitch(Ogre::Degree(-30.0));
    _CamNode->yaw(Ogre::Degree(-15.0));

    // [entities]

    // Root Node

    _RootNode = _ScnMgr->getRootSceneNode();

    // Robot

    _RobEntity = _ScnMgr->createEntity("robot.mesh");
    _RobNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 0, 25.0));
    _RobNode->attachObject(_RobEntity);

    // Objects
    /*
    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(500, -10.0, 25.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);

    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 0, 25.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);

    _ObjEntity = _ScnMgr->createEntity("knot.mesh");
    _ObjNode = _ScnMgr->getRootSceneNode()->createChildSceneNode(Vector3(-100.0, -0, -200.0));
    _ObjNode->attachObject(_ObjEntity);
    _ObjNode->setScale(0.1, 0.1, 0.1);
    */
    // [walklist vectors]

    //_WalkList.push_back(Vector3(-100.0, 0, -200.0));
    //_WalkList.push_back(Vector3(500.0, -10, 25.0));
    //_WalkList.push_back(Vector3(0, 0, 25.0));

    // Construct the framelistener

    createFramelistener();

}


void BaseApplication::createFramelistener() {
    _FrameListener = new MyFrameListener(_ScnMgr, _RobEntity, _RobNode, _Window, _WalkList);
    _Root->addFrameListener(_FrameListener);
    addInputListener(_FrameListener);
}

bool BaseApplication::keyPressed(const KeyboardEvent& evt) {

    int _key = evt.keysym.sym;

    if (_key == SDLK_ESCAPE)
    {
        // We may need to set nullptrs here for the constructed pointers
        //_Character = nullptr;
        getRoot()->queueEndRendering();
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