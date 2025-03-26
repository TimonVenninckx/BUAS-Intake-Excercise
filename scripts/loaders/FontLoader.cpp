#include "FontLoader.h"


// font used https://www.dafont.com/angrybirds.font

namespace FontLoader {
	sf::Font* getFont(std::string fileLocation) {

		if (allFonts.find(fileLocation) != allFonts.end()) {
			return allFonts.at(fileLocation);
		}
		sf::Font* font = new sf::Font;
		font->openFromFile("fonts/" + fileLocation);
		allFonts.emplace(fileLocation, font);
		return font;
	}

}