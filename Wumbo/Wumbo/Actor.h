#pragma once
#include "Physics.h"
#include <SDL.h>
#include <vector>

enum class ActorState
{
	Active,
	Paused,
	Zombie
};

class Actor
{
public:
	Actor(class Game* game);
	virtual ~Actor();

	void input(const Uint8* keyState, const char* buttonState, const char* currButton);
	void update(float deltaTime);

	// Getters/setters
	const Coord2& getPosition() { return position_; }
	void setPosition(Coord2 position) { position_ = position; }
	float getScale() const { return scale_; }
	void setScale(float scale) { scale_ = scale; }
	const Coord2& getSize() { return size_; }
	void setSize(Coord2 size) { size_ = size; }
	ActorState getState() const { return state_; }
	void setState(ActorState state) { state_ = state; }

	bool intersect(Actor* other);

	class Game* getGame() { return game_; }

	template <typename T>
	T* getComponent() const
	{
		SDL_Log("Component size is %d", components_.size());
		for (auto c : components_)
		{
			T* t = dynamic_cast<T*>(c);
			SDL_Log("At component with address %p", c);
			if (t)
				return t;
		}
		return nullptr;
	}

protected:
	virtual void onInput(const Uint8* keyState, const char* buttonState, const char* currButton);
	virtual void onUpdate(float deltaTime);
	Coord2 position_ = Coord2(0.0f, 0.0f);
	ActorState state_ = ActorState::Active;
	float scale_ = 1.0f;
	Coord2 size_ = Coord2(0.0f, 0.0f);
	class Game* game_;

	std::vector<class Component*> components_;

private:
	friend class Component;
	void addComponent(class Component* c) { components_.emplace_back(c); };
};
