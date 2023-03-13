#pragma once

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