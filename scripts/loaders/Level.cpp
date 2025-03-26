#include "Level.h"



static const std::vector<Level::LevelInfo> Level::Levels{
	// level one
	{ 1, "Level one", 3, std::vector<ObjectInfo>{

		{ {5.f,10.f} , M_PI * .5f, ObjectType::woodStick},
		{ {14.f,10.f} , M_PI * .5f, ObjectType::woodStick},
		{ {9.5f,15.5f} , 0.f, ObjectType::woodStick},
		{ {6.f,21.f} , M_PI * .5f, ObjectType::woodStick},
		{ {13.f,21.f} , M_PI * .5f, ObjectType::woodStick},
		{ {9.5f,26.5f} , 0.f, ObjectType::woodStick},
		{ {9.5f,17.5f} , 0.f, ObjectType::enemy},
	} },
	// level two
			// start 6 - 16
	{ 2, "Level two", 3, std::vector<ObjectInfo>{
		{ {6.5f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {15.5f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {11.f,13.5f} , 0.f, ObjectType::woodStick},
		{ {11.f,6.5f} , 0.f, ObjectType::enemy},
		{ {7.75f,18.f} , M_PI * .5f, ObjectType::woodStick},
		{ {14.25f,18.f} , M_PI * .5f, ObjectType::woodStick},
		{ {11.f,24.5f} , 0.f, ObjectType::woodStick},
		{ {11.f,15.5f} , 0.f, ObjectType::enemy},
	} },
	// level three
	{ 3, "Three in a row", 3, std::vector<ObjectInfo>{


		{ {10.f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {10.f,11.f} , 0.f, ObjectType::woodThickStick},
		{ {10.f,12.5f} , 0.f, ObjectType::enemy},

		{ {25.f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {25.f,11.f} , 0.f, ObjectType::woodThickStick},
		{ {25.f,12.5f} , 0.f, ObjectType::enemy},


		{ {40.f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {40.f,11.f} , 0.f, ObjectType::woodThickStick},
		{ {40.f,12.5f} , 0.f, ObjectType::enemy},

	} },

	// level four 
	{ 4, "perfect aim", 3, std::vector<ObjectInfo>{

		{ {20.f,6.5f} , 0.f, ObjectType::enemy},

		{ {15.5f,13.f} , M_PI * .5f, ObjectType::stonePillar,true},
		{ {24.5f,13.f} , M_PI * .5f, ObjectType::stonePillar,true},
	}},
	{ 5, "Level five", 3, std::vector<ObjectInfo>{
		{ {21.f,15.5f} , 0.f, ObjectType::enemy},

		{ {17.f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {25.f,9.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {21.f,13.5f} , 0.f, ObjectType::stoneStick},
		{ {17.f,18.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {25.f,18.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {21.f,22.5f} , 0.f, ObjectType::stoneStick},
		{ {17.f,27.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {25.f,27.f} , M_PI * .5f, ObjectType::stoneThickStick},
		{ {21.f,31.5f} , 0.f, ObjectType::stoneStick},
	} },



};

void Level::LoadLevel(PhysicsWorld& world, const LevelInfo& level, sf::Vector2f worldSize)
{
	// ground
	PhysicsBody* body = new PhysicsBody{ ShapeType::Box,0.f, worldSize.x, 10.f,true,MaterialType::Ground  };
	body->moveTo(sf::Vector2f(0.f, 0.f));
	world.addObject(body);

	for (unsigned int i{ 0 }; i < level.objects.size(); i++) {
		ObjectLoader::loadObject(level.objects[i], world);
	}

	// TODO
	world.setLevelName(level.name);
	world.setPlayerLives(level.playerLives);

}
