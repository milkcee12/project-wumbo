#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include "RGBColor.h"
#include "mosquittopp.h"

class Game {
public:
	Game();
	void init();
	void runLoop();
	void shutdown();
	void initMqtt();

	void loadData();
	void unloadData();

	const std::vector<class Actor*>& getActors() { return actors_; }
	void addActor(class Actor* actor) { actors_.push_back(actor); }
	void removeActor(class Actor* actor);
	void addSprite(class RectComponent* sprite);
	void removeSprite(class RectComponent* sprite);

	SDL_Renderer* getRenderer() { return renderer_; };

	static const int WINDOW_W = 1024;
	static const int WINDOW_H = 768;

	static const int NUM_BUTTONS = 4;
	static const int BUTTON_WIDTH = 120;
	static const int BUTTON_GAP = 170;

	std::vector<class TextUI*> textUis_;

	class TextUI* songTitleText_;
	class TextUI* scoreText_;

	int score_ = 0;
	int total_ = 0;

	class Button* buttons_[NUM_BUTTONS];

	RGBColor bgColor_;
	RGBColor stringColor_;

	RGBColor buttonColors[NUM_BUTTONS] = {
		RGBColor(10, 227, 7),
		RGBColor(252, 186, 3),
		RGBColor(10, 227, 7),
		RGBColor(252, 186, 3),
	};

	RGBColor buttonColorsPressed[NUM_BUTTONS] = {
		RGBColor(10, 138, 6),
		RGBColor(184, 153, 17),
		RGBColor(10, 138, 6),
		RGBColor(184, 153, 17),
	};

	char* currButton = nullptr;
	char* buttonState = nullptr;

	static constexpr float REFRESH_TIME = 0.2f;
	float refresh = REFRESH_TIME;

	class NoteSpawner* noteSpawner_;

	class MqttJoystick* mqtt_joy;
	class MqttButtons* mqtt_button;
	mosqpp::mosquittopp* mqtt_publish;

	//const double TICK_MODIFIER = 0.3;
	//const int SONG_TICKS_PER_SECONDS[4] = { 352 * TICK_MODIFIER, 960 * TICK_MODIFIER, 760 * TICK_MODIFIER };
	//const int SONG_NUM = 2;

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

	int PORT = 1883;
	const char* HOST = "broker.emqx.io";

};
