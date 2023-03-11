#include "Framework.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <list> 
#include <random>
#include <math.h>
#include <cmath>
#include <chrono>
#include <thread>

#include "Timer.h"
#include "MyStructures.h"
#include "Hero.h"
#include "Enums.h"


using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

namespace fs = std::filesystem;

class Game : public Framework {

public:
	int WindowWidth, WindowHeight;
	
	Assets res;
	Hero mainHero;
	WorldParams worldParams;
	

	Game(int windowWidth, int windowHeight)
	{
		WindowWidth = windowWidth;
		WindowHeight = windowHeight;
	}

	//auto it = std::find(l.begin(), l.end(), 16);
	list<Platform> Platforms;
	list<Platform>::iterator PlatformsItr;

	list<Ammo> Ammos;
	list<Ammo>::iterator AmmosItr;

	list<Enemy> Enemys;
	list<Enemy>::iterator EnemysItr;

	list<Ability> Abilitys;
	list<Ability>::iterator AbilitysItr;

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

		//numbers sprite load
		for (int i = 0; i < 10; i++)
		{
			string relativePath = ".\\data\\nums\\";
			relativePath += to_string(i);
			relativePath += ".png";
			res.numbers[i] = createSprite(absPath(relativePath));
		}

		//start params for scoreboards
		worldParams.platformDeletedUi = new Sprite * [1];
		worldParams.platformDeletedUi[0] = res.numbers[0];
		worldParams.traveledDistanceUi = new Sprite * [1];
		worldParams.traveledDistanceUi[0] = res.numbers[0];

		//res load npc sprite
		res.spriteNpc = createSprite(absPath(".\\data\\npc.png"));

		//res load platforms sprite
		res.spritePlarfotmReg = createSprite(absPath(".\\data\\p_regular.png"));
		res.spritePlarfotmBoost = createSprite(absPath(".\\data\\p_boost.png"));
		res.spritePlarfotmEnemy = createSprite(absPath(".\\data\\p_enemy.png"));

		//res load ammo sprite
		res.spriteAmmo = createSprite(absPath(".\\data\\ammo.png"));

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
		sleep_for(seconds(4));
	}

	void MyClose()
	{
		worldParams.doExit = true;
	}

	bool isRectCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
	{
		return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
	}

	void calculateUiNumber(Sprite**& tab, int& lenght, int num)
	{
		int flag = to_string(num).size();
		tab = new Sprite * [flag];
		lenght = flag;
		do 
		{
			flag--;
			int numId = num % 10;
			tab[flag] = res.numbers[numId];
			num /= 10;
		} while (num != 0);
	}

	void drawUiNumber(Sprite** nums, int length,  int offsetX, int offsetY)
	{
		for (int i = length - 1; i >= 0; i--)
		{
			drawSprite(nums[i], offsetX + i * 22, offsetY);
		}
	}

	void Render()
	{
		//render
		drawTestBackground();
		//render platforms
		for (Platform& p : Platforms)
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
		//render enemy
		if (Enemys.size() > 0)
		{
			for (Enemy& e : Enemys)
			{
				drawSprite(e.sprite, e.cord_x, e.cord_y);
			}
		}
		//render hero
		drawSprite(mainHero.sprite, mainHero.cord_x, mainHero.cord_y);

		//render ui
		drawUiNumber(worldParams.traveledDistanceUi, worldParams.traveledDistanceUiLenght, 6, 6);
		drawUiNumber(worldParams.platformDeletedUi, worldParams.platformDeletedUiLenght, 6, 44);
	}

	virtual bool Tick()
	{
		//vertical logic
		if (mainHero.cord_y <= worldParams.cameraYLimit && mainHero.verticalCurrentMovementSpeed < 0)
		{
			//world scroll
			worldParams.traveledDistance -= mainHero.verticalCurrentMovementSpeed;
			calculateUiNumber(worldParams.traveledDistanceUi, worldParams.traveledDistanceUiLenght, worldParams.traveledDistance);
			//scroll platforms
			for (Platform &p : Platforms)
			{
				p.cord_y -= mainHero.verticalCurrentMovementSpeed;
			}
			//scroll ammo
			if (Ammos.size() > 0)
			{
				for (Ammo& a : Ammos)
				{
					a.cord_y -= mainHero.verticalCurrentMovementSpeed;
				}
			}
			//scroll enemys
			if (Enemys.size() > 0)
			{
				for (Enemy& e : Enemys)
				{
					e.cord_y -= mainHero.verticalCurrentMovementSpeed;
				}
			}
		}
		else
		{
			mainHero.cord_y += mainHero.verticalCurrentMovementSpeed;
		}




		//horizontal logic
		switch (mainHero.moveState)
		{
		case HeroMoveState::LEFT:
			mainHero.cord_x -= mainHero.horizontalMovementSpeed;
			break;
		case HeroMoveState::IDLE:
			break;
		case HeroMoveState::RIGHT:
			mainHero.cord_x += mainHero.horizontalMovementSpeed;
			break;
		case HeroMoveState::COUNT:
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

		//ammo teleport on edge touch
		if (Ammos.size() > 0)
		{
			for (Ammo& a : Ammos)
			{
				int a_x = round(a.cord_x);
				if (a_x < 0 - a.sprite_x)
				{
					a.cord_x = WindowWidth;
				}
				else if (WindowWidth < a_x)
				{
					a.cord_x = 0 - a.sprite_x;
				}
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
					}
					heroJump(p.jumpBoost);
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
					spawnPlatform();
					calculateUiNumber(worldParams.platformDeletedUi, worldParams.platformDeletedUiLenght, worldParams.platformDeleted);
					break;
				}
				else
				{
					break;
				}
			}
		}

		//platform x garbage collector collision and spawning new platforms
		if (Ammos.size() > 0)
		{
			AmmosItr = Ammos.begin();
			for (AmmosItr; AmmosItr != Ammos.end(); AmmosItr++)
			{
				if (*&AmmosItr->cord_y > WindowHeight || *&AmmosItr->cord_y < 0 - *&AmmosItr->sprite_y)
				{
					Ammos.erase(AmmosItr);
					break;
				}
			}
		}

		//platform x garbage collector collision and spawning new platforms
		if (Enemys.size() > 0)
		{
			EnemysItr = Enemys.begin();
			for (EnemysItr; EnemysItr != Enemys.end(); EnemysItr++)
			{
				if (*&EnemysItr->cord_y > WindowHeight)
				{
					Enemys.erase(EnemysItr);
					break;
				}
				else
				{
					break;
				}
			}
		}

		//enemy x ammo collision and enemy kill
		if (Enemys.size() > 0 && Ammos.size() > 0)
		{
			EnemysItr = Enemys.begin();
			bool flag = false;
			for (EnemysItr; EnemysItr != Enemys.end(); EnemysItr++)
			{
				AmmosItr = Ammos.begin();
				for (AmmosItr; AmmosItr != Ammos.end(); AmmosItr++)
				{
					if (isRectCollision(
						*&EnemysItr->cord_x, *&EnemysItr->cord_y, *&EnemysItr->sprite_x, *&EnemysItr->sprite_y,
						*&AmmosItr->cord_x, *&AmmosItr->cord_y, *&AmmosItr->sprite_x, *&AmmosItr->sprite_y
					))
					{
						flag = true;
						break;
					}
				}
				if (flag)
				{
					break;
				}
			}
			if (flag)
			{
				Enemys.erase(EnemysItr);
				Ammos.erase(AmmosItr);
			}
		}

		//render all sprites
		Render();

		//all close cases
		//player x enemy top collision and enemy kill
		if (mainHero.verticalCurrentMovementSpeed >= 0 && mainHero.isCollisionOn)
		{
			EnemysItr = Enemys.begin();
			for (EnemysItr; EnemysItr != Enemys.end(); EnemysItr++)
			{
				if (isRectCollision(*&EnemysItr->cord_x, *&EnemysItr->cord_y, *&EnemysItr->sprite_x, *&EnemysItr->sprite_y,
					mainHero.cord_x, mainHero.cord_y + mainHero.collisionOffset_y, mainHero.sprite_x, mainHero.sprite_y - mainHero.collisionOffset_y))
				{
					Enemys.erase(EnemysItr);
					heroJump(-14);
					break;
				}
			}
		}
		//player x enemy collision and game end
		else if (!Enemys.empty())
		{
			for (Enemy& e : Enemys)
			{
				if (isRectCollision(
					e.cord_x, e.cord_y, e.sprite_x, e.sprite_y,
					mainHero.cord_x, mainHero.cord_y, mainHero.sprite_x, mainHero.sprite_y
				))
				{
					MyClose();
				}
			}
		}
		
		//player x garbage collector collision and game end
		if (mainHero.cord_y + mainHero.collisionOffset_y > WindowHeight)
		{
			MyClose();
		}

		return worldParams.doExit;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		mainHero.mouse_x = x;
		mainHero.mouse_y = y;
		//mainHero.mouse_rel_x = xrelative;
		//mainHero.mouse_rel_y = yrelative;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		switch (button)
		{
		case FRMouseButton::LEFT:
			if (!isReleased)
			{
				spawnAmmo();
			}
			break;
		case FRMouseButton::MIDDLE:
			break;
		case FRMouseButton::RIGHT:
			break;
		default:
			break;
		}
	}

	void spawnEnemy(int x, int y)
	{
		Enemy dummyEnemy;

		dummyEnemy.sprite = res.spriteNpc;
		getSpriteSize(dummyEnemy.sprite, dummyEnemy.sprite_x, dummyEnemy.sprite_y);

		//center enemy on platform
		dummyEnemy.cord_x = x + 5;
		dummyEnemy.cord_y = y - dummyEnemy.sprite_y - 5;

		Enemys.push_back(dummyEnemy);

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
			dummyPlatform.id = PlatformsItr->id++;
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
				spawnEnemy(dummyPlatform.cord_x, dummyPlatform.cord_y);
			}
		}
		else
		{
			dummyPlatform.type = pType;
		}

		switch (dummyPlatform.type)
		{
		case PlatformType::REGULAR:
			dummyPlatform.sprite = res.spritePlarfotmReg;
			dummyPlatform.jumpBoost = -8;
			break;
		case PlatformType::BOOST:
			dummyPlatform.sprite = res.spritePlarfotmBoost;
			dummyPlatform.jumpBoost = -15;
			break;
		case PlatformType::ENEMY:
			dummyPlatform.sprite = res.spritePlarfotmEnemy;
			dummyPlatform.jumpBoost = -8;
			break;
		default:
			Log("!!!Error on plarform type choosing!!!");
			break;
		}

		Platforms.push_back(dummyPlatform);
	}

	void spawnAmmo()
	{
		Ammo ammoDummy;
		ammoDummy.sprite = res.spriteAmmo;

		ammoDummy.cord_x = mainHero.cord_x + mainHero.bulletOffset;
		ammoDummy.cord_y = mainHero.cord_y + mainHero.bulletOffset;
		float v2x = (mainHero.mouse_x - ammoDummy.cord_x);
		float v2y = (mainHero.mouse_y - ammoDummy.cord_y);
		float magn = sqrt((v2x * v2x) + (v2y * v2y));
		ammoDummy.unit_vector_x = v2x / magn;
		ammoDummy.unit_vector_y = v2y / magn;
		ammoDummy.speed_x = ammoDummy.unit_vector_x * ammoDummy.basicSpeed;
		ammoDummy.speed_y = ammoDummy.unit_vector_y * ammoDummy.basicSpeed;

		getSpriteSize(ammoDummy.sprite, ammoDummy.sprite_x, ammoDummy.sprite_y);

		Ammos.push_back(ammoDummy);
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
		run(new Game(w, h));
	}

	return 0;
}