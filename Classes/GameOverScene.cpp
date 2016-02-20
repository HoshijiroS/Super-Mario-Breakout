#include "GameOverScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

bool GameOverLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255)))
	{
		return false;
	}

	Size winSize = Director::getInstance()->getWinSize();
	this->_label = Label::createWithSystemFont("", "Arial", 12);
	this->_label->retain();
	this->getLabel()->setColor(Color3B(0, 0, 0));
	this->getLabel()->setPosition(winSize.width / 2, winSize.height / 2);
	this->addChild(_label);

	this->runAction(
		Sequence::create(
			DelayTime::create(3),
			CallFunc::create(
				[&]() {
		gameOverDone();
	}
				),
			NULL
		)
		);

	return true;
}

void GameOverLayer::gameOverDone()
{
	Director::getInstance()->replaceScene(HelloWorld::createScene());
}

GameOverLayer::~GameOverLayer()
{
	_label->release();
}

bool GameOverScene::init()
{
	if (CCScene::init())
	{
		this->_layer = GameOverLayer::create();
		this->_layer->retain();
		this->addChild(_layer);

		return true;
	}
	else
	{
		return false;
	}
}

GameOverScene::~GameOverScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}
