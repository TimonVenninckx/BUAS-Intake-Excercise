#pragma once
#include <SFML/Graphics.hpp>
#include <map>

namespace FontLoader {


	static std::map<std::string, sf::Font*> allFonts;
	sf::Font* getFont(std::string font);
}