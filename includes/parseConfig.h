#ifndef PARSE_CONFIG_H
# define PARSE_CONFIG_H

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "chunk.h"
#include "sound.h"

#define CUBES_SETTING_FILE	"cubes_settings"
#define SOUND_SETTING_FILE	"sounds_settings"
#define SOUND_FOLDER		"sound/"

// maybe have a configData that contains all configs data and then distribute the data to eatch modules that need the data

typedef std::unordered_map<std::string, SoundBuffers> SoundsData;
/*
struct SoundData {
	std::string					name;
	SoundBuffers				sounds;

	void clear() {
		name.clear();
		sounds.clear();
	}
};*/

struct BlockData {
	std::string					name;
	unsigned					id;
	unsigned					topTexID;
	unsigned					sideTexID;
	unsigned					bottomTexID;
	std::string					breakSound;
	std::string					stepSound;

	BlockData() {
		id = 0;
		topTexID = 0;
		sideTexID = 0;
		bottomTexID = 0;
	}

	void clear() {
		id = 0;
		topTexID = 0;
		sideTexID = 0;
		bottomTexID = 0;
		name.clear();
		breakSound.clear();
		stepSound.clear();
	}
};

void parseConfigs(std::vector<std::string>& textures, Sound& sound);
//std::vector<BlockData> parseBlockData(std::vector<std::string>& textures);

#endif