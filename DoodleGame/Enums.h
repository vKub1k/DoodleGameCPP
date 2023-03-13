#pragma once

enum class HeroMoveState {
	LEFT,
	IDLE,
	RIGHT,
	COUNT
};

enum class AbilityType {
	AUTO_SHOOT,
	//JETPACK,
	//ROCKET,
	//SHIELD,
	//SLOW_MOTION,
	COUNT
};

enum class PlatformType {
	REGULAR,
	BOOST,
	ENEMY,
	HOLE,
	//MOVE,
	//ONETOUCH,
	COUNT
};