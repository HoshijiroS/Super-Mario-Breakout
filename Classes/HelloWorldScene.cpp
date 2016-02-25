#include "HelloWorldScene.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 25

HelloWorld::~HelloWorld()
{
    CC_SAFE_DELETE(_world);
    _groundBody = NULL;
    delete _contactListener;
}

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene

	return scene;
}

bool HelloWorld::init() {
	if (!Layer::init()) {
		return false;
	}
    //setTouchEnabled( true );

	//CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bgm.wav", true);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("mario_bgm.wav", true);

	auto listener = EventListenerKeyboard::create();
	auto eventListener = EventListenerKeyboard::create();
	
	listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
	Size winSize = CCDirector::getInstance()->getWinSize();
    
	this->coins = 0;
	TTFConfig label_config2;
	label_config2.fontFilePath = "fonts/8-bit pusab.ttf";
	label_config2.fontSize = 10;
	label_config2.glyphs = GlyphCollection::DYNAMIC;
	label_config2.customGlyphs = nullptr;
	label_config2.distanceFieldEnabled = false;
	label_config2.outlineSize = 0;
	labelScore = Label::createWithTTF(label_config2, " 0");
	labelScore->setColor(Color3B(255, 255, 255));
	labelScore->setPosition(35, 440);
	this->addChild(labelScore, 5);

	// Define the gravity vector.
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	
	// Do we want to let bodies sleep?
	bool doSleep = true;
    
	// Construct a world object, which will hold and simulate the rigid bodies.
	_world = new b2World(gravity);    _world->SetAllowSleeping(doSleep);
    
    float widthInMeters = winSize.width / PTM_RATIO;
    float heightInMeters = winSize.height / PTM_RATIO;
    b2Vec2 lowerLeftCorner = b2Vec2(0, 0);
    b2Vec2 lowerRightCorner = b2Vec2(widthInMeters, 0);
    b2Vec2 upperLeftCorner = b2Vec2(0, heightInMeters);
    b2Vec2 upperRightCorner = b2Vec2(widthInMeters, heightInMeters);
    
    b2BodyDef screenBorderDef;
    screenBorderDef.position.Set(0, 0);
    _groundBody= _world->CreateBody(&screenBorderDef);
    b2EdgeShape screenBorderShape;
    
    // Create fixtures for the four borders (the border shape is re-used)
    screenBorderShape.Set(lowerLeftCorner, lowerRightCorner);
    _bottomFixture = _groundBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(lowerRightCorner, upperRightCorner);
    _groundBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(upperRightCorner, upperLeftCorner);
    _groundBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(upperLeftCorner, lowerLeftCorner);
    _groundBody->CreateFixture(&screenBorderShape, 0);
	
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	background = Sprite::create("bg.png"); // 434 153
	background->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(background);
    
    // Create sprite and add it to the layer
	Sprite *ball = Sprite::create("jellyfish/0001.png");
	this->marioAnimate = setJellyAnimation(2, "jellyfish/%04d.png");
    ball->setPosition(100, 100);
    ball->setTag(1);
    this->addChild(ball);
	ball->runAction(RepeatForever::create(this->marioAnimate));
    
    // Create ball body
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(100/PTM_RATIO, 100/PTM_RATIO);
    ballBodyDef.userData = ball;
    
    b2Body *ballBody = _world->CreateBody(&ballBodyDef);
    
    // Create circle shape
    b2CircleShape circle;
    circle.m_radius = 26.0/PTM_RATIO;
    
    // Create shape definition and add body
    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 1.0f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = 1.0f;
    _ballFixture = ballBody->CreateFixture(&ballShapeDef);
    
    b2Vec2 force = b2Vec2(10, 10);
    ballBody->ApplyLinearImpulse(force, ballBodyDef.position, true);
    
    paddle = Sprite::create("platform.png");
    paddle->setPosition(winSize.width/2, 100);
    this->addChild(paddle);
    
    // Create paddle body
    b2BodyDef paddleBodyDef;
    paddleBodyDef.type = b2_dynamicBody;
    paddleBodyDef.position.Set(winSize.width/2/PTM_RATIO, 120/PTM_RATIO);
	paddleBodyDef.userData = paddle;
    _paddleBody = _world->CreateBody(&paddleBodyDef);
    
    // Create paddle shape
    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(paddle->getContentSize().width/PTM_RATIO/2,
                         paddle->getContentSize().height/PTM_RATIO/2);
    
    // Create shape definition and add to body
    b2FixtureDef paddleShapeDef;
    paddleShapeDef.shape = &paddleShape;
    paddleShapeDef.density = 10.0f;
    paddleShapeDef.friction = 0.0f;
    paddleShapeDef.restitution = 1.0f;
    _paddleFixture = _paddleBody->CreateFixture(&paddleShapeDef);
    
    // Restrict paddle along the x axis
    b2PrismaticJointDef jointDef;
    b2Vec2 worldAxis(1.0f, 0.0f);
    jointDef.collideConnected = true;
    jointDef.Initialize(_paddleBody, _groundBody,
                        _paddleBody->GetWorldCenter(), worldAxis);
    _world->CreateJoint(&jointDef);
    
	b2MouseJointDef md;
	md.bodyA = _groundBody;
	md.bodyB = _paddleBody;
	b2Vec2 paddleLocation = b2Vec2(paddle->getPositionX() / PTM_RATIO, (paddle->getPositionY() / PTM_RATIO));
	md.target = paddleLocation;
	md.collideConnected = true;
	md.maxForce = 1000.0f * _paddleBody->GetMass();

	_mouseJoint = (b2MouseJoint *)_world->CreateJoint(&md);
	_paddleBody->SetAwake(true);
    
    // Create contact listener
    _contactListener = new ContactListener();
    _world->SetContactListener(_contactListener);
    
	int counter = 0;
	int yOffset = 300;
	int xOffset = 0;

	for(int i = 0; i < 3; i++) {
		xOffset = 25;
		for (int j = 0; j < 10; j++) {
			static int padding = 0;

			// Create block and add it to the layer
			Sprite* block = Sprite::create("coin/0001.png");
			this->coinAnimate = setCoinAnimation(4, "coin/%04d.png");
			//int xOffset = padding+block->getContentSize().width/2+
			//((block->getContentSize().width+padding)*i);
			xOffset = xOffset + 25;

			block->setPosition(xOffset, yOffset);
			//block->setPosition(xOffset, 250);
			block->setTag(2);
			this->addChild(block);
			block->runAction(RepeatForever::create(this->coinAnimate));

			// Create block body
			b2BodyDef blockBodyDef;
			blockBodyDef.type = b2_staticBody;
			blockBodyDef.position.Set(xOffset / PTM_RATIO + 0.2, yOffset / PTM_RATIO);
			blockBodyDef.userData = block;
			b2Body *blockBody = _world->CreateBody(&blockBodyDef);

			// Create block shape
			b2PolygonShape blockShape;
			blockShape.SetAsBox(block->getContentSize().width / PTM_RATIO / 2,
				block->getContentSize().height / PTM_RATIO / 2);

			// Create shape definition and add to body
			b2FixtureDef blockShapeDef;
			blockShapeDef.shape = &blockShape;
			blockShapeDef.density = 10.0f;
			blockShapeDef.friction = 0.0f;
			blockShapeDef.restitution = 1.0f;
			//blockShapeDef.isSensor = true;
			blockBody->CreateFixture(&blockShapeDef);
		}
		yOffset = yOffset - 25;
    }
    
	this->schedule(schedule_selector(HelloWorld::tick));

	return true;
}

void HelloWorld::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
}

void HelloWorld::tick(float dt)
{
	//It is recommended that a fixed time step is used with Box2D for stability
	//of the simulation, however, we are using a variable time step here.
	//You need to make an informed choice, the following URL is useful
	//http://gafferongames.com/game-physics/fix-your-timestep/
	
	int velocityIterations = 8;
	int positionIterations = 1;
    
	// Instruct the world to perform a single step of simulation. It is
	// generally best to keep the time step and iterations fixed.
	_world->Step(dt, velocityIterations, positionIterations);
	
    bool blockFound = false;
    
	//Iterate over the bodies in the physics world
	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			//Synchronize the AtlasSprites position and rotation with the corresponding body
			Sprite* myActor = (Sprite*)b->GetUserData();
			myActor->setPosition( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO );
			myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
            
            if (myActor->getTag() == 1) {
                static int maxSpeed = 10;
                
                b2Vec2 velocity = b->GetLinearVelocity();
                float32 speed = velocity.Length();
                
				if (speed > maxSpeed) {
                    b->SetLinearDamping(0.5);
                } else if (speed < maxSpeed) {
                    b->SetLinearDamping(0.0);
                }
                
            }
            
            if (myActor->getTag() == 2) {
                blockFound = true;
            }
		}
	}
    
    std::vector<b2Body *>toDestroy;
    std::vector<MyContact>::iterator pos;
    for(pos = _contactListener->_contacts.begin();
        pos != _contactListener->_contacts.end(); ++pos) {
        MyContact contact = *pos;
        
        if ((contact.fixtureA == _bottomFixture && contact.fixtureB == _ballFixture) ||
            (contact.fixtureA == _ballFixture && contact.fixtureB == _bottomFixture)) {
            GameOverScene *gameOverScene = GameOverScene::create();
            gameOverScene->getLayer()->getLabel()->setString("GAME OVER!");
			SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
			SimpleAudioEngine::getInstance()->playEffect("smb_gameover.wav");
			//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("smb_gameover.wav");
			CCDirector::getInstance()->replaceScene(gameOverScene);
			//cocos2d::Director::getInstance()->pause();
        }
        
        b2Body *bodyA = contact.fixtureA->GetBody();
        b2Body *bodyB = contact.fixtureB->GetBody();
        if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL) {
            Sprite *spriteA = (Sprite *) bodyA->GetUserData();
            Sprite *spriteB = (Sprite *) bodyB->GetUserData();
            
            // Sprite A = ball, Sprite B = Block
            if (spriteA->getTag() == 1 && spriteB->getTag() == 2) {
                if (std::find(toDestroy.begin(), toDestroy.end(), bodyB)
                    == toDestroy.end()) {
					SimpleAudioEngine::getInstance()->playEffect("smb_coin.wav");
					//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("smb_coin.wav");
                    toDestroy.push_back(bodyB);
					coins++;
					this->labelScore ->setString(ccsf(" %d", coins));
                }
            }
            // Sprite B = block, Sprite A = ball
            else if (spriteA->getTag() == 2 && spriteB->getTag() == 1) {
                if (std::find(toDestroy.begin(), toDestroy.end(), bodyA)
                    == toDestroy.end()) {
					SimpleAudioEngine::getInstance()->playEffect("smb_coin.wav");
					//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("smb_coin.wav");
                    toDestroy.push_back(bodyA);
					coins++;
					this->labelScore->setString(ccsf(" %d", coins));
                }
            }
        }
    }
    
    std::vector<b2Body *>::iterator pos2;
    for(pos2 = toDestroy.begin(); pos2 != toDestroy.end(); ++pos2) {
        b2Body *body = *pos2;
        if (body->GetUserData() != NULL) {
            Sprite *sprite = (Sprite *) body->GetUserData();
            this->removeChild(sprite, true);
        }
        _world->DestroyBody(body);
    }
    
    if (!blockFound)
    {
        GameOverScene *gameOverScene = GameOverScene::create();
        gameOverScene->getLayer()->getLabel()->setString("YOU WIN!");
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		SimpleAudioEngine::getInstance()->playEffect("smb_world_clear.wav");
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("smb_world_clear.wav");
        Director::getInstance()->replaceScene(gameOverScene);
		cocos2d::Director::getInstance()->pause();
    }
    
    if (toDestroy.size() > 0)
    {
        // SimpleAudioEngine::getInstance()->playEffect("blip.caf");
    }
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (_mouseJoint == NULL) 
		return;

		b2Vec2 locationWorld = b2Vec2(paddle->getPositionX() / PTM_RATIO, paddle->getPositionY() / PTM_RATIO);

		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			locationWorld = b2Vec2(paddle->getPositionX() / PTM_RATIO + 1, paddle->getPositionY() / PTM_RATIO);
			_mouseJoint->SetTarget(locationWorld);
			break;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			locationWorld = b2Vec2(paddle->getPositionX() / PTM_RATIO - 1, paddle->getPositionY() / PTM_RATIO);
			_mouseJoint->SetTarget(locationWorld);
			break;
		case EventKeyboard::KeyCode::KEY_R:
			Director::getInstance()->replaceScene(HelloWorld::createScene());
			break;
		}

}

Animate* HelloWorld::setCoinAnimation(int j, char* filename) {
	Vector<SpriteFrame*> coinFrames;
	char str[100];
	for (int i = 1; i <= j; i++) //Iterate for the number of images you have
	{
		sprintf(str, filename, i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 16, 16)); //The size of the images in an action should be the same
		coinFrames.pushBack(frame);
	}

	return Animate::create(Animation::createWithSpriteFrames(coinFrames, 0.05f));
}

Animate* HelloWorld::setJellyAnimation(int j, char* filename) {
	Vector<SpriteFrame*> coinFrames;
	char str[100];
	for (int i = 1; i <= j; i++) //Iterate for the number of images you have
	{
		sprintf(str, filename, i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 20, 24)); //The size of the images in an action should be the same
		coinFrames.pushBack(frame);
	}

	return Animate::create(Animation::createWithSpriteFrames(coinFrames, 0.09f));
}