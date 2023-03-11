#ifndef MYSTRUCTURES_H
#define MYSTRUCTURES_H
#include "Enums.h"

struct Assets {
	Sprite* spritePlarfotmReg;
	Sprite* spritePlarfotmBoost;
	Sprite* spritePlarfotmEnemy;
	Sprite* spriteNpc;
	Sprite* spriteAmmo;
	Sprite* spriteGG;
	Sprite** numbers = new Sprite * [10];
	Sprite** abilities = new Sprite * [1];
};

struct Ammo {
	float cord_x = 0;
	float cord_y = 0;
	const int sprite_x = 7;
	const int sprite_y = 7;

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
	const int sprite_x = 80;
	const int sprite_y = 15;

	int id = NULL;

	PlatformType type = PlatformType::REGULAR;
	int jumpBoost = 8;
	Sprite* sprite{};
};

struct WorldParams {
	int platformDeleted = 0;
	int traveledDistance = 0;

	int bottomOffset = 100;
	int cameraYLimit = 200;

	bool doExit = false;

	Sprite** platformDeletedUi;
	int platformDeletedUiLenght = 1;
	Sprite** traveledDistanceUi;
	int traveledDistanceUiLenght = 1;

	int fpsLimiter = 90;
	int targetFrameDelay = 1000 / fpsLimiter;

	bool isDebug = false;

	unsigned int prevTickCounter = 0;
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
	AbilityType type;
	bool isAlive = true;
	int cord_x;
	int cord_y;
	const int sprite_x = 40;
	const int sprite_y = 40;
	Sprite* sprite = NULL;
};

struct Background
{
	int qnt_x = 0;
	int qnt_y = 0;
	int sprite_x = 0;
	int sprite_y = 0;
	Sprite* sprite = NULL;
};

#endif