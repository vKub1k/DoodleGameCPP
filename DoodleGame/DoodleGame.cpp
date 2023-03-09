#include "Framework.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <list> 
#include <random>


using namespace std;
namespace fs = std::filesystem;

class MyFramework : public Framework {

public:
	int WindowWidth, WindowHeight;
	struct GarbageCollector {
		int x1;
		int y1;
		int w1;
		int h1;
	}garbageCollector{};

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
	struct Ammo {
		int cord_x = 0;
		int cord_y = 0;
		int sprite_x = 0;
		int sprite_y = 0;

		Sprite* sprite = NULL;

		int speed_x = 0;
		int speed_y = 0;

		int basicSpeed = 12;
	};

	struct Hero{
		Sprite* sprite = NULL;

		int cord_x = 0;
		int cord_y = 0;

		int sprite_x = 0;
		int sprite_y = 0;

		const int collisionOffset_y = 75;
		bool isCollisionOn = true;

		int teleportOffset = 0;

		HeroMoveState moveState = HeroMoveState::IDLE;
		int horizontalMovementSpeed = 3;
		bool leftPressed = false;
		bool rightPressed = false;

		int verticalBasicMovementSpeed = -8;
		int verticalCurrentMovementSpeed = 0;
		int verticalBasicTickTimer = 8;
		int verticalCurrentTickTimer = 0;

		AbilityType activeAbility = AbilityType::NONE;
		int activeAbilityTimer = 0;

		int biggestPlatformId = -1;

		int mouse_x = 0;
		int mouse_y = 0;
		int mouse_rel_x = 0;
		int mouse_rel_y = 0;

		int bulletOffset = 0;
	}mainHero;

	struct WorldParams{
		int platformDeleted = 0;
		int traveledDistance = 0;

		int bottomOffset = 100;
		int cameraYLimit = 200;

		bool doExit = false;
	}worldParams;

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

	//auto it = std::find(l.begin(), l.end(), 16);
	list<Platform> Platforms;
	list<Platform>::iterator PlatformsItr;

	list<Ammo> Ammos;
	list<Ammo>::iterator AmmoItr;

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

	template <class T>
	void Log(T var)
	{
		cout << var << endl;
	}

	virtual bool Init()
	{
		//main hero sprite and params init
		mainHero.sprite = createSprite(absPath(".\\data\\hero.png"));
		getSpriteSize(mainHero.sprite, mainHero.sprite_x, mainHero.sprite_y);
		mainHero.teleportOffset = mainHero.sprite_x - 2;
		//set start position for hero
		mainHero.cord_x = WindowWidth / 2 - mainHero.sprite_x / 2;
		mainHero.cord_y = WindowHeight - mainHero.sprite_y - worldParams.bottomOffset;
		//hero start falling down
		mainHero.verticalCurrentMovementSpeed = 0;
		mainHero.verticalCurrentTickTimer = mainHero.verticalBasicTickTimer;
		//hero basic params
		mainHero.bulletOffset = mainHero.sprite_x / 2;

		//garbage collector
		garbageCollector.x1 = 0;
		garbageCollector.y1 = WindowHeight + 14;
		garbageCollector.w1 = WindowWidth;
		garbageCollector.h1 = 100;

		//set seed for random from current time
		srand((unsigned)time(NULL));

		//spawn world start platforms
		if (WindowWidth > 600)
		{
			for (int i = 0; i < WindowWidth / 400; i++)
			{
				startWorldCtr();
			}
		}
		else
		{
			startWorldCtr();
		}
		
		return true;
	}

	virtual void Close()
	{
		worldParams.doExit = true;
	}

	bool isRectCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
	{
		return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
	}

	virtual bool Tick()
	{
		//render
		drawTestBackground();
		//render platforms
		for (Platform &p : Platforms)
		{
			drawSprite(p.sprite, p.cord_x, p.cord_y);
		}
		//render ammo
		if (Ammos.size() > 0)
		{
			for (Ammo& a : Ammos)
			{
				drawSprite(a.sprite, a.cord_x, a.cord_y);
			}
		}
		//render hero
		drawSprite(mainHero.sprite, mainHero.cord_x, mainHero.cord_y);

		//vertical logic
		if (mainHero.cord_y <= worldParams.cameraYLimit && mainHero.verticalCurrentMovementSpeed < 0)
		{
			for (Platform &p : Platforms)
			{
				p.cord_y -= mainHero.verticalCurrentMovementSpeed;
			}
		}
		else
		{
			mainHero.cord_y += mainHero.verticalCurrentMovementSpeed;
		}

		//horizontal logic
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

		//hero teleport on edge touch
		if (mainHero.cord_x < 0 - mainHero.teleportOffset)
		{
			mainHero.cord_x = WindowWidth - (mainHero.sprite_x - mainHero.teleportOffset);
		}
		else if (mainHero.cord_x > WindowWidth - (mainHero.sprite_x - mainHero.teleportOffset))
		{
			mainHero.cord_x = 0 - mainHero.teleportOffset;
		}

		//logic with bullets
		if (Ammos.size() > 0)
		{
			for (Ammo& a : Ammos)
			{
				a.cord_x += a.speed_x;
				a.cord_y += a.speed_y;
			}
		}

		//player x platform collision
		if (mainHero.verticalCurrentMovementSpeed >= 0 && mainHero.isCollisionOn)
		{
			for (Platform& p : Platforms)
			{
				if (isRectCollision(p.cord_x, p.cord_y, p.sprite_x, p.sprite_y,
					mainHero.cord_x, mainHero.cord_y + mainHero.collisionOffset_y, mainHero.sprite_x, mainHero.sprite_y - mainHero.collisionOffset_y))
				{
					if (mainHero.biggestPlatformId < p.id)
					{
						mainHero.biggestPlatformId = p.id;
						Log("New ID: ");
						Log(mainHero.biggestPlatformId);
					}
					heroJump(p.jumpBoost);
					Log("Do jump!");
				}
			}
		}

		//platform x garbage collector collision and spawning new platforms
		if (Platforms.size() > 0)
		{
			PlatformsItr = Platforms.begin();
			for (PlatformsItr; PlatformsItr != Platforms.end(); PlatformsItr++)
			{
				if (*&PlatformsItr->cord_y > WindowHeight)
				{
					Platforms.erase(PlatformsItr);
					worldParams.platformDeleted++;
					PlatformsItr;
					spawnPlatform();
					Log("P destr");
					Log(worldParams.platformDeleted);
					break;
				}
				else
				{
					break;
				}
			}
		}
		
		//player x garbage collector collision and game end
		if (isRectCollision(
			garbageCollector.x1, garbageCollector.y1, garbageCollector.w1, garbageCollector.h1,
			mainHero.cord_x, mainHero.cord_y + mainHero.collisionOffset_y,
			mainHero.sprite_x, mainHero.sprite_y - mainHero.collisionOffset_y
		))
		{
			worldParams.doExit = true;
		}

		return worldParams.doExit;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		mainHero.mouse_x = x;
		mainHero.mouse_y = y;
		mainHero.mouse_rel_x = xrelative;
		mainHero.mouse_rel_y = yrelative;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		switch (button)
		{
		case FRMouseButton::LEFT:
			spawnAmmo();
			break;
		case FRMouseButton::MIDDLE:
			break;
		case FRMouseButton::RIGHT:
			break;
		default:
			break;
		}
	}

	void spawnPlatform(bool isRandomPosition = true, int x = 0, int y = 0,
		bool isRandomType = true,PlatformType pType = PlatformType::REGULAR)
	{
		Platform dummyPlatform;

		//id inc
		if (Platforms.size() > 0)
		{
			PlatformsItr = Platforms.end();
			PlatformsItr--;
			dummyPlatform.id = PlatformsItr->id + 1;
		}
		else
		{
			dummyPlatform.id = 0;
		}

		//position
		if (isRandomPosition)
		{
			dummyPlatform.cord_x = rand() % WindowWidth;
			if (dummyPlatform.cord_x > WindowWidth - 80)
			{
				dummyPlatform.cord_x -= 80;
			}
			dummyPlatform.cord_y = -15;
		}
		else
		{
			dummyPlatform.cord_x = x;
			dummyPlatform.cord_y = y;
		}

		if (isRandomType)
		{
			int randValue = rand() % 100 + 1;
			if (randValue <= 70)
			{
				dummyPlatform.type = PlatformType::REGULAR;
			}
			else if (randValue <= 90)
			{
				dummyPlatform.type = PlatformType::BOOST;
			}
			else
			{
				dummyPlatform.type = PlatformType::ENEMY;
			}
		}
		else
		{
			dummyPlatform.type = pType;
		}

		switch (dummyPlatform.type)
		{
		case PlatformType::REGULAR:
			dummyPlatform.sprite = createSprite(absPath(".\\data\\p_regular.png"));
			dummyPlatform.jumpBoost = -8;
			break;
		case PlatformType::BOOST:
			dummyPlatform.sprite = createSprite(absPath(".\\data\\p_boost.png"));
			dummyPlatform.jumpBoost = -15;
			break;
		case PlatformType::ENEMY:
			dummyPlatform.sprite = createSprite(absPath(".\\data\\p_swamp.png"));
			dummyPlatform.jumpBoost = -8;
			break;
		default:
			Log("!!!Error on plarform type choosing!!!");
			break;
		}

		Platforms.push_back(dummyPlatform);
		Log("Platform spawned.");
	}

	void spawnAmmo()
	{
		Ammo ammoDummy;
		ammoDummy.cord_x = mainHero.cord_x + mainHero.bulletOffset;
		ammoDummy.cord_y = mainHero.cord_y + mainHero.bulletOffset;
		ammoDummy.speed_x = (mainHero.mouse_x - ammoDummy.cord_x) / ammoDummy.basicSpeed;
		ammoDummy.speed_y = (mainHero.mouse_y - ammoDummy.cord_y) / ammoDummy.basicSpeed;
		ammoDummy.sprite = createSprite(absPath(".\\data\\ammo.png"));
		getSpriteSize(ammoDummy.sprite, ammoDummy.sprite_x, ammoDummy.sprite_y);

		Ammos.push_back(ammoDummy);
		Log("Ammo spawned.");
	}

	void startWorldCtr()
	{
		spawnPlatform(false, WindowWidth / 2, WindowHeight - worldParams.bottomOffset + 50, false, PlatformType::REGULAR);
		int y = worldParams.bottomOffset - 100;
		while (WindowHeight - y > 0)
		{
			spawnPlatform(false, WindowWidth / 2 - 150, WindowHeight - y, false, PlatformType::REGULAR);
			y += 100;
		}
	}

	void heroJump(int jumpBoost)
	{
		mainHero.verticalCurrentTickTimer = mainHero.verticalBasicTickTimer;
		mainHero.verticalCurrentMovementSpeed = jumpBoost;
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
			worldParams.doExit = true;
			break;
		case FRKey::UP:
			//heroJump(mainHero.verticalBasicMovementSpeed);
			//spawnPlatform();
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
	int w = 600, h = 800;

	/*cout << "Input window Width (recommended 600): ";
	getline(std::cin, input);

	while (!tryParse(input, w))
	{
		std::cout << "Bad entry. Enter a NUMBER (recommended 600): ";
		getline(std::cin, input);
	}

	cout << "Input window Height (recommended 800): ";
	getline(std::cin, input);

	while (!tryParse(input, h))
	{
		std::cout << "Bad entry. Enter a NUMBER (recommended 800): ";
		getline(std::cin, input);
	}*/

	while (true)
	{
		run(new MyFramework(w, h));
	}

	return 0;
}