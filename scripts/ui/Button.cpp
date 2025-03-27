#include "Button.h"

Button::Button(std::string text, sf::Vector2f size, sf::Vector2f position, unsigned int fontSize, sf::Color color)
	: buttonText{ *FontLoader::getFont("angrybirds.ttf"),text, fontSize }
	, clickSound{ *AudioLoader::getAudio("click.mp3")}
{
	backgroundShape = sf::RectangleShape(size);
	backgroundShape.setPosition(position);
	backgroundShape.setFillColor(color);
	backgroundShape.setOrigin(backgroundShape.getGeometricCenter());

	buttonText.setOrigin(buttonText.getGlobalBounds().getCenter());
	buttonText.setPosition(position);
	clickSound.setVolume(AudioLoader::Volume);
}

bool Button::contains(sf::Vector2f mousePos)
{
	if (backgroundShape.getGlobalBounds().contains(mousePos)) {
		clickSound.play();
		return true;
	}
	return false;
}

void Button::draw(sf::RenderWindow& window)
{
	window.draw(backgroundShape);
	window.draw(buttonText);
}
