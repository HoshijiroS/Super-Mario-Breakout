#include "GameOverScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

bool GameOverLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0)))
	{
		return false;
	}

	Size winSize = Director::getInstance()->getWinSize();
	TTFConfig label_config;
	label_config.fontFilePath = "fonts/8-bit pusab.ttf";
	label_config.fontSize = 20;
	label_config.glyphs = GlyphCollection::DYNAMIC;
	label_config.customGlyphs = nullptr;
	label_config.distanceFieldEnabled = false;
	label_config.outlineSize = 0;
	_label = Label::createWithTTF(label_config, "");
	_label->retain();
	_label->setColor(Color3B(255, 255, 255));
	_label->setPosition(winSize.width / 2, winSize.height / 2);
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
