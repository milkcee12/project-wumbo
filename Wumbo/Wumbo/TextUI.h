#include <SDL_ttf.h>
#include <string>
#include "Actor.h"
#include "RGBColor.h"

class TextUI : public Actor
{
public:
	TextUI(class Game* game);
	TextUI(class Game* game, float lifetime, bool bigText);
	~TextUI();

	void onUpdate(float deltaTime) override;

	void writeMessage(std::string message, RGBColor color = RGBColor::White);
	void clearMessage();
	void render();

	float lifetime_ = FLT_MIN;

private:
	TTF_Font* font_ = nullptr;
	class Texture* texture_;
};
