#include "Component.h"
#include "Actor.h"

Component::Component(Actor* owner)
	: owner_(owner)
{
	owner_->addComponent(this);
}

void Component::update(float deltaTime)
{}

void Component::input(const Uint8* keyState)
{}

Component::~Component()
{
}
