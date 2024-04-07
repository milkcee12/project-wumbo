#pragma once
#include "Component.h"
#include "Physics.h"
#include "RGBColor.h"

class RectComponent : public Component {
public:
	RectComponent(class Actor* owner, int drawOrder = 100);
	RectComponent(class Actor* owner, class RGBColor color, int drawOrder = 100);
	~RectComponent();

	void draw(SDL_Renderer* renderer);
	bool isVisible() { return isVisible_; }

	int getDrawOrder() { return drawOrder_; }
	const Coord2& getPosition() { return position_; }
	void setPosition(Coord2 position) { position_ = position; }
	const Coord2& getSize() { return size_; }
	void setSize(Coord2 size) { size_ = size; }

	RGBColor color_;

private:
	Coord2 size_;
	Coord2 position_;
	int drawOrder_;
	bool isVisible_ = true;
	int opacity_ = 255;
};
