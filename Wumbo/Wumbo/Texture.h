#pragma once
#include <SDL_image.h>
#include <string>
#include <SDL_ttf.h>
#include <SDL.h>

class Texture
{
public:
	Texture();
	~Texture();
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer);
	void free();
	void render(SDL_Renderer* renderer, int x, int y);
	int getWidth() { return width_; };
	int getHeight() { return height_; };

private:
	SDL_Texture* texture_;
	int width_;
	int height_;
};
