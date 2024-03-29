#include "parseConfig.h"

static std::vector<std::string> split(const std::string& str, std::string splitChar = " \t") {
    std::vector<std::string> result;
    int n;
    int offset;
    int size;

    n = 0;
    size = (int)str.size();
    while (n < size) {
        while (splitChar.find(str[n]) != std::string::npos && n < size)
            n++;
        offset = n;
        while (splitChar.find(str[n]) == std::string::npos && n < size)
            n++;
        if (offset < n)
            result.push_back(str.substr(offset, n - offset));
    }
    return result;
}

static inline bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

static SoundsData parseSoundData() {
    std::ifstream               settingsFile(SOUND_SETTING_FILE); //TODO change to relatif path
    std::vector<std::string>    splitLine;
    SoundsData                  soundsData;
    std::string                 currentName;
    unsigned                    lineNb;

    lineNb = 0;
    for (std::string line; std::getline(settingsFile, line); ) {
        splitLine = split(line);
        lineNb++;
        if (splitLine.size() <= 1)
            continue;
        if (splitLine[0] == "Name")
            currentName = splitLine[1];
        else if (splitLine[0] == "Files" && !currentName.empty()) {
            splitLine = split(line, " \t,");
            for (unsigned n = 1; n < splitLine.size(); n++) {
                std::string file = SOUND_FOLDER + splitLine[n];
                if (file_exists(file))
                    soundsData[currentName].Add(file.c_str());
            }
        }
        else {
            std::cerr << SOUND_SETTING_FILE << " : Line " << lineNb 
                << " : The keyWord '" << splitLine[0] << "' is not valid" << std::endl;
        }
    }
    return soundsData;
}

static void updateTextureList(const std::string &textureFile, const std::string& textureType, BlockData& blockData, std::vector<std::string>& textures) {
    auto it = std::find(textures.begin(), textures.end(), textureFile);
    char index;

    if (it != textures.end())
        index = (char)(it - textures.begin());
    else {
        index = (char)textures.size();
        textures.push_back(textureFile);
    }
    if (textureType == "Top")
        blockData.topTexID = index;
    else if (textureType == "Side")
        blockData.sideTexID = index;
    else if (textureType == "Bottom")
        blockData.bottomTexID = index;
}

static std::vector<BlockData> parseBlockData(std::vector<std::string>& textures) {
    std::ifstream               settingsFile(CUBES_SETTING_FILE); //TODO change to relatif path
    std::vector<std::string>    splitLine;
    std::vector<BlockData>      blocksData;
    BlockData                   blockData;
    unsigned                    lineNb;

    lineNb = 0;
    for (std::string line; std::getline(settingsFile, line); ) {
        splitLine = split(line);
        lineNb++;
        if (splitLine.size() <= 1)
            continue;
        if (splitLine[0] == "Name") {
            if (!blockData.name.empty()) {
                blocksData.push_back(blockData);
                blockData.clear();
            }
            blockData.name = splitLine[1];
        }
        else if (splitLine[0] == "ID" && std::isdigit(splitLine[1][0]))
            blockData.id = std::stoi(splitLine[1]);
        else if (splitLine[0] == "Top" || splitLine[0] == "Side" || splitLine[0] == "Bottom")
            updateTextureList(splitLine[1], splitLine[0], blockData, textures);
        else if (splitLine[0] == "Break")
            blockData.breakSound = splitLine[1];
        else if (splitLine[0] == "Step")
            blockData.stepSound = splitLine[1];
        else {
            std::cerr << CUBES_SETTING_FILE << " : Line " << lineNb
                << " : The keyWord '" << splitLine[0] << "' is not valid" << std::endl;
        }
    }
    if (!blockData.name.empty())
        blocksData.push_back(blockData);
    return blocksData;
}

static Block blockDataMapper(const BlockData& blockData, const  SoundsData& soundsData, const Sound& sound) {
    Block block;

    block.SetTextures(blockData.topTexID, blockData.sideTexID, blockData.bottomTexID);
    auto soundData = soundsData.find(blockData.breakSound);
    if (soundData != soundsData.end()) {
        block.breakSounds = soundData->second;
        block.soundSources = &sound.sources;
    }
    soundData = soundsData.find(blockData.stepSound);
    if (soundData != soundsData.end()) {
        block.stepSounds = soundData->second;
        block.soundSources = &sound.sources;
    }
    return block;
}

void parseConfigs(std::vector<std::string>& textures, Sound& sound) {
    std::vector<BlockData>      blocksData;
    SoundsData                  soundsData;

    blocksData = parseBlockData(textures);
    soundsData = parseSoundData();

    for (const BlockData& blockData : blocksData)
        if (blockData.id > 0 && blockData.id < MAX_BLOCK_NB)
            Chunk::blocks[blockData.id] = blockDataMapper(blockData, soundsData, sound);
    for (const auto& soundData : soundsData)
        sound.AddSounds(soundData.second);
}