#pragma once

struct Enemy
{
	int cord_x = 0;
	int cord_y = 0;
	int sprite_x = 0;
	int sprite_y = 0;

	bool wasAutoShooted = false;

	Sprite* sprite = NULL;
};