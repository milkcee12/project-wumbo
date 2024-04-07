#pragma once
#include "Actor.h"
#include "Game.h"

class MusicNote : public Actor
{
public:
	MusicNote(Game* game, float start, float duration, int button, int frequency, int pianoNote);
	~MusicNote();

	void onUpdate(float deltaTime) override;
	void passedButton();

	float startTime_;
	float duration_;
	int button_;
	int frequency_;
	int pianoNote_;

	bool published_ = false;
	bool played_ = false;
	bool passed = false;

	const int VELOCITY = Game::WINDOW_H;

private:
	class RectComponent* rectComponent_;
};
