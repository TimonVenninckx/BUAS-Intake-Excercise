#pragma once
#include "LevelIcon.h"

class MainMenu
{
public:
	MainMenu();

	const Level::LevelInfo* ChooseLevel(sf::Vector2f mousePos);
	void draw(sf::RenderWindow& window);

private:
	sf::RectangleShape background;
	std::vector<LevelIcon> levelsUI;
	sf::Text headerText;
};

