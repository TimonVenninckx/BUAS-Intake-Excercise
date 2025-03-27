#pragma once
#include "../objects/PhysicsBody.h"
#include "ObjectLoader.h"
#include "../PhysicsWorld.h"



namespace LevelLoader {
	struct LevelInfo
	{
		int id{};
		std::string name{};
		int playerLives{};
		std::vector<ObjectInfo> objects{};
	};

	void LoadLevel(PhysicsWorld& world, const LevelInfo& level, sf::Vector2f worldSize);

	// set in level.cpp
	extern const std::vector<LevelInfo> Levels;
}



