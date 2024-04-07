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

bool Actor::intersect(Actor* other)
{
	Coord2 thisMin(getPosition().x - (getSize().x / 2), getPosition().y - (getSize().y / 2));
	Coord2 thisMax(getPosition().x + (getSize().x / 2), getPosition().y + (getSize().y / 2));
	Coord2 otherMin(other->getPosition().x - (other->getSize().x / 2), other->getPosition().y - (other->getSize().y / 2));
	Coord2 otherMax(other->getPosition().x + (other->getSize().x / 2), other->getPosition().y + (other->getSize().y / 2));

	bool noIntersection = thisMax.x < otherMin.x || thisMax.y < otherMin.y || otherMax.x < thisMin.x ||
		otherMax.y < thisMin.y;

	return !noIntersection;
}

void Actor::input(const Uint8* keyState, const char* buttonState, const char* currButton)
{
	if (state_ == ActorState::Active)
	{
		onInput(keyState, buttonState, currButton);
	}
}

void Actor::update(float deltaTime)
{
	if (state_ == ActorState::Active)
	{
		onUpdate(deltaTime);
	}
}

void Actor::onInput(const Uint8* keyState, const char* buttonState, const char* currButton) {}
void Actor::onUpdate(float deltaTime) {}
