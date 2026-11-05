#pragma once

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <OgreCameraMan.h>
#include <OgreFrameListener.h>

class GameApplication
    : public OgreBites::ApplicationContext
    , public OgreBites::InputListener
{
public:
    GameApplication();
    virtual ~GameApplication();

    void setup() override;
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

private:
    Ogre::SceneManager* mrSceneManager = nullptr;
    Ogre::Camera* mrCam = nullptr;
    Ogre::SceneNode* mrCamNode = nullptr;
    Ogre::Viewport* mrViewport = nullptr;
    Ogre::SceneNode* mrPenguinNode = nullptr;
    Ogre::Entity* mrPenguinEntity = nullptr;
    Ogre::SceneNode* mrGroundNode = nullptr;
    Ogre::Entity* mrGroundEntity = nullptr;
    Ogre::Light* mrSpotLight = nullptr;
    Ogre::SceneNode* mrSpotLightNode = nullptr;
};
