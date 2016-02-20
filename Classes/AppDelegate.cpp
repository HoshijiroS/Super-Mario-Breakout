#include "AppDelegate.h"
#include "HelloWorldScene.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(320, 448);

void AppDelegate::initMultiResolution()
{
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();

	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);

	std::vector<std::string> searchPaths;
	float scaleFactor = 1.0f;

	director->setContentScaleFactor(scaleFactor);
	FileUtils::getInstance()->setSearchPaths(searchPaths);
}


void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };
	GLView::setGLContextAttrs(glContextAttrs);
}


bool AppDelegate::applicationDidFinishLaunching()
{
	initOpenGL();
	initDirector();
	initMultiResolution();
	createAndRunScene();
	return true;
}


void AppDelegate::initOpenGL()
{
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("Super Mario Rush!", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
		glview = GLViewImpl::create("Code Rush!");
#endif
		director->setOpenGLView(glview);
	}
}

void AppDelegate::initDirector()
{
	auto director = Director::getInstance();
	director->setAnimationInterval(1.0 / 8);
	director->setDisplayStats(true);
}


void AppDelegate::createAndRunScene()
{
	auto scene = HelloWorld::createScene();
	Director::getInstance()->runWithScene(scene);
}


void AppDelegate::applicationDidEnterBackground()
{
	Director::getInstance()->stopAnimation();
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}


void AppDelegate::applicationWillEnterForeground()
{
	Director::getInstance()->startAnimation();
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
