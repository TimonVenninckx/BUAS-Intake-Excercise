#include "GameOverUI.h"
#include <sstream>

GameOverUI::GameOverUI(sf::Vector2f screenSize)
	: messageText{ *FontLoader::getFont("angrybirds.ttf"),"WINNER",80 }
	, scoreText{ *FontLoader::getFont("angrybirds.ttf"),"Score: 10",50 }
	, nextRetryButton{ "Next Level",{ 180.f,70.f },{ (screenSize / 2.f) + sf::Vector2f{  100.f, 150.f } },40,sf::Color(60,60,60,255)}
	, mainMenuButton{ "Main Menu"  ,{ 180.f,70.f },{ (screenSize / 2.f) + sf::Vector2f{ -100.f, 150.f } },40,sf::Color(60,60,60,255) }
	, clickSound{ *AudioLoader::getAudio("click.mp3") }
{
	clickSound.setVolume(AudioLoader::Volume);

	sf::Vector2f center = screenSize / 2.f;

	backgroundShape.setSize({ 500.f,400.f });
	backgroundShape.setPosition(center);
	backgroundShape.setFillColor(sf::Color(30,30,30,200));
	backgroundShape.setOrigin(backgroundShape.getGeometricCenter());



	messageText.setPosition(center - sf::Vector2f{ 0.f, 100.f });
	scoreText.setPosition(center);
}



void GameOverUI::setValues(bool wonGame, int score)
{
	this->score = score;
	timeSinceSettingValues = 0.f;
	if (wonGame) {
		messageText.setString("YOU WON");
		messageText.setOrigin(messageText.getLocalBounds().getCenter());
		messageText.setFillColor(sf::Color::Green);
		nextRetryButton.setString("Next Level");
	}
	else {
		messageText.setString("YOU LOST");
		messageText.setOrigin(messageText.getLocalBounds().getCenter());
		messageText.setFillColor(sf::Color::Red);
		nextRetryButton.setString("Retry");
	}
}

void GameOverUI::update(float delta)
{
	timeSinceSettingValues += delta;
	if (timeSinceSettingValues > 1.f)
		scoreText.setString("Score: " + std::to_string(score));
	else {
		scoreText.setString("Score: " + std::to_string(static_cast<int>(score * timeSinceSettingValues)));
		scoreText.setOrigin(scoreText.getLocalBounds().getCenter());
	}
}




void GameOverUI::draw(sf::RenderWindow& window)
{
	window.draw(backgroundShape);
	mainMenuButton.draw(window);
	nextRetryButton.draw(window);
	window.draw(messageText);
	window.draw(scoreText);
}

bool GameOverUI::nextRetryButtonClicked(sf::Vector2f mousePos)
{
	return nextRetryButton.contains(mousePos);
}

bool GameOverUI::mainMenuButtonClicked(sf::Vector2f mousePos)
{
	return mainMenuButton.contains(mousePos);
}
