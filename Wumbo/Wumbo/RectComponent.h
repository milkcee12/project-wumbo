#pragma once
#include "Component.h"
#include "Physics.h"

struct Color {
	Uint8 r_ = 0;
	Uint8 g_ = 0;
	Uint8 b_ = 0;

	Color() {};
	Color(Uint8 r, Uint8 g, Uint8 b) : r_(r), g_(g), b_(b) {};
};

class RectComponent : public Component {
public:
	RectComponent(class Actor* owner, int drawOrder = 100);
	RectComponent(class Actor* owner, Color color, int drawOrder = 100);
	~RectComponent();

	void draw(SDL_Renderer* renderer);
	bool isVisible() { return isVisible_; }

	int getDrawOrder() { return drawOrder_; }
	const Coord2& getPosition() { return position_; }
	void setPosition(Coord2 position) { position_ = position; }
	const Coord2& getSize() { return size_; }
	void setSize(Coord2 size) { size_ = size; }

	Color color_;

private:
	Coord2 size_;
	Coord2 position_;
	int drawOrder_;
	bool isVisible_ = true;
	int opacity_ = 255;
};
