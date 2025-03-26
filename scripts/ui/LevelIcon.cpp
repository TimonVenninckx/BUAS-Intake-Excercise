#include "LevelIcon.h"

LevelIcon::LevelIcon(const Level::LevelInfo& info, sf::Vector2f position) :
	levelInfo{ info }, text {*FontLoader::getFont("angrybirds.ttf")},
	clickSound{ *AudioLoader::getAudio("click.mp3") }
{
	text.setCharacterSize(50);
	text.setString(std::to_string(info.id));
	text.setOrigin(text.getGlobalBounds().getCenter());
	text.setPosition(position);

	backgroundShape = sf::RectangleShape({ 130.f, 130.f });
	backgroundShape.setFillColor(sf::Color(30, 30, 30, 150));
	backgroundShape.setOrigin(backgroundShape.getGlobalBounds().size / 2.f);
	backgroundShape.setPosition(position);
	clickSound.setVolume(100.f);

}

void LevelIcon::draw(sf::RenderWindow& window)
{
	window.draw(backgroundShape);
	window.draw(text);
}

bool LevelIcon::contains(sf::Vector2f mousePos)
{
	if (backgroundShape.getGlobalBounds().contains(mousePos)) {
		clickSound.play();
		return true;
	}
	return false;
}
