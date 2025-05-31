#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreFrameListener.h>
#include <OgreCameraMan.h>
#include <iostream>

using namespace Ogre;
using namespace OgreBites;

class LavaLampApp : public ApplicationContext, public InputListener
{
public:
    LavaLampApp() : ApplicationContext("LavaLampExample") {}

    void setup() override
    {
        ApplicationContext::setup();
        addInputListener(this);

        SceneManager* scnMgr = getRoot()->createSceneManager();

        Ogre::RTShader::ShaderGenerator::getSingleton().addSceneManager(scnMgr);

        // Register the FrameListener
        getRoot()->addFrameListener(this);

        // Lighting
        Light* light = scnMgr->createLight("MainLight");
        SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->attachObject(light);
        lightNode->setPosition(20, 80, 50);

        // Camera
        Camera* cam = scnMgr->createCamera("MainCam");
        cam->setNearClipDistance(5);
        cam->setAutoAspectRatio(true);

        SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        camNode->attachObject(cam);
        camNode->setPosition(0, 10, 100);
        getRenderWindow()->addViewport(cam);

        // Bubble entity
        bubbleEntity = scnMgr->createEntity("ogrehead.mesh");
        bubbleEntity->setMaterialName("Examples/Chrome");
        bubbleNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        bubbleNode->attachObject(bubbleEntity);
        bubbleNode->setScale(0.5f, 0.5f, 0.5f);
        bubbleNode->setPosition(0, minY, 0);
    }

    // FrameListener method
    bool frameRenderingQueued(const FrameEvent& evt) override
    {
        float deltaTime = evt.timeSinceLastFrame;

        // Move the bubble upward
        Vector3 pos = bubbleNode->getPosition();
        pos.y += speed * deltaTime;

        if (pos.y > maxY)
            pos.y = minY;

        bubbleNode->setPosition(pos);
        return true;
    }

private:
    SceneNode* bubbleNode = nullptr;
    Entity* bubbleEntity = nullptr;
    float speed = 2.0f;       // units per second
    float minY = 0.0f;
    float maxY = 20.0f;
};

int main(int argc, char** argv)
{
    try {
        LavaLampApp app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
