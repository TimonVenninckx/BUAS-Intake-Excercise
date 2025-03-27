#pragma once
#include "../loaders/LevelLoader.h"

class MainMenu
{
public:
	MainMenu();

	const LevelLoader::LevelInfo* ChooseLevel(sf::Vector2f mousePos);
	void draw(sf::RenderWindow& window);

private:
	sf::RectangleShape background;
	std::vector<Button> buttons;
	sf::Text headerText;
};

