#pragma once
#include "Actor.h"
#include "Physics.h"

class Button : public Actor
{
public:
	Button(class Game* game, Coord2 pos, struct Color color);
	~Button();

	class RectComponent* rectComponent_;
};
