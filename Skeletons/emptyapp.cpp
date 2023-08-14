#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"

class TutorialApplication
	: public OgreBites::ApplicationContext,
	public OgreBites::InputListener

{
public:
	TutorialApplication();
	virtual ~TutorialApplication();

	void setup();
	bool keyPressed(const OgreBites::KeyboardEvent& evt);

};

TutorialApplication::TutorialApplication()
	: OgreBites::ApplicationContext("OgreTutorialApp")

{

}

TutorialApplication::~TutorialApplication() 

{

}

void TutorialApplication::setup()
{
	OgreBites::ApplicationContext::setup();
	addInputListener(this);
}

bool TutorialApplication::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
		getRoot()->queueEndRendering();
	}
	return true;
}

int main(int argc, char** argv)
{
	try {
		TutorialApplication app;
		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
	}
	catch (const std::exception& e) {
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		return 1;
	}

	return 0;
}