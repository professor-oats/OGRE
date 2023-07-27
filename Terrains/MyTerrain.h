#ifndef __Terrain_App_h_
#define __Terrain_App_h_

#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

#include <iostream>
#include <exception>

#include "Ogre.h"
#include "OgreTrays.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "SdkSample.h"
#include "OgreOverlaySystem.h"
#include "OgreWindowEventUtilities.h"
#include "OgreConfigFile.h"


#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0
#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513

class TutorialApplication : public OgreBites::ApplicationContext, public OgreBites::SdkSample, public OgreBites::InputListener,
    public Ogre::FrameListener
 {
public:
    TutorialApplication();
    virtual ~TutorialApplication() {}

    void setup();
    bool keyPressed(const OgreBites::KeyboardEvent& evt);

    // add the needed functionalities for the terrain app
protected:
    // here follows the declaration on what functions to use from headers

    void configureTerrainDefaults(Ogre::Light* light);
    void defineTerrain(long x, long y);
    void generateFilename(long x, long y);
    void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);
    void initBlendMaps(Ogre::Terrain* terrain);
    void setFilenameConvention(const Ogre::String& prefix, const Ogre::String& extension);
    void createFrameListener();
    //    bool frameRenderingQueued(const FrameEvent& evt);

private:
    bool mTerrainsImported;
    Ogre::TerrainGroup* mTerrainGroup;
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    OgreBites::Label* mInfoLabel = nullptr;
    Ogre::Vector3 mTerrainPos;
    Ogre::SceneManager* scnMgr;
    OgreBites::TrayManager* mTrayMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;


    //    OgreBites::Label* mInfoLabel;

    //    SceneManager* mSceneMgr;
    //    Camera* mCamera;
};

/*
class KeyHandler : public OgreBites::InputListener
{
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override
    {
        if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
        {
            Ogre::Root::getSingleton().queueEndRendering();
        }
        return true;
    }
};
*/

#endif