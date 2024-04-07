#include "Texture.h"
#include <SDL.h>

Texture::Texture()
{
	texture_ = nullptr;
	width_ = 0;
	height_ = 0;
}

Texture::~Texture()
{
	//Deallocate
	free();
}

void Texture::free()
{
	//Free texture if it exists
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
		width_ = 0;
		height_ = 0;
	}
}

void Texture::render(SDL_Renderer* renderer, int x, int y)
{
	if (texture_)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, width_, height_ };
		SDL_RenderCopy(renderer, texture_, NULL, &renderQuad);
	}
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface == nullptr)
	{
		SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		texture_ = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (texture_ == NULL)
		{
			SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			width_ = textSurface->w;
			height_ = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return texture_ != nullptr;
}
