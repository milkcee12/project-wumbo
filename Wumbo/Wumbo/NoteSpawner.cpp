#include "NoteSpawner.h"
#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "MusicNote.h"
#include "Button.h"
#include "TextUI.h"
#include <format>
#include "RectComponent.h"
#include "mosquittopp.h"

NoteSpawner::NoteSpawner(Game* game)
	: Actor(game)
{
	song = std::get<std::string>(game_->songMap_[game_->currSong_]);
	TICKS_PER_SECOND = std::get<int>(game_->songMap_[game_->currSong_]);

	std::string csvFile = std::format("C:\\Users\\micha\\Documents2\\_active-classes\\project-wumbo\\Wumbo\\Wumbo\\Assets\\Levels\\{}.csv", song);
	loadNotesFromCsv(csvFile);

	std::string wavFile = std::format("C:\\Users\\micha\\Documents2\\_active-classes\\project-wumbo\\Wumbo\\Wumbo\\Assets\\Audio\\{}.wav", song);
	chunk_ = Mix_LoadWAV(wavFile.c_str());

	buttonStrummed_[0] = false;
	buttonStrummed_[1] = false;
	buttonStrummed_[2] = false;
	buttonStrummed_[3] = false;

	buttonPressed_[0] = false;
	buttonPressed_[1] = false;
	buttonPressed_[2] = false;
	buttonPressed_[3] = false;
}

NoteSpawner::~NoteSpawner()
{
	if (chunk_)
		Mix_FreeChunk(chunk_);
}

void NoteSpawner::onUpdate(float deltaTime)
{
	if (waitingNotes_.empty())
		return;

	timer_ += deltaTime;

	if (timer_ > 1.0f && !soundStarted_)
	{
		SDL_Log("Time to start the song!!");
		soundStarted_ = true;
		int channel = Mix_PlayChannel(-1, chunk_, 0);
		Mix_Volume(channel, MIX_MAX_VOLUME);
	}

	while (!waitingNotes_.empty() && waitingNotes_.front()->startTime_ + 0.12f <= timer_)
	{
		waitingNotes_.front()->setState(ActorState::Active);
		MusicNote* temp = waitingNotes_.front();

		waitingNotes_.pop();
		activeNotes_.push_back(temp);
	}


	for (MusicNote* note : activeNotes_)
	{
		if (note)
		{
			// Publish
			if (!note->published_ && note->intersect(game_->buttons_[note->button_]))
			{
				MusicNote* temp = note;
				note->published_ = true;
				lastPlayed++;

				if (temp)
				{
					for (int i = 0; i < Game::NUM_BUTTONS; i++)
						buttonFrequencies_[i] = frequencyLookup_[temp->pianoNote_ + i - temp->button_];

					std::string publishMessage = std::format("{},{},{},{},{},{},",
						buttonFrequencies_[0],
						buttonFrequencies_[1],
						buttonFrequencies_[2],
						buttonFrequencies_[3],
						static_cast<int>(temp->duration_ * TICKS_PER_SECOND),
						TICKS_PER_SECOND);

					if (game_->mqtt_publish)
					{
						game_->mqtt_publish->publish(nullptr,
							"wumbo/frequency",
							publishMessage.size(),
							publishMessage.c_str());
					}
				}
			}

			// Check button press
			if (!note->played_ && !note->passed &&
				note->intersect(game_->buttons_[note->button_]) &&
				buttonStrummed_[note->button_])
			{
				game_->score_ += 1;
				game_->scoreText_->writeMessage(std::to_string(game_->score_) + "/" + std::to_string(game_->total_));

				// WUMBO COMBO
				if (game_->score_ > 0 && game_->score_ % 20 == 0)
				{
					SDL_Log("COMBO");
					TextUI* WUM = new TextUI(game_, 2.404f, true);
					WUM->writeMessage("WUMBO COMBO!!!", RGBColor(0, 255, 0));
					WUM->setPosition(Coord2(230.0f, 300.0f));

					/*TextUI* COMBO = new TextUI(game_, 2.404f, true);
					int comboAmount = game_->score_ / 20;
					COMBO->writeMessage(comboAmount + "x", RGBColor(255, 255, 255));
					COMBO->setPosition(Coord2(280.0f, 400.0f));*/

					game_->textUis_.push_back(WUM);
					//game_->textUis_.push_back(COMBO);
				}

				note->played_ = true;
			}
			if (note->getPosition().y >= game_->buttons_[0]->getPosition().y)
				note->passedButton();
		}
	}

	// Button press
	for (int i = 0; i < game_->NUM_BUTTONS; i++)
	{
		game_->buttons_[i]->rectComponent_->color_ = buttonPressed_[i] ?
			game_->buttonColorsPressed[i] : game_->buttonColors[i];
	}
}

void NoteSpawner::onInput(const Uint8* keyState, const char* buttonState, const char* currButton)
{
	int buttonNum = currButton != nullptr ? std::atoi(currButton) : -1;
	if (buttonNum < 0)
	{
		buttonStrummed_[0] = keyState[SDL_SCANCODE_Q];
		buttonStrummed_[1] = keyState[SDL_SCANCODE_W];
		buttonStrummed_[2] = keyState[SDL_SCANCODE_E];
		buttonStrummed_[3] = keyState[SDL_SCANCODE_R];
	}
	else
	{
		buttonStrummed_[0] = keyState[SDL_SCANCODE_Q] || buttonNum == 0;
		buttonStrummed_[1] = keyState[SDL_SCANCODE_W] || buttonNum == 1;
		buttonStrummed_[2] = keyState[SDL_SCANCODE_E] || buttonNum == 2;
		buttonStrummed_[3] = keyState[SDL_SCANCODE_R] || buttonNum == 3;
	}

	if (buttonState)
	{
		buttonPressed_[0] = keyState[SDL_SCANCODE_Q] || buttonState[0] == '0';
		buttonPressed_[1] = keyState[SDL_SCANCODE_W] || buttonState[1] == '0';
		buttonPressed_[2] = keyState[SDL_SCANCODE_E] || buttonState[2] == '0';
		buttonPressed_[3] = keyState[SDL_SCANCODE_R] || buttonState[3] == '0';
	}
	else
	{
		buttonPressed_[0] = keyState[SDL_SCANCODE_Q];
		buttonPressed_[1] = keyState[SDL_SCANCODE_W];
		buttonPressed_[2] = keyState[SDL_SCANCODE_E];
		buttonPressed_[3] = keyState[SDL_SCANCODE_R];
	}
}

void NoteSpawner::loadNotesFromCsv(std::string filename)
{
	while (!waitingNotes_.empty())
	{
		waitingNotes_.front()->setState(ActorState::Zombie);
		waitingNotes_.pop();
	}

	std::ifstream input(filename);

	if (input.fail())
	{
		SDL_Log("Failed to read notes csv.");
		return;
	}

	std::string line;
	std::getline(input, line);

	if (input.fail())
	{
		SDL_Log("Failed to skip the first two lines of the csv.");
		return;
	}

	float start;
	float duration;
	int button;
	int frequency;
	int pianoNote;

	while (!input.eof() && !input.fail())
	{
		getline(input, line);
		if (line.empty() || input.fail())
			break;

		std::stringstream lineStream(line);
		std::string value;
		std::vector<std::string> noteLine;
		while (getline(lineStream, value, ','))
		{
			noteLine.push_back(value);
		}

		start = std::stof(noteLine[0]) / TICKS_PER_SECOND;
		duration = (std::stof(noteLine[6]) / TICKS_PER_SECOND);
		button = std::stoi(noteLine[5]);
		frequency = std::stoi(noteLine[4]);
		pianoNote = std::stoi(noteLine[3]);

		//SDL_Log("start %f, duration %f, button %d", start, duration, button);

		MusicNote* note = new MusicNote(game_, start, duration, button, frequency, pianoNote);
		note->setState(ActorState::Paused);
		waitingNotes_.push(note);
	}

	game_->total_ = static_cast<int>(waitingNotes_.size());
	game_->scoreText_->writeMessage(std::to_string(game_->score_) + "/" + std::to_string(game_->total_));

	//SDL_Log("size of notes_ vector is %d", static_cast<int>(waitingNotes_.size()));

	for (int i = 0; i < 88; i++)
	{
		frequencyLookup_[i] = static_cast<int>(std::pow(2, (i - 49) / 12.0f) * 440);
	}
}
