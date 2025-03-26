#pragma once
#include "../loaders/Level.h"
#include "../loaders/FontLoader.h"
#include "../loaders/AudioLoader.h"

class LevelIcon {

public:
	const Level::LevelInfo& levelInfo;
	LevelIcon(const Level::LevelInfo& info, sf::Vector2f position);

	void draw(sf::RenderWindow& window);
	bool contains(sf::Vector2f mousePos);

private:
	sf::RectangleShape backgroundShape;
	sf::Text text;
	sf::Sound clickSound;
};