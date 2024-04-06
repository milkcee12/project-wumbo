#pragma once
#include <SDL.h>

class Component
{
public:
	Component(class Actor* owner);
	virtual ~Component();
	virtual void update(float deltaTime);
	virtual void input(const Uint8* keyState);

	class Actor* getOwner() const { return owner_; }

protected:
	class Actor* owner_;
};
