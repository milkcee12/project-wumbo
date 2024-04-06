#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include "SDL_image.h"
#include "Physics.h"
#include "Button.h"

const Color buttonColors[Game::NUM_BUTTONS] = {
	Color(0, 255	, 0),
	Color(255, 0, 0),
	Color(0, 0, 255),
	Color(252, 186, 3),
	Color(255, 255, 255)
};

Game::Game()
{
	bgColor_ = Color(28, 28, 28);
	stringColor_ = Color(45, 45, 45);
}

void Game::runLoop()
{
	while (loopRunning_)
	{
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

	int margin = (WINDOW_W - ((NUM_BUTTONS - 1) * BUTTON_GAP)) / 2;
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		Coord2 pos = Coord2(margin + BUTTON_GAP * i, WINDOW_H - 100);

		RectComponent* string = new RectComponent(background, stringColor_, 51);
		string->setPosition(Coord2(pos.x, WINDOW_H / 2.0f));
		string->setSize(Coord2(5.0f, static_cast<float>(WINDOW_H)));

		Button* button = new Button(this, pos, buttonColors[i]);
		buttons_.push_back(button);
	}
}

void Game::unloadData()
{}

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
}

void Game::render()
{
	SDL_RenderClear(renderer_);

	for (auto it : sprites_)
	{
		if (it->isVisible())
			it->draw(renderer_);
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
		loadData();

		if (window_ && renderer_)
		{
			loopRunning_ = true;
		}
	}
}

void Game::shutdown()
{
	IMG_Quit();
	unloadData();
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

