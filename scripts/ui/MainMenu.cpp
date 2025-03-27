#include "MainMenu.h"
#include "../loaders/FontLoader.h"
#include "../loaders/LevelLoader.h"

MainMenu::MainMenu() :
	headerText {*FontLoader::getFont("angrybirds.ttf")}
{
	// setup level UI
	for (unsigned int i{ 0 }; i < LevelLoader::Levels.size(); i++) {
		Button button{ std::to_string(LevelLoader::Levels[i].id), sf::Vector2f{150.f,150.f},
			sf::Vector2f{200.f + (i % 5 * 250.f), 200.f + 200.f * static_cast<float>(i / 5)},70};
		this->buttons.push_back(button);
	}

	background = sf::RectangleShape({ 1600.f,900.f });
	background.setTexture(TextureLoader::LoadTexture("mainMenuBackground.png"));

	// fixes ui screen size 1600.f, 900.f
	// text positoin WILL NOT scale but thats fine for this fixed view game
	headerText.setPosition({ 800.f, 50.f });
	headerText.setCharacterSize(80);
	headerText.setString("Angry Faces");
	headerText.setOrigin(headerText.getGlobalBounds().size / 2.f);
}

const LevelLoader::LevelInfo* MainMenu::ChooseLevel(sf::Vector2f mousePos) {

	for (unsigned int i{ 0 }; i < buttons.size(); i++) {
		if (buttons[i].contains(mousePos)) {
			return &LevelLoader::Levels[i];;
		}
	}
	return nullptr;
}

void MainMenu::draw(sf::RenderWindow& window)
{
	window.draw(background);
	for (Button& button: this->buttons) {
		button.draw(window);
	}
	window.draw(headerText);
}
