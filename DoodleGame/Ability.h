#pragma once

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