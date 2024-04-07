#include "Button.h"
#include "Game.h"
#include "RectComponent.h"
#include "RGBColor.h"

Button::Button(Game* game, Coord2 pos, RGBColor color)
	:Actor(game) {
	setSize(Coord2(Game::BUTTON_WIDTH, Game::BUTTON_WIDTH));
	setPosition(pos);
	rectComponent_ = new RectComponent(this, color);
}

Button::~Button()
{}
