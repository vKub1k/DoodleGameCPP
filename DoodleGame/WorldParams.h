#pragma once

#include "Framework.h"

struct WorldParams {
	int platformDeleted = 0;
	Sprite** platformDeletedUi;
	int platformDeletedUiLenght = 1;

	int traveledDistance = 0;
	Sprite** traveledDistanceUi;
	int traveledDistanceUiLenght = 1;

	int bottomOffset = 100;
	int cameraYLimit = 200;

	int fpsLimiter = 90;
	int targetFrameDelay = 1000 / fpsLimiter;

	unsigned int prevTickCounter = 0;

	bool doExit = false;

#if defined(_DEBUG)
	bool isDebug = true;
#else
	bool isDebug = false;
#endif
};