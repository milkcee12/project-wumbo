#pragma once
#include <SDL.h>

class RGBColor {
public:
	Uint8 r_ = 0;
	Uint8 g_ = 0;
	Uint8 b_ = 0;

	RGBColor();
	RGBColor(Uint8 r, Uint8 g, Uint8 b);

	static const RGBColor White;
};

