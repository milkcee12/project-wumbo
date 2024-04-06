#include "Button.h"
#include "Game.h"
#include "RectComponent.h"

Button::Button(Game* game, Coord2 pos, Color color)
	:Actor(game) {
	setSize(Coord2(Game::BUTTON_WIDTH, Game::BUTTON_WIDTH));
	setPosition(pos);
	rectComponent_ = new RectComponent(this, color);
}

Button::~Button()
{}
