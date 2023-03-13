#pragma once

#include "Enums.h"

struct Ammo {
	float cord_x = 0;
	float cord_y = 0;
	int sprite_x = 7;
	int sprite_y = 7;
	Sprite* sprite{};

	float unit_vector_x = 0;
	float unit_vector_y = 0;

	float speed_x = 0;
	float speed_y = 0;
	int basicSpeed = 12;
};

struct Platform
{
	int cord_x = 0;
	int cord_y = 0;
	int sprite_x = 80;
	int sprite_y = 15;
	Sprite* sprite{};

	int id = NULL;

	PlatformType type = PlatformType::REGULAR;
	int jumpBoost = 0;
};

struct Enemy
{
	int cord_x = 0;
	int cord_y = 0;
	int sprite_x = 0;
	int sprite_y = 0;

	bool wasAutoShooted = false;

	Sprite* sprite = NULL;
};

struct Ability
{
	int cord_x;
	int cord_y;
	int sprite_x = 40;
	int sprite_y = 40;
	Sprite* sprite = NULL;

	AbilityType type;
	bool isAlive = true;
};

struct Background
{
	int qnt_x = 0;
	int qnt_y = 0;
	int sprite_x = 0;
	int sprite_y = 0;
	Sprite* sprite = NULL;
};