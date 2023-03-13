#pragma once

#include "Enums.h"

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
	int horizontal_move = 0;
};