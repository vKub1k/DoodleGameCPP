#include "Framework.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <list> 
#include <random>
#include <cmath>
#include <chrono>
#include <thread>

#include "MyStructures.h"
#include "Hero.h"
#include "Enums.h"
#include "Assets.h"
#include "WorldParams.h"
#include "Hole.h"
#include "Ammo.h"
#include "Ability.h"
#include "Platform.h"
#include "Enemy.h"
#include "Background.h"


using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

namespace fs = std::filesystem;
bool run_again = true;

class game final : public Framework {

public:
	int window_width, window_height;
	
	Assets res;
	Hero main_hero;
	WorldParams world_params;
	Background bg;

	list<Platform> platforms;
	list<Platform>::iterator platforms_itr;

	list<Ammo> ammos;
	list<Ammo>::iterator ammos_itr;

	list<Enemy> enemys;
	list<Enemy>::iterator enemys_itr;

	list<Ability> abilitys;
	list<Ability>::iterator abilitys_itr;

	list<Hole> holes;
	list<Hole>::iterator holes_itr;
	

	game(const int windowWidth, int windowHeight): res(), world_params()
	{
		window_width = windowWidth;
		window_height = windowHeight;
	}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = window_width;
		height = window_height;
		fullscreen = false;
	}

	char* abs_path(fs::path relative_path) const
	{
		fs::path resultPath = fs::absolute(relative_path);
		string str = resultPath.string();

		int len = str.size();

		char* resultChar = new char[len + 1];
		std::copy(str.begin(), str.end(), resultChar);
		resultChar[len] = '\0';
		cout << "absPath(" << relative_path << ") execution result: " << resultChar << endl;
		return resultChar;
	}

	template <class T>
	void Log(T var)
	{
		cout << var << endl;
	}

	void loadSprites(bool isDebug = false)
	{
		string pathStart = ".\\data\\prod\\";
		if (isDebug)
		{
			pathStart = ".\\data\\debug\\";
		}
		res.spriteMainHero = createSprite(abs_path(pathStart + "hero\\hero.png"));
		

		//bg sprite load and cfg
		bg.sprite = createSprite(abs_path(pathStart + "bg.png"));
		saveBackgroundParams();

		//numbers sprite load
		for (int i = 0; i < 10; i++)
		{
			string relativePath = pathStart + "nums\\";
			relativePath += to_string(i);
			relativePath += ".png";
			res.numbers[i] = createSprite(abs_path(relativePath));
		}

		//res load npc sprite
		res.spriteNpc = createSprite(abs_path(pathStart + "npc.png"));

		//res load platforms sprite
		res.spritePlarfotmReg = createSprite(abs_path(pathStart + "p_regular.png"));
		res.spritePlarfotmBoost = createSprite(abs_path(pathStart + "p_boost.png"));
		res.spritePlarfotmEnemy = createSprite(abs_path(pathStart + "p_enemy.png"));
		res.spritePlarfotmHole = createSprite(abs_path(pathStart + "p_hole.png"));
		res.spritePlarfotmMove = createSprite(abs_path(pathStart + "p_move.png"));
		res.spritePlarfotmOneTouch = createSprite(abs_path(pathStart + "p_onetouch.png"));

		//res load ammo sprite
		res.spriteAmmo = createSprite(abs_path(pathStart + "ammo.png"));

		//res load abilities sprites
		res.abilities[0] = createSprite(abs_path(pathStart + "ab\\autoshoot.png"));
		res.abilities[1] = createSprite(abs_path(pathStart + "ab\\jetpack.png"));
		res.abilities[2] = createSprite(abs_path(pathStart + "ab\\rocket.png"));
		res.abilities[3] = createSprite(abs_path(pathStart + "ab\\shield.png"));
		res.abilities[4] = createSprite(abs_path(pathStart + "ab\\slowmotion.png"));

		//res load main hero states with abilities
		res.mainhero_abilities[0] = createSprite(abs_path(pathStart + "hero\\hero_autoshoot.png"));
		res.mainhero_abilities[1] = createSprite(abs_path(pathStart + "hero\\hero_jetpack.png"));
		res.mainhero_abilities[2] = createSprite(abs_path(pathStart + "hero\\hero_rocket.png"));
		res.mainhero_abilities[3] = createSprite(abs_path(pathStart + "hero\\hero_shield.png"));
		res.mainhero_abilities[4] = createSprite(abs_path(pathStart + "hero\\hero_slowmotion.png"));

		//res load gg
		res.spriteGG = createSprite(abs_path(pathStart + "gg.png"));

		//res load holes
		res.spriteWhiteHole = createSprite(abs_path(pathStart + "whitehole.png"));
		res.spriteBlackHole = createSprite(abs_path(pathStart + "blackhole.png"));
	}

	virtual bool Init()
	{
		loadSprites(world_params.isDebug);

		//main hero params init
		main_hero.sprite = res.spriteMainHero;
		getSpriteSize(main_hero.sprite, main_hero.sprite_x, main_hero.sprite_y);
		main_hero.teleportOffset = main_hero.sprite_x - 2;
		//set start position for hero
		main_hero.cord_x = window_width / 2 - main_hero.sprite_x / 2;
		main_hero.cord_y = window_height - main_hero.sprite_y - world_params.bottomOffset;
		//hero start falling down
		main_hero.verticalCurrentMovementSpeed = 0;
		main_hero.verticalCurrentTickTimer = main_hero.verticalBasicTickTimer;
		//hero basic params
		main_hero.bulletOffset = main_hero.sprite_x / 2;

		//start params for scoreboards
		world_params.platformDeletedUi = new Sprite * [1];
		world_params.platformDeletedUi[0] = res.numbers[0];
		world_params.traveledDistanceUi = new Sprite * [1];
		world_params.traveledDistanceUi[0] = res.numbers[0];
		
		//set seed for random from current time
		srand((unsigned)time(NULL));

		//spawn world start platforms
		if (window_width > 600)
		{
			for (int i = 0; i < window_width / 400; i++)
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

	void ShowGG()
	{
		int w, h;
		getSpriteSize(res.spriteGG, w, h);
		drawSprite(res.spriteGG, (window_width / 2) - (w / 2), (window_height / 2) - (h / 2));
	}

	virtual void Close()
	{
		sleep_for(seconds(4));
	}

	void MyClose()
	{
		world_params.doExit = true;
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

	void saveBackgroundParams()
	{
		getSpriteSize(bg.sprite, bg.sprite_x, bg.sprite_y);
		bg.qnt_x = window_width / bg.sprite_x + 1;
		bg.qnt_y = window_height / bg.sprite_y + 1;
	}

	void drawBackground()
	{
		for (int i = 0; i < bg.qnt_y; i++)
		{
			for (int j = 0; j < bg.qnt_x; j++)
			{
				drawSprite(bg.sprite, bg.sprite_x * j, bg.sprite_y * i);
			}
		}
	}

	void Render()
	{
		//render
		if (world_params.isDebug)
		{
			//drawBackground();
		}
		else
		{
			drawBackground();
		}

		//render platforms
		for (Platform& p : platforms)
		{
			drawSprite(p.sprite, p.cord_x, p.cord_y);
		}

		//render ammo
		if (ammos.size() > 0)
		{
			for (Ammo& a : ammos)
			{
				drawSprite(a.sprite, a.cord_x, a.cord_y);
			}
		}

		//render enemy
		if (enemys.size() > 0)
		{
			for (Enemy& e : enemys)
			{
				drawSprite(e.sprite, e.cord_x, e.cord_y);
			}
		}

		//render abilities
		if (abilitys.size() > 0)
		{
			for (Ability& a : abilitys)
			{
				drawSprite(a.sprite, a.cord_x, a.cord_y);
			}
		}

		//scroll holes
		if (!holes.empty())
		{
			for (Hole& h : holes)
			{
				drawSprite(h.sprite, h.cord_x, h.cord_y);
			}
		}
		
		//render hero
		drawSprite(main_hero.sprite, main_hero.cord_x, main_hero.cord_y);

		//render ui
		drawUiNumber(world_params.traveledDistanceUi, world_params.traveledDistanceUiLenght, 6, 6);
		drawUiNumber(world_params.platformDeletedUi, world_params.platformDeletedUiLenght, 6, 44);

		if (main_hero.activeAbility != AbilityType::COUNT)
		{
			drawSprite(res.abilities[(int)main_hero.activeAbility], window_width - 50, window_height - 50);
		}
	}

	void Scroll()
	{
		//world scroll
		world_params.traveledDistance -= main_hero.verticalCurrentMovementSpeed;
		calculateUiNumber(world_params.traveledDistanceUi, world_params.traveledDistanceUiLenght, world_params.traveledDistance);
		//scroll platforms
		for (Platform& p : platforms)
		{
			p.cord_y -= main_hero.verticalCurrentMovementSpeed;
		}
		//scroll ammo
		if (ammos.size() > 0)
		{
			for (Ammo& a : ammos)
			{
				a.cord_y -= main_hero.verticalCurrentMovementSpeed;
			}
		}
		//scroll enemys
		if (enemys.size() > 0)
		{
			for (Enemy& e : enemys)
			{
				e.cord_y -= main_hero.verticalCurrentMovementSpeed;
			}
		}

		//scroll abilities
		if (abilitys.size() > 0)
		{
			for (Ability& a : abilitys)
			{
				a.cord_y -= main_hero.verticalCurrentMovementSpeed;
			}
		}

		//scroll holes
		if (!holes.empty())
		{
			for (Hole& h : holes)
			{
				h.cord_y -= main_hero.verticalCurrentMovementSpeed;
			}
		}
	}

	virtual bool Tick()
	{
		//vertical logic
		if (main_hero.cord_y <= world_params.cameraYLimit && main_hero.verticalCurrentMovementSpeed < 0)
		{
			Scroll();
		}
		else
		{
			main_hero.cord_y += main_hero.verticalCurrentMovementSpeed + main_hero.passive_force_y;
		}

		//horizontal hero movement
		switch (main_hero.moveState)
		{
		case HeroMoveState::LEFT:
			main_hero.cord_x -= main_hero.horizontalMovementSpeed;
			break;
		case HeroMoveState::IDLE:
			break;
		case HeroMoveState::RIGHT:
			main_hero.cord_x += main_hero.horizontalMovementSpeed;
			break;
		case HeroMoveState::COUNT:
			break;
		default:
			break;
		}
		//hero teleport on edge touch
		if (main_hero.cord_x < 0 - main_hero.teleportOffset)
		{
			main_hero.cord_x = window_width - (main_hero.sprite_x - main_hero.teleportOffset);
		}
		else if (main_hero.cord_x > window_width - (main_hero.sprite_x - main_hero.teleportOffset))
		{
			main_hero.cord_x = 0 - main_hero.teleportOffset;
		}

		//jump tick logic
		if (main_hero.activeAbility != AbilityType::JETPACK && main_hero.activeAbility != AbilityType::ROCKET)
		{
			if (--main_hero.verticalCurrentTickTimer == 0)
			{
				main_hero.verticalCurrentMovementSpeed++;
				main_hero.verticalCurrentTickTimer = main_hero.verticalBasicTickTimer;
			}
		}

		//ability check up
		if (main_hero.activeAbility != AbilityType::COUNT)
		{
			switch (main_hero.activeAbility)
			{
			case AbilityType::AUTO_SHOOT:
				if (!enemys.empty())
				{
					for (Enemy& e : enemys)
					{
						if (isRectCollision(
							0, 0, window_width, window_height,
							e.cord_x, e.cord_y, e.sprite_x, e.sprite_y
							) && !e.wasAutoShooted)
						{
							spawnAmmo(e.cord_x + e.sprite_x / 2, e.cord_y + e.sprite_y / 2);
							e.wasAutoShooted = true;
						}
					}
				}
				break;
			case AbilityType::SHIELD:
				
				break;
			case AbilityType::SLOW_MOTION:

				break;
			case AbilityType::JETPACK:

				break;
			case AbilityType::ROCKET:

				break;
			default:
				break;
			}
		}

		//move plarform
		for(Platform &p : platforms)
		{
			if (p.type == PlatformType::MOVE)
			{
				p.cord_x += p.horizontal_move;
				if (p.cord_x <= 0 || p.cord_x >= window_width - p.sprite_x)
				{
					p.horizontal_move *= -1;
				}
			}
		}

		//move ammo
		if (ammos.size() > 0)
		{
			for (Ammo& a : ammos)
			{
				a.cord_x += a.speed_x;
				a.cord_y += a.speed_y;
			}
		}

		//ammo teleport on edge touch
		if (ammos.size() > 0)
		{
			for (Ammo& a : ammos)
			{
				int a_x = round(a.cord_x);
				if (a_x < 0 - a.sprite_x)
				{
					a.cord_x = window_width;
				}
				else if (window_width < a_x)
				{
					a.cord_x = 0 - a.sprite_x;
				}
			}
		}

		//player x platform collision
		if (main_hero.verticalCurrentMovementSpeed >= 0)
		{
			for (Platform& p : platforms)
			{
				if (isRectCollision(p.cord_x, p.cord_y, p.sprite_x, p.sprite_y,
					main_hero.cord_x, main_hero.cord_y + main_hero.collisionOffset_y, main_hero.sprite_x, main_hero.sprite_y - main_hero.collisionOffset_y))
				{
					if (main_hero.biggestPlatformId < p.id)
					{
						main_hero.biggestPlatformId = p.id;
					}
					if (p.type == PlatformType::ONETOUCH)
					{
						p.cord_x += window_width * 2;
					}
					heroJump(p.jumpBoost);
				}
			}
		}

		//player x holes collision
		if (!holes.empty())
		{
			int tmpHero_x = main_hero.cord_x + main_hero.sprite_x / 2;
			int tmpHero_y = main_hero.cord_y + main_hero.sprite_y / 2;
			for (Hole& h : holes)
			{
				int tmpHole_x = h.cord_x + h.sprite_x / 2;
				int tmpHole_y = h.cord_y + h.sprite_y / 2;
				int delta_x = tmpHero_x - tmpHole_x;
				int delta_y = tmpHero_y - tmpHole_y;
				float distance = sqrt((delta_x* delta_x) + (delta_y * delta_y));
				int power = h.field_radius - distance;
				if (power > 0 && distance != 0)
				{
					int power = h.field_radius - distance;
					float unit_vector_x = delta_x / distance;
					float unit_vector_y = delta_y / distance;
					float power_x = (power * unit_vector_x * h.holeDirection) / h.powerModifier;
					float power_y = (power * unit_vector_y * h.holeDirection) / h.powerModifier;

					main_hero.passive_force_x = power_x;
					main_hero.cord_x += main_hero.passive_force_x;

					main_hero.passive_force_y = power_y;
				}
			}
		}

		//player x ability collision
		if (main_hero.activeAbility == AbilityType::COUNT && !abilitys.empty())
		{
			abilitys_itr = abilitys.begin();
			for (abilitys_itr; abilitys_itr != abilitys.end(); abilitys_itr++)
			{
				if (isRectCollision(
					main_hero.cord_x, main_hero.cord_y, main_hero.sprite_x, main_hero.sprite_y,
					*&abilitys_itr->cord_x, *&abilitys_itr->cord_y, *&abilitys_itr->sprite_x, *&abilitys_itr->sprite_y
				))
				{
					main_hero.activeAbility = *&abilitys_itr->type;
					//ability start
					switch (main_hero.activeAbility)
					{
					case AbilityType::AUTO_SHOOT:
						main_hero.abilityTimer.setFunc([&]() {
							if (!main_hero.abilityFirstRun)
							{
								main_hero.activeAbility = AbilityType::COUNT;
								main_hero.sprite = res.spriteMainHero;
								main_hero.abilityFirstRun = true;
								main_hero.abilityTimer.stop();
							}
							else
							{
								main_hero.sprite = res.mainhero_abilities[(int)main_hero.activeAbility];
								main_hero.abilityFirstRun = false;
							}
							})
							->setInterval(20000)->start();
							break;
					case AbilityType::SLOW_MOTION:
						main_hero.abilityTimer.setFunc([&]() {
							if (!main_hero.abilityFirstRun)
							{
								main_hero.activeAbility = AbilityType::COUNT;
								main_hero.sprite = res.spriteMainHero;
								main_hero.abilityFirstRun = true;
								world_params.fpsLimiter = 90;
								world_params.targetFrameDelay = 1000 / world_params.fpsLimiter;
								main_hero.abilityTimer.stop();
							}
							else
							{
								world_params.fpsLimiter = 45;
								world_params.targetFrameDelay = 1000 / world_params.fpsLimiter;
								main_hero.sprite = res.mainhero_abilities[(int)main_hero.activeAbility];
								main_hero.abilityFirstRun = false;
							}
							})
							->setInterval(10000)->start();
							break;
					case AbilityType::SHIELD:
						main_hero.abilityTimer.setFunc([&]() {
							if (!main_hero.abilityFirstRun)
							{
								main_hero.activeAbility = AbilityType::COUNT;
								main_hero.sprite = res.spriteMainHero;
								main_hero.abilityFirstRun = true;
								main_hero.isCollisionOn = true;
								main_hero.abilityTimer.stop();
							}
							else
							{
								main_hero.isCollisionOn = false;
								main_hero.sprite = res.mainhero_abilities[(int)main_hero.activeAbility];
								main_hero.abilityFirstRun = false;
							}
							})
							->setInterval(20000)->start();
							break;
					case AbilityType::JETPACK:
						main_hero.abilityTimer.setFunc([&]() {
							if (!main_hero.abilityFirstRun)
							{
								main_hero.activeAbility = AbilityType::COUNT;
								main_hero.sprite = res.spriteMainHero;
								main_hero.abilityFirstRun = true;
								main_hero.isCollisionOn = true;
								main_hero.abilityTimer.stop();
							}
							else
							{
								main_hero.isCollisionOn = false;
								main_hero.verticalCurrentMovementSpeed = -8;
								main_hero.sprite = res.mainhero_abilities[(int)main_hero.activeAbility];
								main_hero.abilityFirstRun = false;
							}
							})
							->setInterval(3000)->start();
							break;
					case AbilityType::ROCKET:
						main_hero.abilityTimer.setFunc([&]() {
							if (!main_hero.abilityFirstRun)
							{
								main_hero.activeAbility = AbilityType::COUNT;
								main_hero.sprite = res.spriteMainHero;
								main_hero.abilityFirstRun = true;
								main_hero.isCollisionOn = true;
								main_hero.abilityTimer.stop();
							}
							else
							{
								main_hero.isCollisionOn = false;
								main_hero.verticalCurrentMovementSpeed = -8;
								main_hero.sprite = res.mainhero_abilities[(int)main_hero.activeAbility];
								main_hero.abilityFirstRun = false;
							}
							})
							->setInterval(8000)->start();
							break;
					default:
						break;
					}
					abilitys.erase(abilitys_itr);
					break;
				}
			}
		}

		//platform clean up and spawning new platforms
		if (platforms.size() > 0)
		{
			platforms_itr = platforms.begin();
			for (platforms_itr; platforms_itr != platforms.end(); platforms_itr++)
			{
				if (*&platforms_itr->cord_y > window_height)
				{
					platforms.erase(platforms_itr);
					world_params.platformDeleted++;
					spawnPlatform();
					calculateUiNumber(world_params.platformDeletedUi, world_params.platformDeletedUiLenght, world_params.platformDeleted);
					break;
				}
				else
				{
					break;
				}
			}
		}

		//ammo clean up
		if (ammos.size() > 0)
		{
			ammos_itr = ammos.begin();
			for (ammos_itr; ammos_itr != ammos.end(); ammos_itr++)
			{
				if (*&ammos_itr->cord_y > window_height || *&ammos_itr->cord_y < 0 - *&ammos_itr->sprite_y)
				{
					ammos.erase(ammos_itr);
					break;
				}
			}
		}

		//enemy clean up
		if (enemys.size() > 0)
		{
			enemys_itr = enemys.begin();
			for (enemys_itr; enemys_itr != enemys.end(); enemys_itr++)
			{
				if (*&enemys_itr->cord_y > window_height)
				{
					enemys.erase(enemys_itr);
					break;
				}
				else
				{
					break;
				}
			}
		}

		//holes clean up
		if (holes.size() > 0)
		{
			holes_itr = holes.begin();
			for (holes_itr; holes_itr != holes.end(); holes_itr++)
			{
				if (*&holes_itr->cord_y > window_height)
				{
					holes.erase(holes_itr);
					break;
				}
				else
				{
					break;
				}
			}
		}

		//enemy x ammo collision and enemy kill
		if (enemys.size() > 0 && ammos.size() > 0)
		{
			enemys_itr = enemys.begin();
			bool flag = false;
			for (enemys_itr; enemys_itr != enemys.end(); enemys_itr++)
			{
				ammos_itr = ammos.begin();
				for (ammos_itr; ammos_itr != ammos.end(); ammos_itr++)
				{
					if (isRectCollision(
						*&enemys_itr->cord_x, *&enemys_itr->cord_y, *&enemys_itr->sprite_x, *&enemys_itr->sprite_y,
						*&ammos_itr->cord_x, *&ammos_itr->cord_y, *&ammos_itr->sprite_x, *&ammos_itr->sprite_y
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
				enemys.erase(enemys_itr);
				ammos.erase(ammos_itr);
			}
		}

		//render all game objects
		Render();

		//all game over cases
		//player x enemy top collision and enemy kill
		if (main_hero.verticalCurrentMovementSpeed >= 0 && !enemys.empty())
		{
			enemys_itr = enemys.begin();
			for (enemys_itr; enemys_itr != enemys.end(); enemys_itr++)
			{
				if (isRectCollision(*&enemys_itr->cord_x, *&enemys_itr->cord_y, *&enemys_itr->sprite_x, *&enemys_itr->sprite_y - 65,
					main_hero.cord_x, main_hero.cord_y + main_hero.collisionOffset_y, main_hero.sprite_x, main_hero.sprite_y - main_hero.collisionOffset_y))
				{
					enemys.erase(enemys_itr);
					heroJump(-14);
					break;
				}
			}
		}
		//player x enemy collision and game end
		else if (!enemys.empty())
		{
			enemys_itr = enemys.begin();
			for (enemys_itr; enemys_itr != enemys.end(); enemys_itr++)
			{
				if (isRectCollision(*&enemys_itr->cord_x, *&enemys_itr->cord_y, *&enemys_itr->sprite_x, *&enemys_itr->sprite_y,
					main_hero.cord_x, main_hero.cord_y, main_hero.sprite_x, main_hero.sprite_y))
				{
					if (main_hero.isCollisionOn)
					{
						MyClose();
					}
					else
					{
						enemys.erase(enemys_itr);
						break;
					}
				}
			}
		}
		
		//player x fall out and game end
		if (main_hero.cord_y + main_hero.collisionOffset_y > window_height)
		{
			MyClose();
		}

		//make game process smooth
		int milliseconds_compensation = world_params.targetFrameDelay - (getTickCount() - world_params.prevTickCounter);
		if (milliseconds_compensation > 0)
		{
			sleep_for(milliseconds(milliseconds_compensation));
		}
		world_params.prevTickCounter = getTickCount();

		if (world_params.doExit)
		{
			ShowGG();
		}

		return world_params.doExit;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		main_hero.mouse_x = x;
		main_hero.mouse_y = y;
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
				spawnAmmo(main_hero.mouse_x, main_hero.mouse_y);
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

		enemys.push_back(dummyEnemy);

	}

	void spawnAbility()
	{
		Ability dummyAbility;
		int platformId = rand() % platforms.size();
		platforms_itr = platforms.begin();
		for (int i = 0; i < platformId; i++)
		{
			platforms_itr++;
		}

		dummyAbility.cord_x = *&platforms_itr->cord_x + 20;
		dummyAbility.cord_y = *&platforms_itr->cord_y - 60;
		dummyAbility.type = static_cast<AbilityType>(rand() % (int)AbilityType::COUNT);

		dummyAbility.sprite = res.abilities[(int)dummyAbility.type];

		abilitys.push_back(dummyAbility);
	}

	void spawnPlatform(bool isRandomPosition = true, int x = 0, int y = 0,
		bool isRandomType = true, PlatformType pType = PlatformType::REGULAR)
	{
		Platform dummyPlatform;

		//id inc
		if (platforms.size() > 0)
		{
			platforms_itr = platforms.end();
			platforms_itr--;
			dummyPlatform.id = platforms_itr->id++;
		}
		else
		{
			dummyPlatform.id = 0;
		}

		//position
		if (isRandomPosition)
		{
			dummyPlatform.cord_x = rand() % window_width;
			if (dummyPlatform.cord_x > window_width - 80)
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
			if (randValue <= 40)
			{
				dummyPlatform.type = PlatformType::REGULAR;
			}
			else if (randValue <= 60)
			{
				dummyPlatform.type = PlatformType::MOVE;
			}
			else if (randValue <= 75)
			{
				dummyPlatform.type = PlatformType::BOOST;
			}
			else if (randValue <= 85)
			{
				dummyPlatform.type = PlatformType::ONETOUCH;
			}
			else if (randValue <= 95)
			{
				dummyPlatform.type = PlatformType::ENEMY;
				spawnEnemy(dummyPlatform.cord_x, dummyPlatform.cord_y);
			}
			else
			{
				dummyPlatform.type = PlatformType::HOLE;
				spawnHole(dummyPlatform.cord_x, dummyPlatform.cord_y);
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
		case PlatformType::HOLE:
			dummyPlatform.sprite = res.spritePlarfotmHole;
			dummyPlatform.jumpBoost = -8;
			break;
		case PlatformType::ONETOUCH:
			dummyPlatform.sprite = res.spritePlarfotmOneTouch;
			dummyPlatform.jumpBoost = -8;
			break;
		case PlatformType::MOVE:
			dummyPlatform.sprite = res.spritePlarfotmMove;
			dummyPlatform.jumpBoost = -8;
			dummyPlatform.horizontal_move = -3 + rand() % 4;
			if (dummyPlatform.horizontal_move == 0)
			{
				dummyPlatform.horizontal_move = 1;
			}
			break;
		default:
			Log("!!!Error on plarform type choosing!!!");
			break;
		}

		platforms.push_back(dummyPlatform);
	}

	void spawnAmmo(int targetX, int targetY)
	{
		Ammo ammoDummy;
		ammoDummy.sprite = res.spriteAmmo;

		ammoDummy.cord_x = main_hero.cord_x + main_hero.bulletOffset;
		ammoDummy.cord_y = main_hero.cord_y + main_hero.bulletOffset;
		float v2x = (targetX - ammoDummy.cord_x);
		float v2y = (targetY - ammoDummy.cord_y);
		float magn = sqrt((v2x * v2x) + (v2y * v2y));
		ammoDummy.unit_vector_x = v2x / magn;
		ammoDummy.unit_vector_y = v2y / magn;
		ammoDummy.speed_x = ammoDummy.unit_vector_x * ammoDummy.basicSpeed;
		ammoDummy.speed_y = ammoDummy.unit_vector_y * ammoDummy.basicSpeed;

		ammos.push_back(ammoDummy);
	}

	void spawnHole(int x, int y)
	{
		int randomDirection = 1;
		Sprite* randSprite = res.spriteWhiteHole;
		if (rand() % 2 == 0)
		{
			randomDirection = -1;
			randSprite = res.spriteBlackHole;
		}

		Hole dummyHole = Hole(x, y, randSprite, 400 + rand() % 200, randomDirection);

		dummyHole.cord_x += 5;
		dummyHole.cord_y -= 100;

		holes.push_back(dummyHole);
	}

	void startWorldCtr()
	{
		spawnPlatform(false, window_width / 2, window_height - world_params.bottomOffset + 50, false, PlatformType::REGULAR);
		int y = world_params.bottomOffset - 100;
		while (window_height - y > 0)
		{
			spawnPlatform(false, window_width / 2 - 150, window_height - y, false, PlatformType::ONETOUCH);
			y += 100;
		}
		spawnPlatform(false, window_width / 2 - 150, window_height - y, false, PlatformType::BOOST);
		//spawnHole(500, 300);
	}

	void heroJump(int jumpBoost)
	{
		main_hero.verticalCurrentTickTimer = main_hero.verticalBasicTickTimer;
		main_hero.verticalCurrentMovementSpeed = jumpBoost;
		main_hero.jumpCounter++;
		if (main_hero.jumpCounter % main_hero.jumpsToSpawnAbility == 0)
		{
			spawnAbility();
		}
	}

	virtual void onKeyPressed(FRKey k)
	{
		switch (k)
		{
		case FRKey::RIGHT:
			main_hero.rightPressed = true;
			if (main_hero.moveState == HeroMoveState::IDLE)
			{
				main_hero.moveState = HeroMoveState::RIGHT;
			}
			break;
		case FRKey::LEFT:
			main_hero.leftPressed = true;
			if (main_hero.moveState == HeroMoveState::IDLE)
			{
				main_hero.moveState = HeroMoveState::LEFT;
			}
			break;
		case FRKey::DOWN:
			run_again = false;
			world_params.doExit = true;
			break;
		case FRKey::UP:
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
			main_hero.rightPressed = false;
			if (main_hero.moveState == HeroMoveState::RIGHT)
			{
				if (main_hero.leftPressed)
				{
					main_hero.moveState = HeroMoveState::LEFT;
				}
				else
				{
					main_hero.moveState = HeroMoveState::IDLE;
				}
			}
			break;
		case FRKey::LEFT:
			main_hero.leftPressed = false;
			if (main_hero.moveState == HeroMoveState::LEFT)
			{
				if (main_hero.rightPressed)
				{
					main_hero.moveState = HeroMoveState::RIGHT;
				}
				else
				{
					main_hero.moveState = HeroMoveState::IDLE;
				}
			}
			break;
		case FRKey::DOWN:
			break;
		case FRKey::UP:
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

	/*cout << "Input game window Width (recommended 600): ";
	getline(std::cin, input);

	while (!tryParse(input, w))
	{
		std::cout << "Bad entry. Enter a NUMBER (recommended 600): ";
		getline(std::cin, input);
	}

	cout << "Input game window Height (recommended 800): ";
	getline(std::cin, input);

	while (!tryParse(input, h))
	{
		std::cout << "Bad entry. Enter a NUMBER (recommended 800): ";
		getline(std::cin, input);
	}*/

	while (run_again)
	{
		run(new game(w, h));
	}

	return 0;
}