#include "AudioLoader.h"

sf::SoundBuffer* AudioLoader::getAudio(std::string fileLocation)
{
	if (allSounds.find(fileLocation) != allSounds.end()) {
		return allSounds.at(fileLocation);
	}
	sf::SoundBuffer* soundBuffer = new sf::SoundBuffer;
	soundBuffer->loadFromFile("sounds/" + fileLocation);
	allSounds.emplace(fileLocation, soundBuffer);
	return soundBuffer;
}
