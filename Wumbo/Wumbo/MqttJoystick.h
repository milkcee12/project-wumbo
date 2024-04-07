#pragma once
#include <mosquittopp.h>
#include <SDL.h>
#include "Game.h"

class MqttJoystick : public mosqpp::mosquittopp
{
public:
	MqttJoystick(const char* id, Game* game);

	char* data = nullptr;
	Game* game_ = nullptr;

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_subscribe(int mid, int qos_count, const int* granted_qos);
	void on_message(const struct mosquitto_message* msg);
};

MqttJoystick::MqttJoystick(const char* id, Game* game) : mosqpp::mosquittopp(id)
{
	game_ = game;
}

void MqttJoystick::on_connect(int rc)
{
	if (rc) {
		SDL_Log("Not Connected");
	}
	else {
		SDL_Log("Connected");
		subscribe(NULL, "wumbo/joystick", 0);
	}
}

void MqttJoystick::on_disconnect(int rc)
{
	//run = rc;
}

void MqttJoystick::on_subscribe(int mid, int qos_count, const int* granted_qos)
{

}

void MqttJoystick::on_message(const struct mosquitto_message* msg) {
	data = (char*)msg->payload;
	SDL_Log("%s", data);
	game_->currButton = data;
	game_->refresh = game_->REFRESH_TIME;
}