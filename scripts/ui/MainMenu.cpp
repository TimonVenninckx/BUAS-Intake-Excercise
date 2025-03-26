#include "MainMenu.h"

MainMenu::MainMenu() :
	headerText {*FontLoader::getFont("angrybirds.ttf")}
{
	// setup level UI
	for (unsigned int i{ 0 }; i < Level::Levels.size(); i++) {
		LevelIcon icon{ Level::Levels[i], {200.f + (i * 250.f), 200.f} };
		this->levelsUI.push_back(icon);
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

const Level::LevelInfo* MainMenu::ChooseLevel(sf::Vector2f mousePos) {

	for (LevelIcon& icon : this->levelsUI) {
		if (icon.contains(mousePos)) {
			return &icon.levelInfo;
		}
	}
	return nullptr;
}

void MainMenu::draw(sf::RenderWindow& window)
{
	window.draw(background);
	for (LevelIcon& icon : this->levelsUI) {
		icon.draw(window);
	}
	window.draw(headerText);
}
