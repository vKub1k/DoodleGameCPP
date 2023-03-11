#ifndef MYSTRUCTURES_H
#define MYSTRUCTURES_H
#include "Enums.h"

struct Assets {
	Sprite* spritePlarfotmReg;
	Sprite* spritePlarfotmBoost;
	Sprite* spritePlarfotmEnemy;
	Sprite* spriteNpc;
	Sprite* spriteAmmo;
	Sprite** numbers = new Sprite * [10];
};

struct GarbageCollector {
	int x1;
	int y1;
	int w1;
	int h1;
};

struct Ammo {
	float cord_x = 0;
	float cord_y = 0;
	int sprite_x = 0;
	int sprite_y = 0;

	Sprite* sprite = NULL;

	float unit_vector_x = 0;
	float unit_vector_y = 0;

	float speed_x = 0;
	float speed_y = 0;
	int basicSpeed = 4;
};

struct WorldParams {
	int platformDeleted = 0;
	int traveledDistance = 0;

	int bottomOffset = 100;
	int cameraYLimit = 200;

	bool doExit = false;
};

struct Platform
{
	int id = NULL;

	PlatformType type = PlatformType::REGULAR;
	int jumpBoost = 8;

	int cord_x = 0;
	int cord_y = 0;
	const int sprite_x = 80;
	const int sprite_y = 15;
	Sprite* sprite{};
};

struct Enemy
{
	int cord_x = 0;
	int cord_y = 0;

	Sprite* sprite = NULL;
	int sprite_x = 0;
	int sprite_y = 0;
};

struct Ability
{
	AbilityType type;
	bool isAlive = true;
	int cord_x;
	int cord_y;
	Sprite* sprite = NULL;
};

#endif