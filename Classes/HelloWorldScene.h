#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define COCOS2D_DEBUG 1
#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include <Box2D/Box2D.h>
#include "ContactListener.h"
#include "GameOverScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

class HelloWorld : public cocos2d::Layer {
public:
    ~HelloWorld();
	static cocos2d::Scene* createScene();

	virtual bool init();
    
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
	Animate* setCoinAnimation(int, char*);
	Animate* setJellyAnimation(int, char*);
    void tick(float dt);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	Animate* coinAnimate;
	Animate* marioAnimate;
	CREATE_FUNC(HelloWorld);

	Label* labelScore;
	int coins;
    
private:
    b2World* _world;
    b2Body *_groundBody;
    b2Fixture *_bottomFixture;
    b2Fixture *_ballFixture;
    b2Body *_paddleBody;
    b2Fixture *_paddleFixture;
    b2MouseJoint *_mouseJoint;
	Sprite* background;
	Sprite* paddle;
    
    ContactListener *_contactListener;
};

#endif // __HELLOWORLD_SCENE_H__
