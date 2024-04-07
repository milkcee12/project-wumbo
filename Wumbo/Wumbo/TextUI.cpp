#include "TextUI.h"
#include "Game.h"
#include <SDL.h>
#include "Texture.h"

TextUI::TextUI(Game* game)
	: Actor(game)
{
	std::string fontFile = "C:\\Users\\micha\\Documents2\\_active-classes\\project-wumbo\\Wumbo\\Wumbo\\Assets\\ROCK_KAPAK_2.ttf";
	font_ = TTF_OpenFont(fontFile.c_str(), 36);
	texture_ = new Texture();
}

TextUI::TextUI(class Game* game, float lifetime, bool bigText)
	: Actor(game), lifetime_(lifetime)
{
	std::string fontFile = "C:\\Users\\micha\\Documents2\\_active-classes\\project-wumbo\\Wumbo\\Wumbo\\Assets\\ROCK_KAPAK_2.ttf";

	int size = 36;
	if (bigText)
		size = 80;

	font_ = TTF_OpenFont(fontFile.c_str(), size);
	texture_ = new Texture();
}

void TextUI::onUpdate(float deltaTime)
{
	if (lifetime_ != FLT_MIN)
	{
		if (lifetime_ < 0.0f)
			setState(ActorState::Zombie);
		else
			lifetime_ -= deltaTime;
	}
}

void TextUI::render()
{
	if (getState() != ActorState::Active)
		return;

	if (texture_)
		texture_->render(game_->getRenderer(), static_cast<int>(position_.x), static_cast<int>(position_.y));
}

void TextUI::writeMessage(std::string message, RGBColor color)
{
	if (!texture_)
		return;

	SDL_Color sdlColor;
	sdlColor.r = color.r_;
	sdlColor.g = color.g_;
	sdlColor.b = color.b_;
	texture_->loadFromRenderedText(message, sdlColor, font_, game_->getRenderer());
}

void TextUI::clearMessage()
{
	texture_->free();
}

TextUI::~TextUI()
{
	if (font_)
		TTF_CloseFont(font_);
}
