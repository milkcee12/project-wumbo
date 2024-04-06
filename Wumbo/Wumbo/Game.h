#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include "RectComponent.h"

class Game {
public:
	Game();
	void init();
	void runLoop();
	void shutdown();

	void loadData();
	void unloadData();

	const std::vector<class Actor*>& getActors() { return actors_; }
	void addActor(class Actor* actor) { actors_.push_back(actor); }
	void removeActor(class Actor* actor);
	void addSprite(class RectComponent* sprite);
	void removeSprite(class RectComponent* sprite);

	static const int WINDOW_W = 1024;
	static const int WINDOW_H = 768;

	static const int NUM_BUTTONS = 5;
	static const int BUTTON_WIDTH = 100;
	static const int BUTTON_GAP = 170;

	Color bgColor_;
	Color stringColor_;
	std::vector<class Button*> buttons_;

private:
	// Functions
	void input();
	void update();
	void render();

	// SDL variables
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;

	// Other variables
	bool loopRunning_ = false;
	Uint32 prevFrameTime_ = 0;
	std::vector<class Actor*> actors_;
	std::vector<class RectComponent*> sprites_;
};
