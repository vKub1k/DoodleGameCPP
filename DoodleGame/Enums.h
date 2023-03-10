#pragma once

enum class HeroMoveState {
	LEFT,
	IDLE,
	RIGHT,
	COUNT
};

enum class AbilityType {
	NONE,
	AUTO_SHOOT,
	HORIZONTAL_SPEED_UP,
	JETPACK,
	COUNT
};

enum class PlatformType {
	REGULAR,
	BOOST,
	ENEMY,
	COUNT
};