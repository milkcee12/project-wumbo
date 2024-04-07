#pragma once
#include "Actor.h"
#include <queue>
#include <string>
#include "Game.h"
#include <SDL_mixer.h>

class NoteSpawner : public Actor
{
public:
	NoteSpawner(Game* game);
	~NoteSpawner();

	void onUpdate(float deltaTime) override;
	void onInput(const Uint8* keyState, const char* buttonState, const char* currButton) override;

	void loadNotesFromCsv(std::string filename);

	std::queue<class MusicNote*> waitingNotes_;
	std::vector<class MusicNote*> activeNotes_;
	float timer_ = -1.0f;

	Mix_Chunk* chunk_ = nullptr;
	bool buttonStrummed_[Game::NUM_BUTTONS];
	bool buttonPressed_[Game::NUM_BUTTONS];
	bool soundStarted_ = false;

	int frequencyLookup_[88];
	int buttonFrequencies_[Game::NUM_BUTTONS];

	std::string song = "SomeoneElse";
	const int TICKS_PER_SECOND = 760;

	int lastPlayed = 0;

	/** Ticks per second (tps):
	* Mary Had a Little Lamb - 352 tps
	* All of Me - 960 tps
	* Someone Else - 760 tps
	*/
};
