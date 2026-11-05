#include "GameApplication.h"

GameApplication::GameApplication()
    : OgreBites::ApplicationContext("OgreBestPracticeApp")
{
}

GameApplication::~GameApplication()
{
}

void GameApplication::setup()
{
    OgreBites::ApplicationContext::setup();
    
    addInputListener(this);
    getRoot()->addFrameListener(this);

    mrSceneManager = getRoot()->createSceneManager();

    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(mrSceneManager);

    // Camera Setup
    mrCamNode = mrSceneManager->getRootSceneNode()->createChildSceneNode();
    mrCam = mrSceneManager->createCamera("myCam");

    mrCamNode->setPosition(-50, 400, 800);
    mrCamNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
    mrCam->setNearClipDistance(5);
    mrCamNode->attachObject(mrCam);

    mrViewport = getRenderWindow()->addViewport(mrCam);
    mrViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    mrCam->setAspectRatio(Ogre::Real(mrViewport->getActualWidth()) / Ogre::Real(mrViewport->getActualHeight()));

    // Scene Environment & Lighting
    mrSceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
    mrSceneManager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

    // Creating a mesh to use that we can add to our mrGroundEntity
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", Ogre::RGN_DEFAULT, plane, 2500, 2500, 30, 30, true, 1, 5, 5, Ogre::Vector3::UNIT_Z
    );
    mrGroundEntity = mrSceneManager->createEntity("ground");
    mrGroundNode = mrSceneManager->getRootSceneNode()->createChildSceneNode();
    mrGroundNode->attachObject(mrGroundEntity);
    mrGroundEntity->setCastShadows(false);
    mrGroundEntity->setMaterialName("Examples/TransparentTest");

    // Penguin entity
    mrPenguinEntity = mrSceneManager->createEntity("penguin.mesh");
    mrPenguinEntity->setCastShadows(false);
    
    mrPenguinNode = mrSceneManager->getRootSceneNode()->createChildSceneNode();
    mrPenguinNode->setScale(6, 8, 3);
    mrPenguinNode->attachObject(mrPenguinEntity);

    // Spotlight setup
    mrSpotLight = mrSceneManager->createLight("SpotLight");
    mrSpotLight->setType(Ogre::Light::LT_SPOTLIGHT);
    mrSpotLight->setDiffuseColour(0, 0, 1.0);
    mrSpotLight->setSpecularColour(0, 0, 1.0);

    mrSpotLightNode = mrSceneManager->getRootSceneNode()->createChildSceneNode();
    mrSpotLightNode->attachObject(mrSpotLight);
    mrSpotLightNode->setDirection(-1, -1, 0);
    mrSpotLightNode->setPosition(Ogre::Vector3(200, 200, 0));
    mrSpotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
}

bool GameApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mrPenguinNode)
    {
        mrPenguinNode->yaw(Ogre::Degree(30 * evt.timeSinceLastFrame));
    }
    return true;
}

bool GameApplication::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
        getRoot()->queueEndRendering();
    }
    return true;
}
