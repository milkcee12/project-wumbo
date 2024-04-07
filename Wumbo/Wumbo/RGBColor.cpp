#include "RGBColor.h"

const RGBColor RGBColor::White(255, 255, 255);

RGBColor::RGBColor() {
	r_ = 0;
	g_ = 0;
	b_ = 0;
}

RGBColor::RGBColor(Uint8 r, Uint8 g, Uint8 b)
	: r_(r), g_(g), b_(b)
{}
