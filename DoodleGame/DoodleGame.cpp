#include "Framework.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

class MyFramework : public Framework {

public:
	int WindowWidth, WindowHeight;

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

	struct Hero{
		Sprite* sprite = NULL;

		int cord_x = 0;
		int cord_y = 0;

		int sprite_x = 0;
		int sprite_y = 0;

		int teleportOffset = 0;

		HeroMoveState moveState = HeroMoveState::IDLE;
		int horizontalMovementSpeed = 2;
		bool leftPressed = false;
		bool rightPressed = false;

		int verticalBasicMovementSpeed = -8;
		int verticalCurrentMovementSpeed = 0;
		int verticalBasicTickTimer = 8;
		int verticalCurrentTickTimer = 0;

		AbilityType activeAbility = AbilityType::NONE;
		int activeAbilityTimer = 0;

	}mainHero;

	struct {
		int platformDeleted = 0;
		int traveledDistance = 0;
		int bottomOffset = 100;
	}worldParams;

	struct Platform
	{
		int jumpBooster;
		int cord_x;
		int cord_y;
		Sprite* sprite = NULL;
	};
	
	struct Monster
	{
		bool isAlive = true;
		int cord_x;
		int cord_y;
		Sprite* sprite = NULL;
	};

	struct Ability
	{
		AbilityType type;
		bool isAlive = true;
		int cord_x;
		int cord_y;
		Sprite* sprite = NULL;
	};
	

	MyFramework(int windowWidth, int windowHeight)
	{
		WindowWidth = windowWidth;
		WindowHeight = windowHeight;
	}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = WindowWidth;
		height = WindowHeight;
		fullscreen = false;
	}

	char* absPath(fs::path relativePath)
	{
		fs::path resultPath = fs::absolute(relativePath);
		string str = resultPath.string();

		int len = str.size();

		char* resultChar = new char[len + 1];
		std::copy(str.begin(), str.end(), resultChar);
		resultChar[len] = '\0';
		cout << "absPath(" << relativePath << ") execution result: " << resultChar << endl;
		return resultChar;
	}

	virtual bool Init()
	{
		//main hero sprite and params init
		mainHero.sprite = createSprite(absPath(".\\data\\hero.png"));
		getSpriteSize(mainHero.sprite, mainHero.sprite_x, mainHero.sprite_y);
		mainHero.teleportOffset = mainHero.sprite_x - 2;

		//center sprite in window
		mainHero.cord_x = WindowWidth / 2 - mainHero.sprite_x / 2;
		mainHero.cord_y = WindowHeight - mainHero.sprite_y - worldParams.bottomOffset;
		
		return true;
	}

	virtual void Close()
	{

	}

	virtual bool Tick()
	{
		//draw
		drawTestBackground();
		drawSprite(mainHero.sprite, mainHero.cord_x, mainHero.cord_y += mainHero.verticalCurrentMovementSpeed);

		//move
		switch (mainHero.moveState)
		{
		case MyFramework::HeroMoveState::LEFT:
			mainHero.cord_x -= mainHero.horizontalMovementSpeed;
			break;
		case MyFramework::HeroMoveState::IDLE:
			break;
		case MyFramework::HeroMoveState::RIGHT:
			mainHero.cord_x += mainHero.horizontalMovementSpeed;
			break;
		case MyFramework::HeroMoveState::COUNT:
			break;
		default:
			break;
		}

		//jump tick logic
		if (--mainHero.verticalCurrentTickTimer == 0)
		{
			mainHero.verticalCurrentMovementSpeed++;
			mainHero.verticalCurrentTickTimer = mainHero.verticalBasicTickTimer;
		}

		//teleport on edge touch
		if (mainHero.cord_x < 0 - mainHero.teleportOffset)
		{
			mainHero.cord_x = WindowWidth - (mainHero.sprite_x - mainHero.teleportOffset);
		}
		else if (mainHero.cord_x > WindowWidth - (mainHero.sprite_x - mainHero.teleportOffset))
		{
			mainHero.cord_x = 0 - mainHero.teleportOffset;
		}

		//player collision

		

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{

	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{

	}

	void heroJump()
	{
		mainHero.verticalCurrentTickTimer = mainHero.verticalBasicTickTimer;
		mainHero.verticalCurrentMovementSpeed = mainHero.verticalBasicMovementSpeed;
	}

	virtual void onKeyPressed(FRKey k)
	{
		switch (k)
		{
		case FRKey::RIGHT:
			mainHero.rightPressed = true;
			if (mainHero.moveState == HeroMoveState::IDLE)
			{
				mainHero.moveState = HeroMoveState::RIGHT;
			}
			break;
		case FRKey::LEFT:
			mainHero.leftPressed = true;
			if (mainHero.moveState == HeroMoveState::IDLE)
			{
				mainHero.moveState = HeroMoveState::LEFT;
			}
			break;
		case FRKey::DOWN:
			break;
		case FRKey::UP:
			heroJump();
			break;
		case FRKey::COUNT:
			break;
		default:
			break;
		}
	}

	virtual void onKeyReleased(FRKey k)
	{
		switch (k)
		{
		case FRKey::RIGHT:
			mainHero.rightPressed = false;
			if (mainHero.moveState == HeroMoveState::RIGHT)
			{
				if (mainHero.leftPressed)
				{
					mainHero.moveState = HeroMoveState::LEFT;
				}
				else
				{
					mainHero.moveState = HeroMoveState::IDLE;
				}
			}
			break;
		case FRKey::LEFT:
			mainHero.leftPressed = false;
			if (mainHero.moveState == HeroMoveState::LEFT)
			{
				if (mainHero.rightPressed)
				{
					mainHero.moveState = HeroMoveState::RIGHT;
				}
				else
				{
					mainHero.moveState = HeroMoveState::IDLE;
				}
			}
			break;
		case FRKey::DOWN:
			break;
		case FRKey::UP:
			break;
		case FRKey::COUNT:
			break;
		default:
			break;
		}
	}

	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}
};

bool tryParse(std::string& input, int& output)
{
	try
	{
		output = std::stoi(input);
	}
	catch (std::invalid_argument)
	{
		return false;
	}

	return true;
}

int main(int argc, char* argv[]) // game -window 800x600
{
	string input;
	int w = 800, h = 600;

	/*cout << "Input window Width (for example 800): ";
	getline(std::cin, input);

	while (!tryParse(input, w))
	{
		std::cout << "Bad entry. Enter a NUMBER (for example 800): ";
		getline(std::cin, input);
	}

	cout << "Input window Height (for example 600): ";
	getline(std::cin, input);

	while (!tryParse(input, h))
	{
		std::cout << "Bad entry. Enter a NUMBER (for example 600): ";
		getline(std::cin, input);
	}*/

	return run(new MyFramework(w, h));
}