#pragma once

#include "Enums.h"

using namespace std;

struct Hero {
	Sprite* sprite = NULL;

	float cord_x = 0;
	float cord_y = 0;

	int sprite_x = 0;
	int sprite_y = 0;

	const int collisionOffset_y = 75;

	int teleportOffset = 0;

	HeroMoveState moveState = HeroMoveState::IDLE;
	int horizontalMovementSpeed = 3;
	bool leftPressed = false;
	bool rightPressed = false;

	int verticalBasicMovementSpeed = -8;
	int verticalCurrentMovementSpeed = 0;
	int verticalBasicTickTimer = 8;
	int verticalCurrentTickTimer = 0;

	int biggestPlatformId = -1;

	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_rel_x = 0;
	int mouse_rel_y = 0;

	int bulletOffset = 0;

	unsigned int jumpCounter = 0;
	int jumpsToSpawnAbility = 50;

	AbilityType activeAbility = AbilityType::COUNT;
	unsigned int ability_tick_stop = 0;

	bool isCollisionOn = true;

	float passive_force_x = 0;
	float passive_force_y = 0;

};