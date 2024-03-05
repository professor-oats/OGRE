/* The app class is named BasicTutorial1 but it is my take on BasicTutorial3 from the wikis */
/* This will have to be refined to match the proper methods of included headers and also to */
/* to add trays and cameraman later on. */

/* Also there are some memory leaks that has to be fixed since we create objects without destroying them */
/* A proper shutdown of app handle should be coded. */



#include <iostream>
#include <exception>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"

// Terrain

#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

// Define Paging

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

// Access Samples

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class BasicTutorial1 : public ApplicationContext, public InputListener, 
                       public FrameListener, public SdkSample
{
public:
    BasicTutorial1();
    virtual ~BasicTutorial1() {}

    void setup();
    bool keyPressed(const KeyboardEvent& evt);

private:
    Vector3 _TerrainPos;
    TerrainGroup* _TerrainGroup;
    TerrainGlobalOptions* _TerrainGlobals;
    SceneManager* _ScnMgr;
    Label* _InfoLabel = nullptr;
    Root* _Root;
    Camera* _Cam;

    void configureTerrainDefaults(Ogre::Light* lightin);
    void defineTerrain(long x, long y);
    void getTerrainImage(bool flipX, bool flipY, Image& img);
    void initBlendMaps(Terrain* terrain);

    bool _TerrainsImported;
};


BasicTutorial1::BasicTutorial1()
    : ApplicationContext("OgreTutorialApp")
{
}


void BasicTutorial1::setup()
{
    // do not forget to call the base first

    ApplicationContext::setup();
    addInputListener(this);

    // get a pointer to the already created root

    _Root = getRoot();
    _ScnMgr = _Root->createSceneManager();

    // register our scene with the RTSS

    RTShader::ShaderGenerator* _Shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    _Shadergen->addSceneManager(_ScnMgr);

    OverlaySystem* _OverlaySystem = ApplicationContext::getOverlaySystem();
    _ScnMgr->addRenderQueueListener(_OverlaySystem);

    // Set render window to true

    //_Window = _Root->initialise(true);
 

    // [camera]

    SceneNode* _CamNode = _ScnMgr->getRootSceneNode()->createChildSceneNode();

    _Cam = _ScnMgr->createCamera("myCam");
    _CamNode->setPosition(_TerrainPos + Vector3(1683, 50, 2116));
    _CamNode->lookAt(Vector3(1963, 50, 1660), Node::TS_PARENT);
    _Cam->setNearClipDistance(40); // tight near plane important for shadows
    _Cam->setFarClipDistance(50000);

    _Cam->setFarClipDistance(0); // enable infinite far clip distance

    // Set viewport

    Viewport* _Vp = getRenderWindow()->addViewport(_Cam);  // mWindow is declared in the SdkSample.h
    _Vp->setMaterialScheme(MSN_SHADERGEN);
    //_Vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));  // YES??
    // This is for fog
    ColourValue fadeColour(0.9, 0.9, 0.9);
    _Vp->setBackgroundColour(fadeColour);
    //_ScnMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0, 2000, 10000);
    _ScnMgr->setFog(Ogre::FOG_EXP, fadeColour, 0.001);
    //_ScnMgr->setFog(Ogre::FOG_EXP2, fadeColour, 0.002);

    // Set aspect ratio and attach camera to node

    _Cam->setAspectRatio(Real(_Vp->getActualWidth()) / Real(_Vp->getActualHeight()));
    _CamNode->attachObject(_Cam);

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

    // Create sky
    // We can change the arguments of setSkyBox to change render behaviour if we need performance boosts later on
    // See the tutorial for more

    _ScnMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

    // Alternative we can set it as a dome
    // _ScnMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

    // Create terrain base
    _TerrainGlobals = new TerrainGlobalOptions();

    _TerrainGroup = new Ogre::TerrainGroup(_ScnMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
    _TerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
    _TerrainGroup->setOrigin(_TerrainPos);

    configureTerrainDefaults(_LightDir);

    // Define our terrain - We are only using a single terrain, 
    // so the method will only be called once. The for loops are just for demonstration in our case.

    for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x) {
        for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y) {
            defineTerrain(x, y);
        }
    }
    // sync load since we want everything in place when we start

    _TerrainGroup->loadAllTerrains(true);

    // Initialise blend maps

    if (_TerrainsImported)
    {
        for (const auto& ti : _TerrainGroup->getTerrainSlots())
        {
            initBlendMaps(ti.second->instance);
        }
    }

    _TerrainGroup->freeTemporaryResources();

    // Make a tray top

    //_InfoLabel = mTrayMgr->createLabel(TL_TOP, "TInfo", "", 350);
 
}        

/*
bool BasicTutorial1::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mTrayMgr->isDialogVisible()) {
        return true;
    }

    if (_TerrainGroup->isDerivedDataUpdateInProgress())
    {
        mTrayMgr->moveWidgetToTray(_InfoLabel, TL_TOP, 0);
        _InfoLabel->show();
        if (_TerrainsImported)
        {
            _InfoLabel->setCaption("Building terrain, please wait...");
        }
        else
        {
            _InfoLabel->setCaption("Updating textures, patience...");
        }
    }
    else
    {
        mTrayMgr->removeWidgetFromTray(_InfoLabel);
        _InfoLabel->hide();
        if (_TerrainsImported)
        {
            // FIXME does not end up in the correct resource group
            // saveTerrains(true);
            _TerrainsImported = false;
        }
    }

    mInputListenerChain.frameRendered(evt);
    return true;
}
*/

void BasicTutorial1::configureTerrainDefaults(Light* lightin) {
    //! [configure_lod]
    _TerrainGlobals->setMaxPixelError(8);
    _TerrainGlobals->setCompositeMapDistance(3000);
    //! [configure_lod]

    // mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
    // mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
    // mTerrainGlobals->setLightMapSize(256);

    TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
        _TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());

    // Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on
    // desktop). Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
    if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities()->getNumTextureUnits() < 9)
    {
        matProfile->setLightmapEnabled(false);
    }

    // Disable steep parallax by default
    matProfile->setLayerParallaxOcclusionMappingEnabled(false);

    //! [composite_lighting]
    // Important to set these so that the terrain knows what to use for baked (non-realtime) data
    _TerrainGlobals->setLightMapDirection(lightin->getDerivedDirection());
    _TerrainGlobals->setCompositeMapAmbient(_ScnMgr->getAmbientLight());
    _TerrainGlobals->setCompositeMapDiffuse(lightin->getDiffuseColour());
    //! [composite_lighting]
    // mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);

    // Configure default import settings for if we use imported image
    //! [import_settings]
    Ogre::Terrain::ImportData& defaultimp = _TerrainGroup->getDefaultImportSettings();
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    //! [import_settings]

    //! [tex_from_src]
    Image combined;
    combined.loadTwoImagesAsRGBA("Ground23_col.jpg", "Ground23_spec.png", "General");
    TextureManager::getSingleton().loadImage("Ground23_diffspec", "General", combined);
    //! [tex_from_src]

    //! [textures]
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 200;
    defaultimp.layerList[0].textureNames.push_back("Ground37_diffspec.dds");
    defaultimp.layerList[0].textureNames.push_back("Ground37_normheight.dds");
    defaultimp.layerList[1].worldSize = 200;
    defaultimp.layerList[1].textureNames.push_back("Ground23_diffspec"); // loaded from memory
    defaultimp.layerList[1].textureNames.push_back("Ground23_normheight.dds");
    defaultimp.layerList[2].worldSize = 400;
    defaultimp.layerList[2].textureNames.push_back("Rock20_diffspec.dds");
    defaultimp.layerList[2].textureNames.push_back("Rock20_normheight.dds");
    //! [textures]
}

void BasicTutorial1::defineTerrain(long x, long y) {

    // if a file is available, use it
    // if not, generate file from import

 
    String filename = _TerrainGroup->generateFilename(x, y);
    if (ResourceGroupManager::getSingleton().resourceExists(_TerrainGroup->getResourceGroup(), filename))
    {
        _TerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        _TerrainGroup->defineTerrain(x, y, &img);
        _TerrainsImported = true;
    }

}

void BasicTutorial1::getTerrainImage(bool flipX, bool flipY, Image& img)
{
    //! [heightmap]
    img.load("terrain.png", _TerrainGroup->getResourceGroup());
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
    //! [heightmap]
}

void BasicTutorial1::initBlendMaps(Terrain* terrain) {

    //! [blendmap]

    TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    float minHeight0 = 20;
    float fadeDist0 = 15;
    float minHeight1 = 70;
    float fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Real tx, ty;

            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            float height = terrain->getHeightAtTerrainPosition(tx, ty);

            *pBlend0++ = Math::saturate((height - minHeight0) / fadeDist0);
            *pBlend1++ = Math::saturate((height - minHeight1) / fadeDist1);
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
    //! [blendmap]
    // set up a colour map
    /*
      if (!terrain->getGlobalColourMapEnabled())
      {
      terrain->setGlobalColourMapEnabled(true);
      Image colourMap;
      colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      terrain->getGlobalColourMap()->loadImage(colourMap);
      }
    */
}


bool BasicTutorial1::keyPressed(const KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}


int main(int argc, char** argv)
{
    try
    {
        BasicTutorial1 app;
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

//! [fullsource]