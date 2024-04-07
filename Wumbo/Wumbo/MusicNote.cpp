#include "MusicNote.h"
#include "Game.h"
#include "RectComponent.h"
#include "Button.h"

MusicNote::MusicNote(Game* game, float start, float duration, int button, int frequency, int pianoNote)
	: Actor(game)
{
	startTime_ = start;
	duration_ = duration;
	button_ = button;
	frequency_ = frequency;
	pianoNote_ = pianoNote;

	float height = (Game::BUTTON_WIDTH) * (duration * 4);
	setSize(Coord2(Game::BUTTON_WIDTH, height));
	setPosition(Coord2(game_->buttons_[button_]->getPosition().x, -height / 2));
	rectComponent_ = new RectComponent(this, game_->buttonColors[button_]);
}

MusicNote::~MusicNote()
{
}


void MusicNote::passedButton()
{
	if (!passed)
	{
		if (played_)
			rectComponent_->color_ = RGBColor(255, 255, 255);
		else
			rectComponent_->color_ = RGBColor(255, 0, 0);
	}
	else
	{
		passed = true;
	}
}

void MusicNote::onUpdate(float deltaTime)
{
	position_.y += VELOCITY * deltaTime;
	rectComponent_->setPosition(position_);
}
