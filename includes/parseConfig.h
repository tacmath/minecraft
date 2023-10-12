#ifndef PARSE_CONFIG_H
# define PARSE_CONFIG_H

#include <fstream>
#include <vector>
#include <string>
#include "chunk.h"

#define CUBES_SETTING_FILE	"cubes_settings"
#define SOUND_SETTING_FILE	"sounds_settings"
#define SOUND_FOLDER		"sound/"


struct SoundData {
	std::string					name;
	std::vector<std::string>	soundFiles;

	void clear() {
		name.clear();
		soundFiles.clear();
	}
};

struct BlockData {
	std::string					name;
	unsigned					id;
	unsigned					topTexID;
	unsigned					sideTexID;
	unsigned					bottomTexID;
	std::string					breakSound;

	BlockData() {
		id = 0;
	}

	void clear() {
		id = 0;
		topTexID = 0;
		sideTexID = 0;
		bottomTexID = 0;
		name.clear();
		breakSound.clear();
	}
};

void parseConfigs(std::vector<std::string>& textures);

#endif