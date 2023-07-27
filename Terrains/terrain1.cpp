#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"

#include "MyTerrain.h"

TutorialApplication::TutorialApplication()
    : OgreBites::ApplicationContext("OgreTutorialApp"),
      OgreBites::SdkSample(),
      OgreBites::InputListener(),
      Ogre::FrameListener(),
      mTerrainsImported(false)
{
}


void TutorialApplication::setup() {
#ifdef _DEBUG
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    // setup resources
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    cf.getSettingsBySection();

    // important to call the base first
    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    // get a pointer to the already created root
    Ogre::Root* root = getRoot();
    scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    //initiate overlaysystem
    Ogre::OverlaySystem* pOverlaySystem = OgreBites::ApplicationContext::getOverlaySystem();
    scnMgr->addRenderQueueListener(pOverlaySystem);

    //create camera
    Ogre::SceneNode* mCameraNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Camera* mCamera = scnMgr->createCamera("MyCamera");

    // setup camera

    mCameraNode->setPosition(mTerrainPos + Ogre::Vector3(1683, 300, 1116));
    mCameraNode->lookAt(Ogre::Vector3(1963, 300, 1660), Ogre::Node::TS_PARENT);
    mCamera->setNearClipDistance(40);  // tight near plane important for shadows
    mCamera->setFarClipDistance(50000);

    // setup viewport
    Ogre::Viewport* vp = getRenderWindow()->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // set aspectratio

    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    // attach camera
    mCameraNode->attachObject(mCamera);

    // set shadows
    scnMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
    scnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_NONE);

    // set overlays
    Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "myNewPanel");

    // setup sample window
    _setupTrays(getRenderWindow());

    // Create any resource listeners (for loading screens)
    //createResourceListener();

    // load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

    // setup light
    Ogre::Light* lglobal = scnMgr->createLight();
    lglobal->setType(Ogre::Light::LT_DIRECTIONAL);
    lglobal->setDiffuseColour(Ogre::ColourValue::White);
    lglobal->setSpecularColour(Ogre::ColourValue(1, 1, 1));

    Ogre::SceneNode* lglobalNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lglobalNode->setDirection(Ogre::Vector3(0.55, -0.3, 0.75).normalisedCopy());  //make into e.vector
    lglobalNode->attachObject(lglobal);

    // making fog

    Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    vp->setBackgroundColour(fadeColour);
    scnMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0, 2000, 10000);

    // make terrain base

    mTerrainGlobals = new Ogre::TerrainGlobalOptions();
    mTerrainGroup = new Ogre::TerrainGroup(scnMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
    mTerrainGroup->setFilenameConvention(Ogre::TERRAIN_FILE_PREFIX, Ogre::TERRAIN_FILE_SUFFIX);
    mTerrainGroup->setOrigin(mTerrainPos);
    mTerrainGroup->setResourceGroup("General");

    configureTerrainDefaults(lglobal);

    // define our terrain

    for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x) {
        for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y) {
            defineTerrain(x, y);
        }
    }
    mTerrainGroup->loadAllTerrains(true);

    if (mTerrainsImported) {
        for (const auto& ti : mTerrainGroup->getTerrainSlots()) {
            initBlendMaps(ti.second->instance);
        }
    }
    mTerrainGroup->freeTemporaryResources();


    // making of skies

    //scnMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
    //scnMgr->setSkyBox(true, "Examples/CloudyNoonSkyBok", 1000, false);    // render 1000 units away from camera, set false in render first
    //scnMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

    Ogre::Plane plane;
    plane.d = 1000;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;

    scnMgr->setSkyPlane(true, plane, "Examples/SpaceSkyPlane", 1500, 50, true, 1.5, 150, 150);

}

void TutorialApplication::configureTerrainDefaults(Ogre::Light* lglobal) {

    // set details of low level in 
    mTerrainGlobals->setMaxPixelError(8);   // how well mapping fits to the mesh lower amount gives better detail less performance due to more vertices
    mTerrainGlobals->setCompositeMapDistance(3000);    // distance on how many pixels ahead we will give high level rendering with light applied

    // set light to render on terrain - lglobal will be parent SceneNode to terrain to define the properties
    mTerrainGlobals->setLightMapDirection(lglobal->getDerivedDirection());    // set the terrainlight to get the direction of lglobal vector (proper reflection)
    mTerrainGlobals->setCompositeMapAmbient(scnMgr->getAmbientLight());    // enable ambient lightning for terrain
    mTerrainGlobals->setCompositeMapAmbient(lglobal->getDiffuseColour());   // set terrain ambient color to be decided by lglobal

    // so diffuse color is color on materials and specular is the nonmaterial color for light? ok boomer

    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();  // use defaultImportSettings() when wanting to import terrain
    defaultimp.inputScale = 600;    // decides scale of the imported terrain
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    /* We are using a somewhat large scale because our heightmap image is a 8bit grayscale bitmap,
        that is normalised to the [0; 1] range on loading. You can use floating point raw heightmaps to avoid
        applying any input scaling and gain a higher precision, but such images require more storage
        and are not supported by common image viewers.
    */

    // define the import of terrain

    defaultimp.layerList.resize(3);    // decide how many terrain parts to import

    // push elements to layerList[], worldsize defines how big the load is - smaller is higher resolution

    defaultimp.layerList[0].worldSize = 200;
    defaultimp.layerList[0].textureNames.push_back("Ground37_diffspec.dds");
    defaultimp.layerList[0].textureNames.push_back("Ground37_normheight.dds");
    defaultimp.layerList[1].worldSize = 200;
    defaultimp.layerList[1].textureNames.push_back("Ground23_spec.png");    // loaded from memory
    defaultimp.layerList[1].textureNames.push_back("Ground23_normheight.dds");
    defaultimp.layerList[2].worldSize = 400;
    defaultimp.layerList[2].textureNames.push_back("Rock20_diffspec.dds");
    defaultimp.layerList[2].textureNames.push_back("Rock20_normheight.dds");

    /* The default material generator requires two textures maps per layer:
        one containing diffuse + specular data and
        another containing normal + displacement data.
    */

    /* It is recommended that you pre-merge your textures accordingly e.g. using ImageMagick.
        This way you save storage space and speed up loading. However if you want more flexibility,
        you can also make Ogre combine the images at loading accordingly as shown below
    */

    Ogre::Image combined;
    combined.loadTwoImagesAsRGBA("Ground23_col.jpg", "Ground23_spec.png", "General");
    Ogre::TextureManager::getSingleton().loadImage("Ground23_diffspec", "General", combined);

}


/* Now we will tackle our defineTerrain method. The first thing we do is ask the TerrainGroup
    to define a unique filename for this Terrain. If it has already been generated,
    then we can call TerrainGroup::defineTerrain method to set up this grid location with the previously
    generated filename automatically. If it has not been generated, then we generate an image with
    getTerrainImage and then call a different overload of TerrainGroup::defineTerrain that takes a
    reference to our generated image. Finally, we set the mTerrainsImported flag to true.
*/

void TutorialApplication::defineTerrain(long x, long y) {
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

void TutorialApplication::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img) {

    img.load("terrain.png", mTerrainGroup->getResourceGroup());
    if (flipX) {
        img.flipAroundY();
    }
    if (flipY) {
        img.flipAroundX();
    }
}

void TutorialApplication::initBlendMaps(Ogre::Terrain* terrain) {
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    float minHeight0 = 20;
    float fadeDist0 = 15;
    float minHeight1 = 70;
    float fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;

            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            float height = terrain->getHeightAtTerrainPosition(tx, ty);

            *pBlend0++ = Ogre::Math::saturate((height - minHeight0) / fadeDist0);
            *pBlend1++ = Ogre::Math::saturate((height - minHeight1) / fadeDist1);
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}

/*
void createFrameListener() {
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}
*/

bool TutorialApplication::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}

int main(int argc, char** argv)
{
    try
    {
        TutorialApplication app;
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