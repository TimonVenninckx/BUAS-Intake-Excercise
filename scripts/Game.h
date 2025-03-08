#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsBody.h"


class Game
{
public:
	Game();
	~Game();


	void Update(float delta);
	void Draw();

	void TryPlace(sf::Vector2f position);

		
private:
	sf::RenderWindow window{};
	sf::View view{};

	sf::Vertex line[2];
	sf::Vertex line2[2];

	
	std::vector<PhysicsBody*> bodylist;
	std::vector<sf::Color> colors;
	sf::Clock clock;
};

