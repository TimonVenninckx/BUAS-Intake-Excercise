#pragma once
#include <SFML/Graphics.hpp>
#include "objects/PhysicsBody.h"
#include "PhysicsWorld.h"
#include "collision/Collision.h"
#include "PhysicsMath.h"
#include "ui/MainMenu.h"
#include "loaders/Level.h"
#include "loaders/TextureLoader.h"


class Game
{
public:
	Game();
	~Game();


private:


	void update(float delta);
	void draw();

	sf::RenderWindow window{};

	sf::Vector2f viewSize{};

	sf::View view{};
	sf::View uiView{};

	bool inLevel{ false };
	MainMenu mainMenu;

	PhysicsWorld world;
	std::vector<sf::Color> colors;
	sf::Clock clock;

	sf::Text* frameCounterText;

	float acumulatedPhysicsDelta{};

};

