#pragma once

#include "GameObject.h"

class Hole : public GameObject
{
public:
	int field_radius;
	int holeDirection;
	float powerModifier;

	Hole(int x, int y, Sprite* s, int radius, int dir, float mod = 200)
	{
		cord_x = x;
		cord_y = y;
		sprite = s;
		getSpriteSize(s, sprite_x, sprite_y);

		field_radius = radius;
		holeDirection = dir;
		powerModifier = mod;
	}
};