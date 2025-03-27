#pragma once
#include "../loaders/FontLoader.h"
#include "../loaders/AudioLoader.h"
#include "SFML/Graphics.hpp"


class Button
{

public:
	Button(std::string text, sf::Vector2f size, sf::Vector2f position,unsigned int fontSize = 30, sf::Color color = sf::Color(30, 30, 30, 155));
	bool contains(sf::Vector2f mousePos);
	void draw(sf::RenderWindow& window);

private:

	sf::RectangleShape backgroundShape;
	sf::Text buttonText;
	sf::Sound clickSound;
};

