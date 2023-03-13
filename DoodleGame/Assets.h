#pragma once

#include "Framework.h"

struct Assets {
	Sprite* spritePlarfotmReg;
	Sprite* spritePlarfotmBoost;
	Sprite* spritePlarfotmEnemy;
	Sprite* spritePlarfotmHole;
	Sprite* spritePlarfotmMove;
	Sprite* spritePlarfotmOneTouch;

	Sprite* spriteNpc;
	Sprite* spriteAmmo;
	Sprite* spriteGG;
	Sprite* spriteWhiteHole;
	Sprite* spriteBlackHole;
	Sprite** numbers = new Sprite * [10];
	Sprite** abilities = new Sprite * [1];
};