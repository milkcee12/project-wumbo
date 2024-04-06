#include "Actor.h"
#include "Game.h"

Actor::Actor(Game* game)
	: game_(game)
{
	game_->addActor(this);
}

Actor::~Actor()
{
	game_->removeActor(this);
}

void Actor::input(const Uint8* keyState)
{
	if (state_ == ActorState::Active)
	{
		onInput(keyState);
	}
}

void Actor::update(float deltaTime)
{
	if (state_ == ActorState::Active)
	{
		onUpdate(deltaTime);
	}
}

void Actor::onInput(const Uint8* keyState) {}
void Actor::onUpdate(float deltaTime) {}
