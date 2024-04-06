#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(Actor* owner)
	: Component(owner)
{}

bool CollisionComponent::intersect(Actor* other)
{
	return false;
}
