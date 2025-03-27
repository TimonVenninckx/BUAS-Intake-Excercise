#include "AudioLoader.h"


sf::SoundBuffer* AudioLoader::getAudio(std::string fileLocation)
{
	if (allSounds.find(fileLocation) != allSounds.end()) {
		return allSounds.at(fileLocation);
	}
	sf::SoundBuffer* soundBuffer = new sf::SoundBuffer;
	// always add to allsounds so we don't call loadfromfile
	// but just read the empty sound next time
	soundBuffer->loadFromFile("sounds/" + fileLocation);
	allSounds.emplace(fileLocation, soundBuffer);
	return soundBuffer;
}
