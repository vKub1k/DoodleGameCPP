#pragma once

enum class HeroMoveState {
	LEFT,
	IDLE,
	RIGHT,
	COUNT
};

enum class AbilityType {
	AUTO_SHOOT,
	//HORIZONTAL_SPEED_UP,
	//JETPACK,
	//SLOW_MOTION,
	COUNT
};

enum class PlatformType {
	REGULAR,
	BOOST,
	ENEMY,
	COUNT
};