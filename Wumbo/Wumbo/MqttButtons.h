#pragma once
#include <mosquittopp.h>
#include <SDL.h>
#include "Game.h"

class MqttButtons : public mosqpp::mosquittopp
{
public:
	MqttButtons(const char* id, Game* game);

	char* data = nullptr;
	Game* game_ = nullptr;

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_subscribe(int mid, int qos_count, const int* granted_qos);
	void on_message(const struct mosquitto_message* msg);
};

MqttButtons::MqttButtons(const char* id, Game* game) : mosqpp::mosquittopp(id)
{
	game_ = game;
}

void MqttButtons::on_connect(int rc)
{
	if (rc) {
		SDL_Log("Not Connected");
	}
	else {
		SDL_Log("Connected");
		subscribe(NULL, "wumbo/button", 0);
	}
}

void MqttButtons::on_disconnect(int rc)
{

}

void MqttButtons::on_subscribe(int mid, int qos_count, const int* granted_qos)
{

}

void MqttButtons::on_message(const struct mosquitto_message* msg) {
	data = (char*)msg->payload;
	SDL_Log("%s", data);
	game_->buttonState = data;
}