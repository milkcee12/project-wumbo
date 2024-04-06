#include "RectComponent.h"
#include "Actor.h"
#include <SDL.h>
#include "Game.h"

RectComponent::RectComponent(Actor* owner, int drawOrder)
	: Component(owner)
	, drawOrder_(drawOrder)
	, isVisible_(true)
{
	size_ = owner_->getSize();
	position_ = owner_->getPosition();
	owner_->getGame()->addSprite(this);
}

RectComponent::RectComponent(Actor* owner, Color color, int drawOrder)
	: Component(owner)
	, drawOrder_(drawOrder)
	, isVisible_(true)
{
	color_ = color;
	size_ = owner_->getSize();
	position_ = owner_->getPosition();
	owner_->getGame()->addSprite(this);
}

RectComponent::~RectComponent()
{
	owner_->getGame()->removeSprite(this);
}

void RectComponent::draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, color_.r_, color_.g_, color_.b_, opacity_);

	SDL_Rect r;
	r.w = static_cast<int>(size_.x * owner_->getScale());
	r.h = static_cast<int>(size_.y * owner_->getScale());

	// Center the rectangle around the position of the owner
	r.x = static_cast<int>(position_.x - r.w / 2);
	r.y = static_cast<int>(position_.y - r.h / 2);

	SDL_RenderFillRect(renderer, &r);

	// Draw (have to convert angle from radians to degrees, and clockwise to counter)
	// SDL_RenderCopyEx(renderer, mTexture, nullptr, &r, -Math::ToDegrees(owner_->getRotation()),
	//	nullptr, SDL_FLIP_NONE);
}
