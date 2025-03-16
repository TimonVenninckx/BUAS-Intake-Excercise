#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsBody.h"
#include "PhysicsWorld.h"


class Game
{
public:
	Game();
	~Game();


	void update(float delta);
	void draw();

		
private:


	sf::RenderWindow window{};

	sf::View view{};
	sf::View uiView{};

	PhysicsWorld world;

	
	std::vector<sf::Color> colors;
	sf::Clock clock;

	sf::Font font;
	sf::Text* frameCounterText;
	sf::Text* worldStepTimeText;
	sf::Text* bodyCountText;



	// for debugging
	int totalSampleCount;
	int totalBodyCount;
	float totalWorldStepTime;
	sf::Clock sampleTimer;

};

