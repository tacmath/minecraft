#include <fstream>
#include <vector>
#include <string>
#include "chunk.h"

static void split(std::string& str, std::vector<std::string>& result) {
    int n;
    int offset;
    int size;

    result.clear();
    n = 0;
    size = (int)str.size();
    while (n < size) {
        while ((str[n] == ' ' || str[n] == '\t') && n < size)
            n++;
        offset = n;
        while (str[n] != ' ' && str[n] != '\t' && n < size)
            n++;
        if (offset < n)
            result.push_back(str.substr(offset, n - offset));
    }
}

void parseBlockData(std::vector<std::string>& textures) {
    std::ifstream   infile("cubes_settings");
    std::vector<std::string> splitLine;
    Block           block;
    unsigned char   blockID;

    block.SetTextures(0, 0, 0);
    blockID = 0;

    for (std::string line; std::getline(infile, line); ) {
        if (!line.size())
            continue;
        if (line.substr(0, 4) == "Name") {
            Chunk::blocks[blockID] = block;
            blockID = 0;
            memset(&block, 0, sizeof(block));
        }
        else if (line.substr(1, 2) == "ID")
            blockID = std::stoi(line.substr(3, line.size() - 3));
        else {
            split(line, splitLine);
            if (splitLine.size() < 2)
                continue;

            if (splitLine[0] == "Top" || splitLine[0] == "Side" || splitLine[0] == "Bottom") {
                auto it = std::find(textures.begin(), textures.end(), splitLine[1]);
                char index;

                if (it != textures.end())
                    index = (char)(it - textures.begin());
                else {
                    index = (char)textures.size();
                    textures.push_back(splitLine[1]);
                }
                block.SetTexture(splitLine[0], index);
            }
            else if (splitLine[0] == "Break") {
                block.soundBuffer.Load(splitLine[1].c_str());
                block.sound.Gen();
                
            }
        }
    }
    Chunk::blocks[blockID] = block;
}