#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Physics.h"
#include "Button.h"
#include "TextUI.h"
#include "RectComponent.h"
#include "RGBColor.h"
#include "NoteSpawner.h"
#include <SDL_mixer.h>
#include "mosquittopp.h"
#include "MqttJoystick.h"
#include "MqttButtons.h"

Game::Game()
	: songTitleText_(nullptr),
	scoreText_(nullptr),
	score_(0),
	noteSpawner_(nullptr),
	mqtt_joy(nullptr),
	mqtt_button(nullptr),
	mqtt_publish(nullptr)
{
	for (int i = 0; i < NUM_BUTTONS; i++)
		buttons_[i] = nullptr;

	bgColor_ = RGBColor(28, 28, 28);
	stringColor_ = RGBColor(45, 45, 45);
}

void Game::runLoop()
{
	while (loopRunning_)
	{
		mqtt_joy->loop();
		mqtt_button->loop();
		input();
		update();
		render();
	}
}

void Game::removeActor(Actor* actor)
{
	auto it = std::find(actors_.begin(), actors_.end(), actor);
	if (it != actors_.end())
		actors_.erase(it);
}

void Game::loadData()
{
	// Create background art
	Actor* background = new Actor(this);
	background->setSize(Coord2(static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H)));
	background->setPosition(Coord2(static_cast<float>(WINDOW_W) / 2, static_cast<float>(WINDOW_H) / 2));
	RectComponent* bgFill = new RectComponent(background, bgColor_, 50);

	float margin = (WINDOW_W - ((NUM_BUTTONS - 1) * BUTTON_GAP)) / 2.0f;
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		Coord2 pos = Coord2(margin + BUTTON_GAP * i, static_cast<float>(WINDOW_H - 100));

		RectComponent* string = new RectComponent(background, stringColor_, 51);
		string->setPosition(Coord2(pos.x, WINDOW_H / 2.0f));
		string->setSize(Coord2(5.0f, static_cast<float>(WINDOW_H)));

		Button* button = new Button(this, pos, buttonColors[i]);
		buttons_[i] = button;
	}

	songTitleText_ = new TextUI(this);
	songTitleText_->setPosition(Coord2(100.0f, 50.0f));
	songTitleText_->writeMessage("All of Me");

	scoreText_ = new TextUI(this);
	scoreText_->setPosition(Coord2(100.0f, 120.0f));
	scoreText_->writeMessage("0/?");

	textUis_.push_back(songTitleText_);
	textUis_.push_back(scoreText_);

	noteSpawner_ = new NoteSpawner(this);
}

void Game::unloadData()
{
	if (mqtt_joy) {
		delete mqtt_joy;
	}
	if (mqtt_button)
		delete mqtt_button;

	if (mqtt_publish)
		delete mqtt_publish;
	mosqpp::lib_cleanup();
}

void Game::input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// Allows X button to quit
		switch (event.type)
		{
		case SDL_QUIT:
			loopRunning_ = false;
			break;
		}
	}

	// Keyboard state
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		loopRunning_ = false;
	}

	std::vector<Actor*> actorCopy = actors_;
	for (Actor* actor : actorCopy)
		actor->input(keyState, buttonState, currButton);

}

void Game::update()
{
	// Calculate delta time
	Uint32 currTime = 0;
	Uint32 tickDiff = 0;
	do
	{
		currTime = SDL_GetTicks();
		tickDiff = currTime - prevFrameTime_;
	} while (tickDiff < 16);

	tickDiff = std::min(static_cast<Uint32>(33), tickDiff);
	float deltaTime = tickDiff / 1000.0f;
	prevFrameTime_ = currTime;

	// Update all actors
	std::vector<Actor*> actorCopy = actors_;
	for (Actor* actor : actorCopy)
		actor->update(deltaTime);

	std::vector<Actor*> actorsToDestroy;
	for (Actor* actor : actorCopy)
	{
		if (actor->getState() == ActorState::Zombie)
			actorsToDestroy.push_back(actor);
	}

	for (Actor* actor : actorsToDestroy)
		delete actor;

	/*refresh -= deltaTime;
	if (refresh <= 0.0f)
	{
		buttonState = nullptr;
		currButton = nullptr;
		refresh = REFRESH_TIME;
	}*/
}

void Game::render()
{
	SDL_RenderClear(renderer_);

	for (auto it : sprites_)
	{
		if (it->isVisible())
			it->draw(renderer_);
	}

	for (TextUI* text : textUis_)
	{
		if (text)
			text->render();
	}

	SDL_RenderPresent(renderer_);
}

void Game::addSprite(RectComponent* sprite)
{
	sprites_.push_back(sprite);
	std::sort(sprites_.begin(), sprites_.end(), [](RectComponent* lhs, RectComponent* rhs) {
		return lhs->getDrawOrder() < rhs->getDrawOrder();
		});
}

void Game::removeSprite(RectComponent* sprite)
{
	std::vector<RectComponent*>::iterator it =
		std::find(sprites_.begin(), sprites_.end(), sprite);
	if (it != sprites_.end())
		sprites_.erase(it);
}

void Game::init()
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	window_ = SDL_CreateWindow("Wumbo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, 0);

	if (window_) // make sure it's not null
	{
		renderer_ = SDL_CreateRenderer(window_, 0,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		IMG_Init(IMG_INIT_PNG);
		TTF_Init();

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		initMqtt();

		loadData();

		if (window_ && renderer_)
		{
			loopRunning_ = true;
		}
	}
}

void Game::initMqtt()
{
	const char* user = "emqs";
	const char* pass = "public";

	mosqpp::lib_init();

	mqtt_joy = new MqttJoystick("mqtt_joy", this);
	mqtt_button = new MqttButtons("mqtt_buttons", this);
	mqtt_publish = new mosqpp::mosquittopp("mqtt_publish", this);

	mqtt_joy->connect(HOST, PORT, 0);
	mqtt_button->connect(HOST, PORT, 0);
	mqtt_publish->connect(HOST, PORT, 0);
}

void Game::shutdown()
{
	IMG_Quit();
	Mix_HaltChannel(-1);
	Mix_CloseAudio();
	unloadData();
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

