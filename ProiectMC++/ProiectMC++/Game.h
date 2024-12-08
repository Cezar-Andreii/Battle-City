#pragma once

#include "Bullet.h"
#include "Player.h"
#include "Map.h"
#include "Wall.h"
#include "Level.h"
#include "BulletManager.h"

class Game
{
private:
	Map gameMap;
	Player player;
	BulletManager bulletManager;
	int checkWinner();
	bool m_isGameOver = false;

public:
	Game();
	void start();
	void update();
	void handleInput(Player& player);
	void handleExplosion(Wall& wall);
	void render();

private:
	Map m_map;
	std::vector<Bullet> m_bullets;
	std::vector<Player> m_players;
	std::vector<Wall> m_walls;

};
