#include "Component.h"
#include "Physics.h"

class CollisionComponent : Component
{
	CollisionComponent(class Actor* owner);

	bool intersect(class Actor* other);

private:
};
