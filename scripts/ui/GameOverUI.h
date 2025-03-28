#pragma once
#include "Button.h"
#include "../loaders/FontLoader.h"
#include "../loaders/AudioLoader.h"
#include "SFML/Graphics.hpp"

class GameOverUI
{
public:
	GameOverUI(sf::Vector2f screenSize);

	void setValues(bool wonGame, int score);
	void update(float delta);
	void draw(sf::RenderWindow& window);
	bool nextRetryButtonClicked(sf::Vector2f mousePos);
	bool mainMenuButtonClicked(sf::Vector2f mousePos);

private:

	bool wonGame{};

	float timeSinceSettingValues{};
	int score{};

	sf::RectangleShape backgroundShape;
	Button mainMenuButton;
	Button nextRetryButton;
	sf::Text messageText;
	sf::Text scoreText;
	sf::Sound clickSound;
};

